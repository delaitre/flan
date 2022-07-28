
#include <flan/data_source_serial_port.hpp>
#include <flan/data_source_serial_port_delegate.hpp>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QtSerialPort/QSerialPortInfo>

namespace flan
{
namespace
{
void make_combobox_data_current(QComboBox& combobox, QVariant data)
{
    combobox.setCurrentIndex(std::max(0, combobox.findData(data)));
}
} // namespace

data_source_serial_port_delegate_t::data_source_serial_port_delegate_t(
    data_source_serial_port_t& data_source,
    QObject* parent)
    : data_source_delegate_t{tr("Serial port"), data_source, parent}
    , _data_source_serial_port{data_source}
{
}

QDialog* data_source_serial_port_delegate_t::settings_dialog(QWidget* parent)
{
    auto& settings = _data_source_serial_port.settings();

    auto port_selection_combobox = new QComboBox;
    for (auto& port: QSerialPortInfo::availablePorts())
    {
        QString name = port.portName();
        if (!port.description().isEmpty())
            name.append(QString(" [%1]").arg(port.description()));

        port_selection_combobox->addItem(name, port.systemLocation());
    }
    make_combobox_data_current(*port_selection_combobox, settings.port_name);

    auto baudrate_combobox = new QComboBox;
    for (auto& baudrate: QSerialPortInfo::standardBaudRates())
    {
        baudrate_combobox->addItem(QString::number(baudrate), baudrate);
    }
    make_combobox_data_current(*baudrate_combobox, settings.baudrate);

    auto flow_control_combobox = new QComboBox;
    flow_control_combobox->addItem(tr("No flow control"), QSerialPort::NoFlowControl);
    flow_control_combobox->addItem(tr("Hardware (RTS/CTS)"), QSerialPort::HardwareControl);
    flow_control_combobox->addItem(tr("Software (XON/XOFF)"), QSerialPort::SoftwareControl);
    make_combobox_data_current(*flow_control_combobox, settings.flow_control);

    auto data_bits_combobox = new QComboBox;
    data_bits_combobox->addItem("5", QSerialPort::Data5);
    data_bits_combobox->addItem("6", QSerialPort::Data6);
    data_bits_combobox->addItem("7", QSerialPort::Data7);
    data_bits_combobox->addItem("8", QSerialPort::Data8);
    make_combobox_data_current(*data_bits_combobox, settings.data_bits);

    auto parity_combobox = new QComboBox;
    parity_combobox->addItem(tr("No"), QSerialPort::NoParity);
    parity_combobox->addItem(tr("Even"), QSerialPort::EvenParity);
    parity_combobox->addItem(tr("Odd"), QSerialPort::OddParity);
    parity_combobox->addItem(tr("Space"), QSerialPort::SpaceParity);
    parity_combobox->addItem(tr("Mark"), QSerialPort::MarkParity);
    make_combobox_data_current(*parity_combobox, settings.parity);

    auto stop_bits_combobox = new QComboBox;
    stop_bits_combobox->addItem(tr("1"), QSerialPort::OneStop);
    stop_bits_combobox->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
    stop_bits_combobox->addItem(tr("2"), QSerialPort::TwoStop);
    make_combobox_data_current(*stop_bits_combobox, settings.stop_bits);

    auto form_layout = new QFormLayout;
    form_layout->addRow(tr("Port"), port_selection_combobox);
    form_layout->addRow(tr("Baudrate"), baudrate_combobox);
    form_layout->addRow(tr("Flow control"), flow_control_combobox);
    form_layout->addRow(tr("Data bits"), data_bits_combobox);
    form_layout->addRow(tr("Parity"), parity_combobox);
    form_layout->addRow(tr("Stop bits"), stop_bits_combobox);

    auto button_box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    auto main_layout = new QVBoxLayout;
    main_layout->addLayout(form_layout);
    main_layout->addWidget(button_box);

    auto dialog = new QDialog{parent};
    dialog->setWindowTitle(tr("Serial port settings"));
    dialog->setLayout(main_layout);

    connect(button_box, &QDialogButtonBox::accepted, dialog, [=]() {
        data_source_serial_port_t::settings_t settings;

        settings.port_name = port_selection_combobox->currentData().toString();
        settings.baudrate = baudrate_combobox->currentData().value<QSerialPort::BaudRate>();
        settings.flow_control =
            flow_control_combobox->currentData().value<QSerialPort::FlowControl>();
        settings.data_bits = data_bits_combobox->currentData().value<QSerialPort::DataBits>();
        settings.parity = parity_combobox->currentData().value<QSerialPort::Parity>();
        settings.stop_bits = stop_bits_combobox->currentData().value<QSerialPort::StopBits>();

        _data_source_serial_port.set_settings(std::move(settings));

        dialog->accept();
    });
    connect(button_box, &QDialogButtonBox::rejected, dialog, &QDialog::reject);

    return dialog;
}
} // namespace flan
