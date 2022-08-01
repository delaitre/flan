
#pragma once

#include <flan/styled_matching_rule.hpp>
#include <QPlainTextEdit>

namespace flan
{
class rule_highlighter_t;
class log_margin_area_widget_t;

class log_widget_t : public QPlainTextEdit
{
    Q_OBJECT

    // The margin widget needs access to protected functions to work properly.
    // Both the log widget and the margin widget are designed to work together and having a friend
    // avoids having to clutter the log widget API with seemingly random functions just to be able
    // to implement the margin widget.
    friend class log_margin_area_widget_t;

public:
    log_widget_t(const QString& text, QWidget* parent = nullptr);

    log_widget_t(QWidget* parent = nullptr)
        : log_widget_t{QString{}, parent}
    {
    }

    //! Return the selected content as plain text with the lines matching removing rules being
    //! excluded.
    QString plain_text_with_rules_applied() const;

public slots:

    void set_rules(flan::styled_matching_rule_list_t rules);

protected:
    void mouseMoveEvent(QMouseEvent* event) override;
    QMimeData* createMimeDataFromSelection() const override;

private:
    QString tooltip_at(QPoint position);

private slots:

    void apply_rules();

private:
    rule_highlighter_t* _highlighter = nullptr;
    styled_matching_rule_list_t _rules;
};
} // namespace flan
