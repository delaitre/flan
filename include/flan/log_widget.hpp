
#pragma once

#include <flan/matching_rule.hpp>
#include <flan/style.hpp>
#include <QPlainTextEdit>

namespace flan
{
class rule_highlighter_t;

class log_widget_t : public QPlainTextEdit
{
    Q_OBJECT

public:
    log_widget_t(const QString& text, QWidget* parent = nullptr);

    log_widget_t(QWidget* parent = nullptr)
        : log_widget_t{QString{}, parent}
    {
    }

public slots:

    void set_rules(matching_rule_list_t rules);

    void set_highlighting_style(matching_style_list_t styles);

protected:
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    QString tooltip_at(QPoint position);

private slots:

    void apply_rules();

private:
    rule_highlighter_t* _highlighter = nullptr;
    matching_rule_list_t _rules;
};
} // namespace flan
