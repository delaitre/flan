
#pragma once

#include <QObject>
#include <QWidget>

namespace flan
{
class data_source_t;

class data_source_delegate_t : public QObject
{
    Q_OBJECT

public:
    explicit data_source_delegate_t(data_source_t& data_source, QObject* parent = nullptr);

    data_source_t& data_source() const;
    virtual QWidget* create_view(QWidget* parent = nullptr) const;

private:
    data_source_t& _data_source;
};
} // namespace flan
