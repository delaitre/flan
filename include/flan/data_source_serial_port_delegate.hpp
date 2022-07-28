
#pragma once

#include <flan/data_source_delegate.hpp>

namespace flan
{
class data_source_serial_port_t;

class data_source_serial_port_delegate_t : public data_source_delegate_t
{
    Q_OBJECT

public:
    data_source_serial_port_delegate_t(
        data_source_serial_port_t& data_source,
        QObject* parent = nullptr);

    bool is_settings_supported() const override { return true; }
    QDialog* settings_dialog(QWidget* parent = nullptr) override;

private:
    data_source_serial_port_t& _data_source_serial_port;
};
} // namespace flan
