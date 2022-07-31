
#pragma once

#include <flan/find_controller.hpp>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

namespace flan
{
class find_widget_t : public QWidget
{
    Q_OBJECT

public:
    find_widget_t(find_controller_t* controller, QWidget* parent = nullptr);

private slots:
    void update_regexp_validity(bool is_valid);

private:
    find_controller_t* _controller;
    QLineEdit* _pattern_lineedit;
    QCheckBox* _case_sensitivity_checkbox;
    QCheckBox* _regexp_checkbox;
    QPushButton* _previous_button;
    QPushButton* _next_button;
};
} // namespace flan
