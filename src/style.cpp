
#include <flan/style.hpp>

namespace flan
{
matching_style_list_t get_default_styles()
{
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
}
} // namespace flan
