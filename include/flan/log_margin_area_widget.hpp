
#pragma once

#include <flan/timestamp_format.hpp>
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
    QAction* use_timestamp_action() { return _use_timestamp_action; }
    QAction* timestamp_format_settings_action() { return _timestamp_format_settings_action; }

    const timestamp_format_list_t& timestamp_formats() const { return _timestamp_formats; }
    void set_timestamp_formats(timestamp_format_list_t formats);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    int ideal_width() const;
    QString text_for_block(const QTextBlock& block);
    bool use_relative_value() const;
    bool use_timestamp() const;
    QTime get_block_timestamp(const QTextBlock& block) const;

private slots:
    void update_width();
    void update_area(const QRect& rect, int dy);
    void show_timestamp_format_settings_dialog();

private:
    log_widget_t* _log_widget = nullptr;
    QAction* _use_relative_value_action = nullptr;
    QAction* _use_timestamp_action = nullptr;
    QAction* _timestamp_format_settings_action = nullptr;

    timestamp_format_list_t _timestamp_formats;
};
} // namespace flan
