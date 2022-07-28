
#pragma once

#include <QObject>
#include <QString>

namespace flan
{
class data_source_t : public QObject
{
    Q_OBJECT

public:
    explicit data_source_t(QObject* parent = nullptr);

    virtual QString text() const = 0;
    virtual QString info() const = 0;
    virtual QString error_message() const = 0;

signals:
    void new_text(QString text);
    void info_changed(QString info);

    //! Emitted every time an error occured with the given \a error_message.
    //!
    //! If the \a error_message is null (QString::isNull()) the error has been cleared.
    void error_changed(QString error_message);
};
} // namespace flan
