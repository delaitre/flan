
#include <flan/data_source_serial_port.hpp>
#include <QtDebug>

namespace flan
{
namespace
{
QString parity_to_short_string(QSerialPort::Parity parity)
{
    switch (parity)
    {
    case QSerialPort::NoParity:
        return "N";
    case QSerialPort::EvenParity:
        return "E";
    case QSerialPort::OddParity:
        return "O";
    case QSerialPort::SpaceParity:
        return "S";
    case QSerialPort::MarkParity:
        return "M";
    }

    return "?";
}

QString stop_bits_to_short_string(QSerialPort::StopBits stop_bits)
{
    switch (stop_bits)
    {
    case QSerialPort::OneStop:
        return "1";
    case QSerialPort::OneAndHalfStop:
        return "1.5";
    case QSerialPort::TwoStop:
        return "2";
    }

    return "?";
}
} // namespace

data_source_serial_port_t::data_source_serial_port_t(QObject* parent)
    : data_source_t{parent}
{
    connect(&_port, &QSerialPort::errorOccurred, this, [this](auto error) {
        switch (error)
        {
        case QSerialPort::NoError:
            break;
        default:
            if (_port.isOpen())
                _port.close();
            break;
        }

        emit error_changed(error_message());
    });

    connect(&_port, &QSerialPort::readyRead, this, [this]() {
        emit new_text(QString::fromUtf8(_port.readAll()));
    });
}

void data_source_serial_port_t::set_settings(settings_t settings)
{
    // Always re-apply the settings even if they did not change.
    // This allows to re-open the same port with the same settings after an error occured.

    _settings = settings;

    if (_port.isOpen())
        _port.close();

    _port.setPortName(_settings.port_name);
    _port.setBaudRate(_settings.baudrate);
    _port.setFlowControl(_settings.flow_control);
    _port.setDataBits(_settings.data_bits);
    _port.setParity(_settings.parity);
    _port.setStopBits(_settings.stop_bits);

    _port.open(QIODevice::ReadOnly);

    emit info_changed(info());
}

QString data_source_serial_port_t::info() const
{
    if (!_port.isOpen())
        return tr("<not open>");

    return QString("%1, %2 @ %3-%4-%5")
        .arg(_port.portName())
        .arg(_port.baudRate())
        .arg(_port.dataBits())
        .arg(parity_to_short_string(_port.parity()))
        .arg(stop_bits_to_short_string(_port.stopBits()));
}

QString data_source_serial_port_t::error_message() const
{
    switch (_port.error())
    {
    case QSerialPort::NoError:
        return {};
    default:
        return _port.errorString();
    }

    return {};
}
} // namespace flan
