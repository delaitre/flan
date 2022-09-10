
#include <flan/data_source_delegate_serial_port.hpp>
#include <flan/data_source_serial_port.hpp>
#include <QAction>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QToolButton>
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

data_source_delegate_serial_port_t::data_source_delegate_serial_port_t(
    data_source_serial_port_t& data_source,
    QObject* parent)
    : data_source_delegate_t{data_source, parent}
    , _data_source_serial_port{data_source}
{
}

QWidget* data_source_delegate_serial_port_t::create_view(QWidget* parent) const
{
    auto open_button = new QPushButton{tr("Open")};
    open_button->setCheckable(true);

    auto settings_button = new QToolButton;
    settings_button->setAutoRaise(true);
    settings_button->setToolButtonStyle(Qt::ToolButtonTextOnly);

    auto settings_button_font = settings_button->font();
    settings_button_font.setItalic(true);
    settings_button->setFont(settings_button_font);

    auto layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(open_button);
    layout->addWidget(settings_button);

    auto main_widget = new QWidget{parent};
    main_widget->setLayout(layout);

    connect(open_button, &QPushButton::toggled, this, [this](bool checked) {
        if (checked)
            _data_source_serial_port.open();
        else
            _data_source_serial_port.close();
    });

    connect(
        &_data_source_serial_port,
        &data_source_serial_port_t::is_open_changed,
        open_button,
        &QPushButton::setChecked);
    open_button->setChecked(_data_source_serial_port.is_open());

    auto settings_action = new QAction{tr("Configure the serial port"), settings_button};
    settings_button->setDefaultAction(settings_action);
    connect(settings_button, &QToolButton::triggered, this, [this, settings_button]() {
        auto dialog = settings_dialog(settings_button);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        dialog->open();
    });

    connect(
        &_data_source_serial_port,
        &data_source_serial_port_t::info_changed,
        settings_action,
        &QAction::setText);
    settings_action->setText(_data_source_serial_port.info());

    return main_widget;
}

QDialog* data_source_delegate_serial_port_t::settings_dialog(QWidget* parent) const
{
    auto& settings = _data_source_serial_port.settings();

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

        settings.baudrate = baudrate_combobox->currentData().value<QSerialPort::BaudRate>();
        settings.flow_control =
            flow_control_combobox->currentData().value<QSerialPort::FlowControl>();
        settings.data_bits = data_bits_combobox->currentData().value<QSerialPort::DataBits>();
        settings.parity = parity_combobox->currentData().value<QSerialPort::Parity>();
        settings.stop_bits = stop_bits_combobox->currentData().value<QSerialPort::StopBits>();

        bool was_open = _data_source_serial_port.is_open();
        _data_source_serial_port.set_settings(std::move(settings));
        if (was_open)
            _data_source_serial_port.open();

        dialog->accept();
    });
    connect(button_box, &QDialogButtonBox::rejected, dialog, &QDialog::reject);

    return dialog;
}
} // namespace flan
