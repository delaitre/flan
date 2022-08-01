
#include "stdin_socket_notifier.hpp"
#include <iostream>

namespace flan
{
stdin_socket_notifier_t::stdin_socket_notifier_t(QObject* parent)
    : QObject{parent}
    , _stream{stdin, QIODeviceBase::ReadOnly}
#ifdef Q_OS_WIN
    , _notifier{new QWinEventNotifier{GetStdHandle(STD_INPUT_HANDLE)}}
#else
    , _notifier{new QSocketNotifier{fileno(stdin), QSocketNotifier::Read}}
#endif
{
    // reading lines is blocking so move the notifier in its own thread and use a QueuedConnection
    // to get new content back in the main thread in a thread safe manner.
    _notifier->moveToThread(&_thread);
    connect(&_thread, &QThread::finished, _notifier, &QObject::deleteLater);
    connect(
        this,
        &stdin_socket_notifier_t::new_line_private,
        this,
        &stdin_socket_notifier_t::new_line,
        Qt::QueuedConnection);

    connect(
        _notifier,
#ifdef Q_OS_WIN
        &QWinEventNotifier::activated,
#else
        &QSocketNotifier::activated,
#endif
        _notifier,
        [this]() {
            QString line;
            while (_stream.readLineInto(&line))
            {
                // readLineInto removes the new line, so add it back...
                line.append('\n');
                emit new_line_private(line);
            }
        });

    _thread.start();
}

stdin_socket_notifier_t::~stdin_socket_notifier_t()
{
    _thread.quit();
    _thread.wait();
}
} // namespace flan
