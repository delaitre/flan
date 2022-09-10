
#pragma once

#include <flan/data_source.hpp>
#include <QtSerialPort/QSerialPort>

namespace flan
{
class data_source_serial_port_t : public data_source_t
{
    Q_OBJECT

public:
    struct settings_t
    {
        qint32 baudrate = 2000000;
        QSerialPort::FlowControl flow_control = QSerialPort::NoFlowControl;
        QSerialPort::DataBits data_bits = QSerialPort::Data8;
        QSerialPort::Parity parity = QSerialPort::NoParity;
        QSerialPort::StopBits stop_bits = QSerialPort::OneStop;

        bool operator==(const settings_t& other) const
        {
            return (baudrate == other.baudrate) && (flow_control == other.flow_control)
                && (data_bits == other.data_bits) && (parity == other.parity)
                && (stop_bits == other.stop_bits);
        }
        bool operator!=(const settings_t& other) const { return !(*this == other); }
    };

public:
    explicit data_source_serial_port_t(QSerialPortInfo info, QObject* parent = nullptr);

    void open();
    void close();
    bool is_open() const;

    const settings_t& settings() const { return _settings; }
    void set_settings(settings_t settings);

    QString port_name() const;
    QString name() const override;
    QString text() const override { return {}; }
    QString info() const;
    QString error_message() const override;

signals:
    void info_changed(QString info);
    void is_open_changed(bool is_open);

private:
    settings_t _settings;
    QSerialPort _port;
};
} // namespace flan
