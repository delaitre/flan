
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

        setStyleSheet(
            QString("QToolButton { %1 %2 %3 %4 }")
                .arg(
                    _style.foreground_color.isValid() ?
                        QString("color: %1;").arg(_style.foreground_color.name(QColor::HexArgb)) :
                        "")
                .arg(
                    _style.background_color.isValid() ?
                        QString("background-color: %1;")
                            .arg(_style.background_color.name(QColor::HexArgb)) :
                        "")
                .arg(QString("font-style: %1;").arg(to_css_string(_style.font_style)))
                .arg(QString("font-weight: %1;").arg(to_css_string(_style.font_weight))));

        emit style_changed(_style);
    }
}
} // namespace flan
