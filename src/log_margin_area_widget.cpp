
#include <flan/log_margin_area_widget.hpp>
#include <flan/log_widget.hpp>
#include <QPainter>

namespace flan
{
namespace
{
static constexpr int _number_area_margin = 4;
}

log_margin_area_widget_t::log_margin_area_widget_t(log_widget_t* log_widget)
    : QWidget{log_widget}
    , _log_widget{log_widget}
{
    connect(
        _log_widget,
        &log_widget_t::blockCountChanged,
        this,
        &log_margin_area_widget_t::update_width);
    connect(
        _log_widget, &log_widget_t::updateRequest, this, &log_margin_area_widget_t::update_area);

    update_width();
}

int log_margin_area_widget_t::ideal_width() const
{
    int digits = 1;
    int max = qMax(1, _log_widget->blockCount());
    while (max >= 10)
    {
        max /= 10;
        ++digits;
    }

    int space =
        3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits + _number_area_margin * 2;

    return space;
}

QString log_margin_area_widget_t::text_for_block(const QTextBlock& block)
{
    return QString::number(block.blockNumber() + 1);
}

void log_margin_area_widget_t::paintEvent(QPaintEvent* event)
{
    QPainter painter{this};
    painter.fillRect(event->rect(), palette().color(backgroundRole()).darker(105));

    QTextBlock block = _log_widget->firstVisibleBlock();
    int top = qRound(
        _log_widget->blockBoundingGeometry(block).translated(_log_widget->contentOffset()).top());
    int bottom = top + qRound(_log_widget->blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom())
    {
        if (block.isVisible() && bottom >= event->rect().top())
        {
            // Use a bold font for the current line
            auto font = painter.font();
            font.setBold(block == _log_widget->textCursor().block());
            painter.setFont(font);

            painter.drawText(
                -_number_area_margin,
                top,
                width(),
                fontMetrics().height(),
                Qt::AlignRight,
                text_for_block(block));
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(_log_widget->blockBoundingRect(block).height());
    }
}

void log_margin_area_widget_t::update_width()
{
    _log_widget->setViewportMargins(ideal_width(), 0, 0, 0);

    QRect r = _log_widget->contentsRect();
    setGeometry(QRect(r.left(), r.top(), ideal_width(), r.height()));
}

void log_margin_area_widget_t::update_area(const QRect& rect, int dy)
{
    if (dy)
        scroll(0, dy);
    else
        update(0, rect.y(), width(), rect.height());

    if (rect.contains(_log_widget->viewport()->rect()))
        update_width();
}
} // namespace flan
