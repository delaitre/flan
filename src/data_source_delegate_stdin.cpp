
#include <flan/data_source_delegate_stdin.hpp>
#include <flan/data_source_stdin.hpp>

namespace flan
{
data_source_delegate_stdin_t::data_source_delegate_stdin_t(
    data_source_stdin_t& data_source,
    QObject* parent)
    : data_source_delegate_t{data_source, parent}
{
}
} // namespace flan
