
#pragma once

#include <flan/matching_rule.hpp>
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

protected:
    void highlightBlock(const QString& text) final;

private:
    QVector<matching_rule_t> _rules;
};
} // namespace flan
