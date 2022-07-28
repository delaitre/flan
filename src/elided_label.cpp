
#include <flan/elided_label.hpp>
#include <QFontMetrics>
#include <QStylePainter>

namespace flan
{
elided_label_t::elided_label_t(const QString& text, QWidget* parent)
    : QFrame{parent}
    , _text{text}
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

void elided_label_t::set_text(const QString& text)
{
    if (_text != text)
    {
        _text = text;
        emit text_changed(_text);
        updateGeometry();
        update();
    }
}

QSize elided_label_t::sizeHint() const
{
    return QFontMetrics{font()}.boundingRect(_text).size();
}

void elided_label_t::paintEvent(QPaintEvent* event)
{
    QFrame::paintEvent(event);

    QStylePainter painter{this};
    QFontMetrics font_metrics = painter.fontMetrics();

    painter.drawText(
        0, font_metrics.ascent(), font_metrics.elidedText(_text, Qt::ElideMiddle, width()));
}
} // namespace flan
