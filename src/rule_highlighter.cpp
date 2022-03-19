
#include <flan/rule_highlighter.hpp>

namespace flan
{
namespace
{
QTextCharFormat::UnderlineStyle to_qt(underline_style_t underline_style)
{
    switch (underline_style)
    {
    case flan::underline_style_t::none:
        return QTextCharFormat::NoUnderline;
    case flan::underline_style_t::solid:
        return QTextCharFormat::SingleUnderline;
    case flan::underline_style_t::dash:
        return QTextCharFormat::DashUnderline;
    case flan::underline_style_t::dot:
        return QTextCharFormat::DotLine;
    case flan::underline_style_t::wave:
        return QTextCharFormat::WaveUnderline;
    }

    return QTextCharFormat::NoUnderline;
}

QTextCharFormat to_qt(const matching_style_t& style)
{
    QTextCharFormat format;
    if (style.foreground_color.isValid())
        format.setForeground(style.foreground_color);
    if (style.background_color.isValid())
        format.setBackground(style.background_color);
    if (style.underline_color.isValid())
        format.setUnderlineColor(style.underline_color);
    format.setUnderlineStyle(to_qt(style.underline_style));
    return format;
}
} // namespace

rule_highlighter_t::rule_highlighter_t(QTextDocument* parent)
    : QSyntaxHighlighter{parent}
    , _styles{get_default_styles()}
{
}

void rule_highlighter_t::set_rules(QVector<matching_rule_t> rules)
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

    for (const auto& rule: _rules)
    {
        if (!rule.highlight_match)
            continue;

        QRegularExpressionMatchIterator it = rule.rule.globalMatch(text);
        while (it.hasNext())
        {
            QRegularExpressionMatch match = it.next();

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
