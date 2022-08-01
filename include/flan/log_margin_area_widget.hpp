
#pragma once

#include <QAction>
#include <QTextBlock>
#include <QWidget>

namespace flan
{
class log_widget_t;

class log_margin_area_widget_t : public QWidget
{
    Q_OBJECT

public:
    log_margin_area_widget_t(log_widget_t* log_widget);

    QSize sizeHint() const override { return QSize(ideal_width(), 0); }

    QAction* use_relative_value_action() { return _use_relative_value_action; }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    int ideal_width() const;
    QString text_for_block(const QTextBlock& block);
    bool use_relative_value() const;

private slots:
    void update_width();
    void update_area(const QRect& rect, int dy);

private:
    log_widget_t* _log_widget = nullptr;
    QAction* _use_relative_value_action = nullptr;
};
} // namespace flan
