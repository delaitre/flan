
#include <flan/data_source_serial_port.hpp>
#include <QtSerialPort/QSerialPortInfo>

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

data_source_serial_port_t::data_source_serial_port_t(QSerialPortInfo info, QObject* parent)
    : data_source_t{parent}
    , _port{info}
{
    connect(&_port, &QSerialPort::errorOccurred, this, [this](auto error) {
        switch (error)
        {
        case QSerialPort::NoError:
            break;
        default:
            close();
            break;
        }

        emit error_changed(error_message());
    });

    connect(&_port, &QSerialPort::readyRead, this, [this]() {
        emit new_text(QString::fromUtf8(_port.readAll()));
    });

    set_settings(_settings);
}

void data_source_serial_port_t::open()
{
    if (!_port.isOpen())
    {
        _port.open(QIODevice::ReadOnly);
        emit info_changed(info());
        emit is_open_changed(is_open());
    }
}

void data_source_serial_port_t::close()
{
    if (_port.isOpen())
    {
        _port.close();
        emit info_changed(info());
        emit is_open_changed(is_open());
    }
}

bool data_source_serial_port_t::is_open() const
{
    return _port.isOpen();
}

void data_source_serial_port_t::set_settings(settings_t settings)
{
    _settings = settings;

    close();

    _port.setBaudRate(_settings.baudrate);
    _port.setFlowControl(_settings.flow_control);
    _port.setDataBits(_settings.data_bits);
    _port.setParity(_settings.parity);
    _port.setStopBits(_settings.stop_bits);

    emit info_changed(info());
}

QString data_source_serial_port_t::port_name() const
{
    return _port.portName();
}

QString data_source_serial_port_t::name() const
{
    QString name = _port.portName();
    if (auto desc = QSerialPortInfo{_port}.description(); !desc.isEmpty())
        name.append(QString(" [%1]").arg(desc));

    return name;
}

QString data_source_serial_port_t::info() const
{
    return QString("%1 @ %2-%3-%4")
        .arg(_settings.baudrate)
        .arg(_settings.data_bits)
        .arg(parity_to_short_string(_settings.parity))
        .arg(stop_bits_to_short_string(_settings.stop_bits));
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
