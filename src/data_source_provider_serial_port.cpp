
#include <flan/data_source_provider_serial_port.hpp>
#include <QtSerialPort/QSerialPortInfo>

namespace flan
{
data_source_provider_serial_port_t::data_source_provider_serial_port_t(QObject* parent)
    : QObject{parent}
{
    _timer.setInterval(2000);

    connect(
        &_timer, &QTimer::timeout, this, &data_source_provider_serial_port_t::update_data_sources);
    update_data_sources();

    _timer.start();
}

std::vector<data_source_serial_port_t*> data_source_provider_serial_port_t::data_sources() const
{
    return _data_sources;
}

void data_source_provider_serial_port_t::update_data_sources()
{
    bool changed = false;

    for (auto& port: QSerialPortInfo::availablePorts())
    {
        if (std::find_if(
                _data_sources.begin(),
                _data_sources.end(),
                [port_name = port.portName()](data_source_serial_port_t* data_source) {
                    return data_source->port_name() == port_name;
                })
            == _data_sources.end())
        {
            changed = true;
            _data_sources.push_back(new data_source_serial_port_t{port, this});
        }
    }

    if (changed)
        emit data_sources_changed(_data_sources);
}
} // namespace flan
