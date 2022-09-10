
#include <flan/data_source_delegate_provider_serial_port.hpp>
#include <flan/data_source_delegate_serial_port.hpp>

namespace flan
{
data_source_delegate_provider_serial_port_t::data_source_delegate_provider_serial_port_t(
    QObject* parent)
    : data_source_delegate_provider_t{parent}
{
    connect(
        &_provider,
        &data_source_provider_serial_port_t::data_sources_changed,
        this,
        &data_source_delegate_provider_serial_port_t::update_delegates);
    update_delegates();
}

std::vector<data_source_delegate_t*> data_source_delegate_provider_serial_port_t::delegates() const
{
    return _delegates;
}

void data_source_delegate_provider_serial_port_t::update_delegates()
{
    bool changed = false;

    for (auto data_source: _provider.data_sources())
    {
        if (std::find_if(
                _delegates.begin(),
                _delegates.end(),
                [data_source](data_source_delegate_t* delegate) {
                    return &delegate->data_source() == data_source;
                })
            == _delegates.end())
        {
            changed = true;
            _delegates.push_back(new data_source_delegate_serial_port_t{*data_source, this});
        }
    }

    if (changed)
        emit data_source_delegates_changed(_delegates);
}
} // namespace flan
