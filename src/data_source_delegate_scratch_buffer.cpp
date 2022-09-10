
#include <flan/data_source_delegate_scratch_buffer.hpp>
#include <flan/data_source_scratch_buffer.hpp>

namespace flan
{
data_source_delegate_scratch_buffer_t::data_source_delegate_scratch_buffer_t(
    data_source_scratch_buffer_t& data_source,
    QObject* parent)
    : data_source_delegate_t{data_source, parent}
{
}
} // namespace flan
