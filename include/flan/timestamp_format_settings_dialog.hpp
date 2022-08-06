
#pragma once

#include <flan/timestamp_format.hpp>
#include <QAction>
#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QSpinBox>

namespace flan
{
class validated_lineedit_t;

class timestamp_format_settings_dialog_t : public QDialog
{
    Q_OBJECT

public:
    timestamp_format_settings_dialog_t(QString test_string, QWidget* parent = nullptr);

    timestamp_format_list_t formats() const;
    void set_formats(const timestamp_format_list_t& formats);

private:
    std::optional<timestamp_format_t> format(int i) const;
    std::optional<timestamp_format_t> current_format() const;

private slots:
    void add_format();
    void add_format(flan::timestamp_format_t format);
    void remove_format();
    void update_widgets_from_current_format();
    void update_current_format_from_widgets();

private:
    struct component_capture_widgets_t : public QHBoxLayout
    {
        QSpinBox* captured_index_spinbox;
        QLabel* preview_label;

        component_capture_widgets_t(QString component_name, QWidget* parent = nullptr);

        void update(bool is_enabled, int capture_count, int captured_index, QString match);
    };

private:
    QListWidget* _format_list;
    QToolButton* _add_format_button;
    QAction* _add_format_action;
    QToolButton* _remove_format_button;
    QAction* _remove_format_action;

    QLineEdit* _name_lineedit;
    validated_lineedit_t* _pattern_lineedit;
    component_capture_widgets_t* _hour_widgets;
    component_capture_widgets_t* _minute_widgets;
    component_capture_widgets_t* _second_widgets;
    component_capture_widgets_t* _millisecond_widgets;

    QLineEdit* _test_string_lineedit;
    QLabel* _preview_label;
};
} // namespace flan
