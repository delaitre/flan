
#pragma once

#include <flan/style.hpp>
#include <QToolButton>

namespace flan
{
class style_button_t : public QToolButton
{
    Q_OBJECT

public:
    style_button_t(QWidget* parent = nullptr);

    matching_style_t style() const { return _style; }
    void set_style(matching_style_t style);

signals:

    void style_changed(matching_style_t style);

private:
    matching_style_t _style;
};
} // namespace flan
