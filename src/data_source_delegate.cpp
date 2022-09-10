
#include <flan/data_source.hpp>
#include <flan/data_source_delegate.hpp>

namespace flan
{
data_source_delegate_t::data_source_delegate_t(data_source_t& data_source, QObject* parent)
    : QObject{parent}
    , _data_source{data_source}
{
}

data_source_t& data_source_delegate_t::data_source() const
{
    return _data_source;
}

QWidget* data_source_delegate_t::create_view(QWidget* parent) const
{
    (void)parent;
    return nullptr;
}
} // namespace flan
