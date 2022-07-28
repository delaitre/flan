
#pragma once

#include <QFrame>

namespace flan
{
class elided_label_t : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE set_text NOTIFY text_changed)

public:
    explicit elided_label_t(const QString& text = {}, QWidget* parent = nullptr);

    const QString& text() const { return _text; }
    void set_text(const QString& text);

    QSize sizeHint() const override;

signals:
    void text_changed(QString text);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QString _text;
};
} // namespace flan
