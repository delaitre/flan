
#pragma once

#include <flan/styled_matching_rule.hpp>
#include <QSyntaxHighlighter>
#include <QVector>

namespace flan
{
class rule_highlighter_t : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    rule_highlighter_t(QTextDocument* parent);

    void set_rules(styled_matching_rule_list_t rules);

protected:
    void highlightBlock(const QString& text) final;

private:
    styled_matching_rule_list_t _rules;
};
} // namespace flan
