
#include "stdin_socket_notifier.hpp"
#include <flan/data_source_stdin.hpp>

namespace flan
{
data_source_stdin_t::data_source_stdin_t(QObject* parent)
    : data_source_t{parent}
    , _notifier{new stdin_socket_notifier_t{this}}
{
    connect(_notifier, &stdin_socket_notifier_t::new_line, this, &data_source_stdin_t::new_text);
}
} // namespace flan
