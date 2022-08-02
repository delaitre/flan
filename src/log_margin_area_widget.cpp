
#include <flan/log_margin_area_widget.hpp>
#include <flan/log_widget.hpp>
#include <QPainter>
#include <QTime>

namespace flan
{
namespace
{
static constexpr int _number_area_margin = 4;
}

log_margin_area_widget_t::log_margin_area_widget_t(log_widget_t* log_widget)
    : QWidget{log_widget}
    , _log_widget{log_widget}
    , _use_relative_value_action{new QAction{tr("Use relative value"), this}}
    , _use_timestamp_action{new QAction{tr("Use timestamp"), this}}
{
    connect(
        _log_widget,
        &log_widget_t::blockCountChanged,
        this,
        &log_margin_area_widget_t::update_width);
    connect(
        _log_widget, &log_widget_t::updateRequest, this, &log_margin_area_widget_t::update_area);
    connect(_log_widget, &log_widget_t::cursorPositionChanged, this, [this]() {
        if (use_relative_value())
            update();
    });

    update_width();

    _use_relative_value_action->setCheckable(true);
    connect(
        _use_relative_value_action,
        &QAction::toggled,
        this,
        &log_margin_area_widget_t::update_width);
    addAction(_use_relative_value_action);

    _use_timestamp_action->setCheckable(true);
    connect(
        _use_timestamp_action, &QAction::toggled, this, &log_margin_area_widget_t::update_width);
    addAction(_use_timestamp_action);

    setContextMenuPolicy(Qt::ActionsContextMenu);
}

int log_margin_area_widget_t::ideal_width() const
{
    // Init with the margins;
    int width = _number_area_margin * 2;

    if (use_timestamp())
    {
        width +=
            fontMetrics().horizontalAdvance(QTime{23, 59, 59, 999}.toString(Qt::ISODateWithMs));
    }
    else
    {
        int digits = 1;
        int max = qMax(1, _log_widget->blockCount());

        if (use_relative_value())
        {
            // because it is relative, things start at 0 (e.g. if blockCount == 10 the max number
            // in the margin will be 9, not 10)
            --max;
        }

        while (max >= 10)
        {
            max /= 10;
            ++digits;
        }

        width += fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    }

    if (use_relative_value())
        width += fontMetrics().horizontalAdvance(QLatin1Char('-'));

    return width;
}

namespace
{
QTime get_block_timestamp(const QTextBlock& block)
{
    QRegularExpression time_regexp{"timestamp: ([0-9]+)"};
    if (auto match = time_regexp.match(block.text()); match.hasMatch())
        return QTime::fromMSecsSinceStartOfDay(match.captured(1).toInt());

    return {};
}
} // namespace

QString log_margin_area_widget_t::text_for_block(const QTextBlock& block)
{
    if (use_timestamp())
    {
        if (auto timestamp = get_block_timestamp(block); timestamp.isValid())
        {
            if (use_relative_value())
            {
                auto cursor_timestamp = get_block_timestamp(_log_widget->textCursor().block());
                if (cursor_timestamp.isValid())
                {
                    auto relative_timestamp = QTime::fromMSecsSinceStartOfDay(qAbs(
                        cursor_timestamp.msecsSinceStartOfDay()
                        - timestamp.msecsSinceStartOfDay()));

                    QString text;
                    if (cursor_timestamp > timestamp)
                        text.append('-');

                    text.append(relative_timestamp.toString(Qt::ISODateWithMs));
                    return text;
                }
                else
                {
                    return {};
                }
            }
            else
            {
                return timestamp.toString(Qt::ISODateWithMs);
            }
        }

        return {};
    }
    else if (use_relative_value())
        return QString::number(block.blockNumber() - _log_widget->textCursor().blockNumber());
    else
        return QString::number(block.blockNumber() + 1);
}

bool log_margin_area_widget_t::use_relative_value() const
{
    return _use_relative_value_action->isChecked();
}

bool log_margin_area_widget_t::use_timestamp() const
{
    return _use_timestamp_action->isChecked();
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
