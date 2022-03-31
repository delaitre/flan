
#pragma once

#include <flan/style.hpp>
#include <QWidget>
#include <vector>

class QToolButton;
class QLabel;
class QHBoxLayout;

namespace flan
{
class style_button_t;

class style_list_widget_t : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(matching_style_list_t styles READ styles WRITE set_styles NOTIFY styles_changed)

public:
    style_list_widget_t(QWidget* parent = nullptr);

    inline matching_style_list_t styles() { return _styles; }
    void set_styles(matching_style_list_t styles);

signals:
    void styles_changed(matching_style_list_t styles);

private:
    std::optional<matching_style_t> show_style_editor(const matching_style_t& initial_style);
    style_button_t* create_style_button();
    void update_buttons_from_styles();

private:
    matching_style_list_t _styles;
    std::vector<style_button_t*> _style_buttons;
    QToolButton* _add_button = nullptr;
    QToolButton* _remove_button = nullptr;
    QLabel* _no_style_label = nullptr;
    QHBoxLayout* _buttons_layout = nullptr;
};
} // namespace flan
