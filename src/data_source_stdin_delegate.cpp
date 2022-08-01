
#include <flan/data_source_stdin.hpp>
#include <flan/data_source_stdin_delegate.hpp>

namespace flan
{
data_source_stdin_delegate_t::data_source_stdin_delegate_t(
    data_source_stdin_t& data_source,
    QObject* parent)
    : data_source_delegate_t{tr("Standard input (stdin)"), data_source, parent}
{
}
} // namespace flan
