
#include <flan/rule_highlighter.hpp>

namespace flan
{
rule_highlighter_t::rule_highlighter_t(QTextDocument* parent)
    : QSyntaxHighlighter{parent}
{
}

void rule_highlighter_t::set_rules(QVector<matching_rule_t> rules)
{
    _rules = std::move(rules);
    rehighlight();
}

void rule_highlighter_t::highlightBlock(const QString& text)
{
    QTextCharFormat class_format;
    class_format.setFontWeight(QFont::Bold);
    class_format.setForeground(Qt::darkMagenta);

    for (const auto& rule: _rules)
    {
        if (!rule.highlight_match)
            continue;

        QRegularExpressionMatchIterator it = rule.rule.globalMatch(text);
        while (it.hasNext())
        {
            QRegularExpressionMatch match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), class_format);
        }
    }
}
} // namespace flan
