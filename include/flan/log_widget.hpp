
#pragma once

#include <flan/matching_rule.hpp>
#include <QWidget>

class QPlainTextEdit;

namespace flan
{
class rule_highlighter_t;

class log_widget_t : public QWidget
{
    Q_OBJECT

public:
    log_widget_t(QWidget* parent = nullptr);

public slots:

    void set_content(const QString& text);
    void append_content(const QString& text);

    void set_rules(QVector<matching_rule_t> rules);

private slots:

    void apply_rules();

private:
    QPlainTextEdit* _text_edit = nullptr;
    rule_highlighter_t* _highlighter = nullptr;
    QVector<matching_rule_t> _rules;
};
} // namespace flan
