
#pragma once

#include <flan/data_source_delegate_provider.hpp>

namespace flan
{
class data_source_stdin_t;
class data_source_delegate_stdin_t;

class data_source_delegate_provider_stdin_t : public data_source_delegate_provider_t
{
    Q_OBJECT

public:
    explicit data_source_delegate_provider_stdin_t(QObject* parent = nullptr);

    std::vector<data_source_delegate_t*> delegates() const override;

private:
    data_source_stdin_t* _source; //!< The only accessible data source.
    data_source_delegate_stdin_t* _delegate; //!< The only accessible delegate.
};
} // namespace flan
