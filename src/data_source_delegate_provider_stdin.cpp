
#include <flan/data_source_delegate_provider_stdin.hpp>
#include <flan/data_source_delegate_stdin.hpp>
#include <flan/data_source_stdin.hpp>

namespace flan
{
data_source_delegate_provider_stdin_t::data_source_delegate_provider_stdin_t(QObject* parent)
    : data_source_delegate_provider_t{parent}
    , _source{new data_source_stdin_t{this}}
    , _delegate{new data_source_delegate_stdin_t{*_source, this}}
{
}

std::vector<data_source_delegate_t*> data_source_delegate_provider_stdin_t::delegates() const
{
    return {_delegate};
}
} // namespace flan
