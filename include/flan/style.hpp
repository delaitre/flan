
#pragma once

#include <QColor>
#include <vector>

namespace flan
{
enum class underline_style_t
{
    none,
    solid,
    dash,
    dot,
    wave,
};

struct matching_style_t
{
    QColor foreground_color;
    QColor background_color;
    QColor underline_color;
    underline_style_t underline_style;
};

using matching_style_list_t = std::vector<matching_style_t>;

matching_style_list_t get_default_styles();
} // namespace flan
