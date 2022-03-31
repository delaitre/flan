
#include <flan/color_button.hpp>
#include <QAction>
#include <QColorDialog>
#include <QMouseEvent>
#include <QPainter>

namespace flan
{
color_button_t::color_button_t(QWidget* parent)
    : QToolButton{parent}
{
    setAutoRaise(true);
    setAutoFillBackground(true);

    auto show_color_picker_action = new QAction{this};
    connect(show_color_picker_action, &QAction::triggered, this, [this]() {
        if (auto new_color = QColorDialog::getColor(color(), this, tr("Select color"));
            new_color.isValid())
        {
            set_color(new_color);
        }
    });

    // setToolTip needs to be called after setDefaultAction.
    setDefaultAction(show_color_picker_action);
    setToolTip(tr("Right click to clear"));
}

void color_button_t::set_color(QColor color)
{
    if (_color != color)
    {
        _color = color;

        setStyleSheet(
            QString("QToolButton { %1 }")
                .arg(
                    _color.isValid() ?
                        QString("background-color: %1;").arg(_color.name(QColor::HexArgb)) :
                        ""));

        emit color_changed(_color);
    }
}

void color_button_t::paintEvent(QPaintEvent* event)
{
    QToolButton::paintEvent(event);

    // If the color is invalid, draw a cross over the button
    if (!_color.isValid())
    {
        QPainter painter{this};
        painter.setPen(QPen{Qt::red, 2});
        painter.setRenderHint(QPainter::Antialiasing, true);
        constexpr int cross_margin = 4;
        auto bounds = rect();
        bounds.adjust(cross_margin, cross_margin, -cross_margin, -cross_margin);
        painter.drawLine(bounds.topLeft(), bounds.bottomRight());
        painter.drawLine(bounds.topRight(), bounds.bottomLeft());
    }
}

void color_button_t::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        set_color(QColor{});
        event->accept();
    }
    else
    {
        QToolButton::mouseReleaseEvent(event);
    }
}
} // namespace flan
