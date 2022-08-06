
#pragma once

#include <flan/find_controller.hpp>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

namespace flan
{
class validated_lineedit_t;

class find_widget_t : public QWidget
{
    Q_OBJECT

public:
    find_widget_t(find_controller_t* controller, QWidget* parent = nullptr);

private:
    find_controller_t* _controller;
    validated_lineedit_t* _pattern_lineedit;
    QCheckBox* _case_sensitivity_checkbox;
    QCheckBox* _regexp_checkbox;
    QPushButton* _previous_button;
    QPushButton* _next_button;
};
} // namespace flan
