
#include <flan/data_source.hpp>
#include <flan/data_source_delegate.hpp>

namespace flan
{
data_source_delegate_t::data_source_delegate_t(
    QString name,
    data_source_t& data_source,
    QObject* parent)
    : QObject{parent}
    , _name{name}
    , _data_source{data_source}
{
    connect(
        &_data_source, &data_source_t::info_changed, this, &data_source_delegate_t::info_changed);

    connect(
        &_data_source, &data_source_t::error_changed, this, &data_source_delegate_t::error_changed);
}

QString data_source_delegate_t::info() const
{
    return _data_source.info();
}

QString data_source_delegate_t::error_message() const
{
    return _data_source.error_message();
}
} // namespace flan
