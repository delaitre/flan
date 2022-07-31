
#include <flan/find_widget.hpp>
#include <QHBoxLayout>
#include <QLabel>
#include <QRegularExpression>

namespace flan
{
find_widget_t::find_widget_t(find_controller_t* controller, QWidget* parent)
    : QWidget{parent}
    , _controller{controller}
    , _pattern_lineedit{new QLineEdit}
    , _case_sensitivity_checkbox{new QCheckBox{_controller->is_case_sensitive_action()->text()}}
    , _regexp_checkbox{new QCheckBox{_controller->use_regexp_action()->text()}}
    , _previous_button{new QPushButton{_controller->previous_action()->text()}}
    , _next_button{new QPushButton{_controller->next_action()->text()}}
{
    _pattern_lineedit->setMinimumWidth(200);

    auto layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(new QLabel(tr("Find")));
    layout->addWidget(_pattern_lineedit);
    layout->addWidget(_case_sensitivity_checkbox);
    layout->addWidget(_regexp_checkbox);
    layout->addStretch();
    layout->addWidget(_previous_button);
    layout->addWidget(_next_button);
    setLayout(layout);

    connect(
        _pattern_lineedit, &QLineEdit::textEdited, _controller, &find_controller_t::set_pattern);
    connect(
        _case_sensitivity_checkbox,
        &QCheckBox::clicked,
        _controller->is_case_sensitive_action(),
        &QAction::setChecked);
    connect(
        _regexp_checkbox,
        &QCheckBox::clicked,
        _controller->use_regexp_action(),
        &QAction::setChecked);
    connect(
        _previous_button, &QPushButton::clicked, _controller->previous_action(), &QAction::trigger);
    connect(_next_button, &QPushButton::clicked, _controller->next_action(), &QAction::trigger);

    connect(_controller, &find_controller_t::pattern_changed, this, [this](QString pattern) {
        _pattern_lineedit->setText(pattern);
        update_regexp_validity(_controller->is_pattern_valid());
    });
}

void find_widget_t::update_regexp_validity(bool is_valid)
{
    // Update the base color if the current pattern is invalid
    QPalette palette = _pattern_lineedit->parentWidget()->palette();
    if (!is_valid)
    {
        QColor invalid_color{0xff, 0x80, 0x80};
        palette.setColor(QPalette::Active, QPalette::Base, invalid_color);
        palette.setColor(QPalette::Inactive, QPalette::Base, invalid_color);
    }

    _pattern_lineedit->setPalette(palette);
}
} // namespace flan
