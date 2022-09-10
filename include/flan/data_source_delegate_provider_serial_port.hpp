
#pragma once

#include <flan/data_source_delegate_provider.hpp>
#include <flan/data_source_provider_serial_port.hpp>

namespace flan
{
class data_source_delegate_provider_serial_port_t : public data_source_delegate_provider_t
{
    Q_OBJECT

public:
    explicit data_source_delegate_provider_serial_port_t(QObject* parent = nullptr);

    std::vector<data_source_delegate_t*> delegates() const override;

private:
    void update_delegates();

private:
    data_source_provider_serial_port_t _provider;
    std::vector<data_source_delegate_t*> _delegates;
};
} // namespace flan
