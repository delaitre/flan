
#include <flan/data_source_delegate_provider_scratch_buffer.hpp>
#include <flan/data_source_delegate_scratch_buffer.hpp>
#include <flan/data_source_scratch_buffer.hpp>

namespace flan
{
data_source_delegate_provider_scratch_buffer_t::data_source_delegate_provider_scratch_buffer_t(
    QObject* parent)
    : data_source_delegate_provider_t{parent}
    , _source{new data_source_scratch_buffer_t{this}}
    , _delegate{new data_source_delegate_scratch_buffer_t{*_source, this}}
{
}

std::vector<data_source_delegate_t*> data_source_delegate_provider_scratch_buffer_t::delegates()
    const
{
    return {_delegate};
}
} // namespace flan
