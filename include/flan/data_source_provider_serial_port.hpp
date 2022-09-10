
#pragma once

#include <flan/data_source_serial_port.hpp>
#include <QObject>
#include <QTimer>
#include <vector>

namespace flan
{
class data_source_serial_port_t;

class data_source_provider_serial_port_t : public QObject
{
    Q_OBJECT

public:
    explicit data_source_provider_serial_port_t(QObject* parent = nullptr);

    virtual std::vector<data_source_serial_port_t*> data_sources() const;

signals:
    void data_sources_changed(std::vector<flan::data_source_serial_port_t*> data_sources);

private slots:
    void update_data_sources();

private:
    std::vector<data_source_serial_port_t*> _data_sources;
    QTimer _timer;
};
} // namespace flan
