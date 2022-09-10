
#pragma once

#include <flan/data_source_delegate.hpp>

namespace flan
{
class data_source_stdin_t;

class data_source_delegate_stdin_t : public data_source_delegate_t
{
    Q_OBJECT

public:
    data_source_delegate_stdin_t(data_source_stdin_t& data_source, QObject* parent = nullptr);
};
} // namespace flan
