
#include <flan/style_button.hpp>

namespace flan
{
style_button_t::style_button_t(QWidget* parent)
    : QToolButton{parent}
{
    setAutoRaise(true);
    setAutoFillBackground(true);
    setFixedSize(20, 20);
}

void style_button_t::set_style(matching_style_t style)
{
    if (_style != style)
    {
        _style = style;

        setStyleSheet(QString("QToolButton { %1 }").arg(to_css_string(_style)));

        emit style_changed(_style);
    }
}
} // namespace flan
