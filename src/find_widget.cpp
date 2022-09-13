
#include <flan/find_widget.hpp>
#include <flan/valid_regular_expression_validator.hpp>
#include <flan/validated_lineedit.hpp>
#include <QHBoxLayout>
#include <QLabel>
#include <QRegularExpression>

namespace flan
{
find_widget_t::find_widget_t(find_controller_t* controller, QWidget* parent)
    : QWidget{parent}
    , _controller{controller}
    , _pattern_lineedit{new validated_lineedit_t}
    , _case_sensitivity_checkbox{new QCheckBox{_controller->is_case_sensitive_action()->text()}}
    , _regexp_checkbox{new QCheckBox{_controller->use_regexp_action()->text()}}
    , _previous_button{new QPushButton{_controller->previous_action()->text()}}
    , _next_button{new QPushButton{_controller->next_action()->text()}}
{
    _pattern_lineedit->setMinimumWidth(200);
    _pattern_lineedit->setValidator(new valid_regular_expression_validator_t{_pattern_lineedit});

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

    setFocusProxy(_pattern_lineedit);

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
        _controller->use_regexp_action(),
        &QAction::toggled,
        _pattern_lineedit,
        &validated_lineedit_t::set_validity_check_enabled);
    _pattern_lineedit->set_validity_check_enabled(_controller->use_regexp_action()->isChecked());
    connect(
        _previous_button, &QPushButton::clicked, _controller->previous_action(), &QAction::trigger);
    connect(_next_button, &QPushButton::clicked, _controller->next_action(), &QAction::trigger);

    connect(_controller, &find_controller_t::pattern_changed, this, [this](QString pattern) {
        _pattern_lineedit->setText(pattern);
    });
}
} // namespace flan
