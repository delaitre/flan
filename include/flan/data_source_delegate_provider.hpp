
#pragma once

#include <QObject>

namespace flan
{
class data_source_delegate_t;

class data_source_delegate_provider_t : public QObject
{
    Q_OBJECT

public:
    explicit data_source_delegate_provider_t(QObject* parent = nullptr);

    virtual std::vector<data_source_delegate_t*> delegates() const = 0;

signals:
    void data_source_delegates_changed(std::vector<flan::data_source_delegate_t*> delegates);
};
} // namespace flan
