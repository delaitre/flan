
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
{
}

void rule_highlighter_t::set_rules(styled_matching_rule_list_t rules)
{
    // Only copy the rules with highlighting turns on has the other ones have no impact for the
    // highlighter.
    _rules.clear();
    std::copy_if(
        rules.begin(),
        rules.end(),
        std::back_inserter(_rules),
        [](const styled_matching_rule_t& rule) { return rule.rule.highlight_match; });

    rehighlight();
}

void rule_highlighter_t::highlightBlock(const QString& text)
{
    // Iterator over the rules in reverse order to keep the first rule as highest priority.
    for (auto rule_it = std::rbegin(_rules); rule_it != std::rend(_rules); ++rule_it)
    {
        const auto& styled_rule = *rule_it;

        if (!styled_rule.rule.rule.isValid())
            continue;

        QRegularExpressionMatchIterator match_it = styled_rule.rule.rule.globalMatch(text);
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
                    to_qt(styled_rule.styles[style_index % styled_rule.styles.size()]));
            }
        }
    }
}
} // namespace flan
