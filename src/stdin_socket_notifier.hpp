
#pragma once

#include <QObject>
#include <QString>
#include <QTextStream>
#include <QThread>

#ifdef Q_OS_WIN
#include <QWinEventNotifier>
#else
#include <QSocketNotifier>
#endif

namespace flan
{
class stdin_socket_notifier_t : public QObject
{
    Q_OBJECT

public:
    explicit stdin_socket_notifier_t(QObject* parent = nullptr);
    virtual ~stdin_socket_notifier_t();

signals:
    //! Signal emitted when a new \a line has been received.
    void new_line(QString line);

    //! Signal used internally only to transfer the received \a line from the receiving thread to
    //! the notifier's thread.
    //!
    //! \warning Users of the stdin_socket_notifier_t should use the new_line() signal instead.
    void new_line_private(QString line);

private:
    QThread _thread;
    QTextStream _stream;

#ifdef Q_OS_WIN
    QWinEventNotifier* _notifier;
#else
    QSocketNotifier* _notifier;
#endif
};
} // namespace flan
