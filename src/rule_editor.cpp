
#include <flan/rule_editor.hpp>
#include <flan/rule_model.hpp>
#include <flan/rule_set.hpp>
#include <flan/valid_regular_expression_validator.hpp>
#include <QAction>
#include <QDialog>
#include <QFile>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStyle>
#include <QTextEdit>
#include <QVBoxLayout>

// Q_INIT_RESOURCE has to be called from the global namespace, hence this workaround.
inline void init_resource()
{
    Q_INIT_RESOURCE(libflan);
}

namespace
{
QByteArray get_pcre_cheatsheet_content()
{
    init_resource();
    QFile file(":/pcre_cheatsheet");
    file.open(QIODeviceBase::ReadOnly);
    auto data = file.readAll();
    file.close();

    return data;
}

//! Submit the \a mapper data and ensure the cursor position of the \a line_edit does not change in
//! the process.
//!
//! Because submitting the data will change the model, which will make the \a mapper update the
//! \a line_edit again (with the same string), it results in the cursor being moved to the end of
//! the \a line_edit. This prevents modifying the \a line_edit in the middle in a easy way (one can
//! only modify one character at the time).
//!
//! This function saves the cursor position of the \a line_edit before submitting the \a mapper data
//! and then restores the cursor position. This makes the \a line_edit much nicer to use when
//! coupled with a mapper.
void submit_data_and_restore_cursor(QDataWidgetMapper& mapper, QLineEdit& line_edit)
{
    auto cursor_position = line_edit.cursorPosition();
    mapper.submit();
    line_edit.setCursorPosition(cursor_position);
}
} // namespace

namespace flan
{
rule_editor_t::rule_editor_t(QWidget* parent)
    : QWidget{parent}
    , _name_edit{new QLineEdit}
    , _pattern_edit{new QLineEdit}
    , _tooltip_edit{new QLineEdit}
{
    QFormLayout* layout = new QFormLayout;
    layout->addRow(tr("Name"), _name_edit);
    layout->addRow(tr("Pattern"), _pattern_edit);
    layout->addRow(tr("Tooltip"), _tooltip_edit);

    setLayout(layout);

    auto show_cheatsheet_action = _pattern_edit->addAction(
        style()->standardIcon(QStyle::SP_DialogHelpButton), QLineEdit::TrailingPosition);
    show_cheatsheet_action->setToolTip(tr("PCRE Regular Expression Cheatsheet"));

    connect(
        show_cheatsheet_action,
        &QAction::triggered,
        this,
        &rule_editor_t::show_pcre_cheatsheet_dialog);

    _pattern_edit->setValidator(new valid_regular_expression_validator_t{_pattern_edit});
    connect(_pattern_edit, &QLineEdit::textEdited, this, &rule_editor_t::update_pattern_validity);
    update_pattern_validity();

    _tooltip_edit->setPlaceholderText(tr("Leave empty to default to the name"));

    connect(_name_edit, &QLineEdit::textEdited, this, [this]() {
        submit_data_and_restore_cursor(_mapper, *_name_edit);
    });
    connect(_pattern_edit, &QLineEdit::textEdited, this, [this]() {
        submit_data_and_restore_cursor(_mapper, *_pattern_edit);
    });
    connect(_tooltip_edit, &QLineEdit::textEdited, this, [this]() {
        submit_data_and_restore_cursor(_mapper, *_tooltip_edit);
    });
}

void rule_editor_t::set_model(QAbstractItemModel* model)
{
    _mapper.setModel(model);

    _mapper.addMapping(_name_edit, rule_model_t::name_column_index);
    _mapper.addMapping(_pattern_edit, rule_model_t::pattern_column_index);
    _mapper.addMapping(_tooltip_edit, rule_model_t::tooltip_column_index);

    update_pattern_validity();
}

void rule_editor_t::set_current_model_index(const QModelIndex& index)
{
    _mapper.setRootIndex(index.parent());
    _mapper.setCurrentModelIndex(index);

    // FIXME: this shouldn't access the pointer directly. A read only and hidden "node type" column
    // should be added in the model and queried here to know which node type the index refers to.
    auto node = static_cast<const base_node_t*>(index.constInternalPointer());
    if (node)
    {
        switch (node->type())
        {
        case node_type_t::base:
            _name_edit->setEnabled(false);
            _pattern_edit->setEnabled(false);
            _tooltip_edit->setEnabled(false);
            break;
        case node_type_t::group:
            _name_edit->setEnabled(true);
            _pattern_edit->setEnabled(false);
            _tooltip_edit->setEnabled(false);
            break;
        case node_type_t::rule:
            _name_edit->setEnabled(true);
            _pattern_edit->setEnabled(true);
            _tooltip_edit->setEnabled(true);
            break;
        }
    }
    else
    {
        _name_edit->setEnabled(false);
        _pattern_edit->setEnabled(false);
        _tooltip_edit->setEnabled(false);
    }

    update_pattern_validity();
}

void rule_editor_t::update_pattern_validity()
{
    QString text = _pattern_edit->text();
    int position = _pattern_edit->cursorPosition();
    switch (_pattern_edit->validator()->validate(text, position))
    {
    case QValidator::Invalid:
        [[fallthrough]];
    case QValidator::Intermediate:
        update_pattern_palette(false);
        break;
    case QValidator::Acceptable:
        update_pattern_palette(true);
        break;
    }
}

void rule_editor_t::update_pattern_palette(bool is_pattern_valid)
{
    QPalette palette = _pattern_edit->parentWidget()->palette();

    // Update the base color if the current pattern is invalid
    if (!is_pattern_valid)
    {
        QColor invalid_color{0xff, 0x80, 0x80};
        palette.setColor(QPalette::Active, QPalette::Base, invalid_color);
        palette.setColor(QPalette::Inactive, QPalette::Base, invalid_color);
    }

    _pattern_edit->setPalette(palette);
}

void rule_editor_t::show_pcre_cheatsheet_dialog()
{
    // The dialog is created only once and shown/hidden as required.
    // This avoids having to re-parse the HTML cheatsheet each time and also ensure only one of
    // those dialog can be opened at the same time.
    if (!_pcre_cheatsheet_dialog)
    {
        auto content = new QTextEdit{get_pcre_cheatsheet_content()};
        content->setReadOnly(true);
        auto close_button = new QPushButton{tr("Close")};

        auto layout = new QVBoxLayout;
        layout->addWidget(content);
        layout->addWidget(close_button, 0, Qt::AlignRight);

        _pcre_cheatsheet_dialog = new QDialog{this};
        _pcre_cheatsheet_dialog->setWindowTitle(tr("PCRE Regular Expression Cheatsheet"));
        _pcre_cheatsheet_dialog->setModal(false);
        _pcre_cheatsheet_dialog->setLayout(layout);
        _pcre_cheatsheet_dialog->resize(1200, 600);

        connect(
            _pcre_cheatsheet_dialog, &QDialog::finished, _pcre_cheatsheet_dialog, &QDialog::hide);
        connect(close_button, &QPushButton::clicked, _pcre_cheatsheet_dialog, &QDialog::accept);
    }

    _pcre_cheatsheet_dialog->show();
}
} // namespace flan
