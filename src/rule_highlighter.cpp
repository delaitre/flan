
#include <flan/rule_highlighter.hpp>

namespace flan
{
namespace
{
constexpr QFont::Weight to_qt(font_weight_t font_weight)
{
    switch (font_weight)
    {
    case font_weight_t::normal:
        return QFont::Normal;
    case font_weight_t::bold:
        return QFont::Bold;
    }

    return QFont::Normal;
}

QTextCharFormat to_qt(const matching_style_t& style)
{
    QTextCharFormat format;
    if (style.foreground_color.isValid())
        format.setForeground(style.foreground_color);
    if (style.background_color.isValid())
        format.setBackground(style.background_color);

    switch (style.font_style)
    {
    case font_style_t::normal:
        format.setFontItalic(false);
        break;
    case font_style_t::italic:
        format.setFontItalic(true);
        break;
    }

    format.setFontWeight(to_qt(style.font_weight));

    return format;
}
} // namespace

rule_highlighter_t::rule_highlighter_t(QTextDocument* parent)
    : QSyntaxHighlighter{parent}
    , _styles{get_default_styles()}
{
}

void rule_highlighter_t::set_rules(matching_rule_list_t rules)
{
    _rules = std::move(rules);
    rehighlight();
}

void rule_highlighter_t::set_style_list(matching_style_list_t styles)
{
    _styles = std::move(styles);
    rehighlight();
}

void rule_highlighter_t::highlightBlock(const QString& text)
{
    if (_styles.empty())
        return;

    // Iterator over the rules in reverse order to keep the first rule as highest priority.
    for (auto rule_it = std::rbegin(_rules); rule_it != std::rend(_rules); ++rule_it)
    {
        const auto& rule = *rule_it;

        if (!rule.highlight_match)
            continue;

        QRegularExpressionMatchIterator match_it = rule.rule.globalMatch(text);
        while (match_it.hasNext())
        {
            QRegularExpressionMatch match = match_it.next();

            // Highlight each capture individually, or if no specific capture group was specified
            // highlight the whole match.
            int start_index = (match.lastCapturedIndex() == 0) ? 0 : 1;
            for (int i = start_index, style_index = 0; i <= match.lastCapturedIndex();
                 ++i, ++style_index)
            {
                setFormat(
                    match.capturedStart(i),
                    match.capturedLength(i),
                    to_qt(_styles[style_index % _styles.size()]));
            }
        }
    }
}
} // namespace flan
