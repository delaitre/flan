
#include <flan/style_button.hpp>
#include <flan/style_editor_dialog.hpp>
#include <flan/style_list_widget.hpp>
#include <QAction>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QToolButton>

// Q_INIT_RESOURCE has to be called from the global namespace, hence this workaround.
inline void init_resource()
{
    Q_INIT_RESOURCE(libflan);
}

namespace flan
{
style_list_widget_t::style_list_widget_t(QWidget* parent)
    : QWidget{parent}
    , _no_style_label{new QLabel{tr("<em>inherited</em>")}}
    , _buttons_layout{new QHBoxLayout}
{
    init_resource();

    _add_button = new QToolButton{this};
    _add_button->setAutoRaise(true);
    auto add_action = new QAction{QIcon{":/icons/light/plus"}, tr("Add style"), _add_button};
    _add_button->setDefaultAction(add_action);
    connect(add_action, &QAction::triggered, this, [this]() {
        if (auto style = show_style_editor({}))
        {
            _styles.push_back(*style);
            update_buttons_from_styles();
            emit styles_changed(_styles);
        }
    });

    _remove_button = new QToolButton{this};
    _remove_button->setAutoRaise(true);
    auto remove_action = new QAction{QIcon{":/icons/light/minus"}, tr("Remove style"), _remove_button};
    _remove_button->setDefaultAction(remove_action);
    connect(remove_action, &QAction::triggered, this, [this]() {
        if (!_styles.empty())
        {
            _styles.pop_back();
            update_buttons_from_styles();
            emit styles_changed(_styles);
        }
    });

    _buttons_layout->setContentsMargins(0, 0, 0, 0);
    _buttons_layout->addWidget(_no_style_label);
    _buttons_layout->addStretch(); // Always have a stretch between the colors and the +/- buttons
    _buttons_layout->addWidget(_add_button);
    _buttons_layout->addWidget(_remove_button);

    setLayout(_buttons_layout);
    update_buttons_from_styles();
}

void style_list_widget_t::set_styles(matching_style_list_t styles)
{
    if (_styles != styles)
    {
        _styles = std::move(styles);
        update_buttons_from_styles();
        emit styles_changed(_styles);
    }
}

std::optional<matching_style_t> style_list_widget_t::show_style_editor(
    const matching_style_t& initial_style)
{
    return style_editor_dialog_t::get_style(initial_style, this, tr("Rule highlighting style"));
}

style_button_t* style_list_widget_t::create_style_button()
{
    auto button = new style_button_t;
    _style_buttons.push_back(button);
    int button_index = _style_buttons.size() - 1;
    button->set_style(_styles[button_index]);

    connect(button, &style_button_t::style_changed, this, [this, button_index](auto style) {
        _styles[button_index] = style;
        update_buttons_from_styles();
        emit styles_changed(_styles);
    });

    auto show_style_editor_action = new QAction{button};
    connect(show_style_editor_action, &QAction::triggered, this, [this, button, button_index]() {
        if (auto style = show_style_editor(_styles[button_index]))
            button->set_style(*style);
    });

    // setText needs to be called after setDefaultAction.
    button->setDefaultAction(show_style_editor_action);
    button->setText(QString::number(button_index + 1));

    // Insert the new button before the +/- buttons and stretch item which are always at the
    // back.
    _buttons_layout->insertWidget(_buttons_layout->count() - 3, button);

    return button;
}

void style_list_widget_t::update_buttons_from_styles()
{
    // If there is no styles to show, show the appropriate label and buttons.
    _no_style_label->setVisible(_styles.empty());
    _remove_button->setEnabled(!_styles.empty());

    // Add buttons or update existing ones to match the new colors.
    for (int i = 0; i < _styles.size(); ++i)
    {
        if (i < _style_buttons.size())
            _style_buttons[i]->set_style(_styles[i]);
        else
            create_style_button();
    }

    // Remove additional buttons that may be present if the list of styles was bigger before.
    while (_styles.size() < _style_buttons.size())
    {
        auto* button = _style_buttons.back();
        _style_buttons.pop_back();
        _buttons_layout->removeWidget(button);
        delete button;
    }
}
} // namespace flan
