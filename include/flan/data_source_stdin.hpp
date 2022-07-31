
#pragma once

#include <flan/data_source.hpp>

namespace flan
{
class stdin_socket_notifier_t;

class data_source_stdin_t : public data_source_t
{
    Q_OBJECT

public:
    explicit data_source_stdin_t(QObject* parent = nullptr);

    QString text() const override { return {}; }
    QString info() const override { return {}; }
    QString error_message() const override { return {}; }

private:
    stdin_socket_notifier_t* _notifier;
};
} // namespace flan
