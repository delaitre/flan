
#include <flan/style.hpp>

namespace flan
{
const matching_style_list_t& get_default_styles()
{
    static matching_style_list_t styles = []() {
        std::array foreground_colors{
            QColor{Qt::red},
            QColor{Qt::green},
            QColor{Qt::blue},
            QColor{Qt::cyan},
            QColor{Qt::magenta},
            QColor{Qt::yellow},
            QColor{Qt::gray}};

        matching_style_list_t styles;
        styles.resize(foreground_colors.size());
        for (int i = 0; i < styles.size(); ++i)
            styles[i].foreground_color = foreground_colors[i];

        return styles;
    }();

    return styles;
}

constexpr const char* to_css_string(font_style_t font_style)
{
    switch (font_style)
    {
    case font_style_t::normal:
        return "normal";
    case font_style_t::italic:
        return "italic";
    }

    return "normal";
}

constexpr const char* to_css_string(font_weight_t font_weight)
{
    switch (font_weight)
    {
    case font_weight_t::normal:
        return "normal";
    case font_weight_t::bold:
        return "bold";
    }

    return "normal";
}

QString to_css_string(const matching_style_t& style)
{
    return QString("%1 %2 %3 %4")
        .arg(
            style.foreground_color.isValid() ?
                QString("color: %1;").arg(style.foreground_color.name(QColor::HexArgb)) :
                "")
        .arg(
            style.background_color.isValid() ?
                QString("background-color: %1;").arg(style.background_color.name(QColor::HexArgb)) :
                "")
        .arg(QString("font-style: %1;").arg(to_css_string(style.font_style)))
        .arg(QString("font-weight: %1;").arg(to_css_string(style.font_weight)));
}
} // namespace flan
