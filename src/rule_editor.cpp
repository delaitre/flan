
#include <flan/pcre_cheatsheet_dialog.hpp>
#include <flan/rule_editor.hpp>
#include <flan/rule_model.hpp>
#include <flan/rule_set.hpp>
#include <flan/style_list_widget.hpp>
#include <flan/valid_regular_expression_validator.hpp>
#include <flan/validated_lineedit.hpp>
#include <QAction>
#include <QFormLayout>
#include <QLineEdit>
#include <QStyle>

namespace
{
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
    , _pattern_edit{new validated_lineedit_t}
    , _tooltip_edit{new QLineEdit}
    , _style_list{new style_list_widget_t}
{
    QFormLayout* layout = new QFormLayout;
    layout->setContentsMargins(5, 0, 0, 0);
    layout->addRow(tr("Name"), _name_edit);
    layout->addRow(tr("Pattern"), _pattern_edit);
    layout->addRow(tr("Tooltip"), _tooltip_edit);
    layout->addRow(tr("Styles"), _style_list);

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
    connect(
        _style_list, &style_list_widget_t::styles_changed, this, [this]() { _mapper.submit(); });
}

void rule_editor_t::set_model(QAbstractItemModel* model)
{
    _mapper.setModel(model);

    _mapper.addMapping(_name_edit, rule_model_t::name_column_index);
    _mapper.addMapping(_pattern_edit, rule_model_t::pattern_column_index);
    _mapper.addMapping(_tooltip_edit, rule_model_t::tooltip_column_index);
    _mapper.addMapping(_style_list, rule_model_t::styles_column_index, "styles");
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
}

void rule_editor_t::show_pcre_cheatsheet_dialog()
{
    // The dialog is created only once and shown/hidden as required.
    // This avoids having to re-parse the HTML cheatsheet each time and also ensure only one of
    // those dialog can be opened at the same time.
    if (!_pcre_cheatsheet_dialog)
    {
        _pcre_cheatsheet_dialog = new pcre_cheatsheet_dialog_t{this};
        _pcre_cheatsheet_dialog->setModal(false);

        connect(
            _pcre_cheatsheet_dialog, &QDialog::finished, _pcre_cheatsheet_dialog, &QDialog::hide);
    }

    _pcre_cheatsheet_dialog->show();
}
} // namespace flan
