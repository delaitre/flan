
#include <flan/rule_highlighter.hpp>
#include <array>

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
    static std::array highlight_colors{
        QColor{Qt::red},
        QColor{Qt::green},
        QColor{Qt::blue},
        QColor{Qt::cyan},
        QColor{Qt::magenta},
        QColor{Qt::yellow},
        QColor{Qt::gray}};

    QTextCharFormat class_format;

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
            for (int i = start_index, color_index = 0; i <= match.lastCapturedIndex();
                 ++i, ++color_index)
            {
                class_format.setForeground(highlight_colors[color_index % highlight_colors.size()]);
                setFormat(match.capturedStart(i), match.capturedLength(i), class_format);
            }
        }
    }
}
} // namespace flan
