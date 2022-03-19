
#pragma once

#include <flan/matching_rule.hpp>
#include <flan/style.hpp>
#include <QSyntaxHighlighter>
#include <QVector>

namespace flan
{
class rule_highlighter_t : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    rule_highlighter_t(QTextDocument* parent);

    void set_rules(QVector<matching_rule_t> rules);

    void set_style_list(matching_style_list_t styles);

protected:
    void highlightBlock(const QString& text) final;

private:
    QVector<matching_rule_t> _rules;
    matching_style_list_t _styles;
};
} // namespace flan
