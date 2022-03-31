
#pragma once

#include <QColor>
#include <QMetaType>
#include <vector>

namespace flan
{
enum class font_style_t
{
    normal,
    italic,
};

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

enum class font_weight_t
{
    normal,
    bold,
};

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

struct matching_style_t
{
    QColor foreground_color;
    QColor background_color;
    font_style_t font_style{font_style_t::normal};
    font_weight_t font_weight{font_weight_t::normal};
};

inline bool operator==(const matching_style_t& lhs, const matching_style_t& rhs)
{
    return (lhs.foreground_color == rhs.foreground_color)
        && (lhs.background_color == rhs.background_color) && (lhs.font_style == rhs.font_style)
        && (lhs.font_weight == rhs.font_weight);
}

inline bool operator!=(const matching_style_t& lhs, const matching_style_t& rhs)
{
    return !(lhs == rhs);
}

using matching_style_list_t = std::vector<matching_style_t>;

const matching_style_list_t& get_default_styles();
} // namespace flan

Q_DECLARE_METATYPE(flan::font_style_t)
Q_DECLARE_METATYPE(flan::font_weight_t)
Q_DECLARE_METATYPE(flan::matching_style_t)
