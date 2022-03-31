
#pragma once

#include <QColor>
#include <QMetaType>
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

inline bool operator==(const matching_style_t& lhs, const matching_style_t& rhs)
{
    return (lhs.foreground_color == rhs.foreground_color)
        && (lhs.background_color == rhs.background_color)
        && (lhs.underline_color == rhs.underline_color)
        && (lhs.underline_style == rhs.underline_style);
}

inline bool operator!=(const matching_style_t& lhs, const matching_style_t& rhs)
{
    return !(lhs == rhs);
}

using matching_style_list_t = std::vector<matching_style_t>;

const matching_style_list_t& get_default_styles();
} // namespace flan

Q_DECLARE_METATYPE(flan::underline_style_t)
Q_DECLARE_METATYPE(flan::matching_style_t)
