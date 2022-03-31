
#pragma once

#include <QColor>
#include <QToolButton>

namespace flan
{
class color_button_t : public QToolButton
{
    Q_OBJECT

public:
    color_button_t(QWidget* parent = nullptr);

    QColor color() const { return _color; }
    void set_color(QColor color);

signals:

    void color_changed(QColor color);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    QColor _color;
};
} // namespace flan
