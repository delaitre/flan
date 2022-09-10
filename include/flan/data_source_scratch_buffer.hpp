
#pragma once

#include <flan/data_source.hpp>

namespace flan
{
class data_source_scratch_buffer_t : public data_source_t
{
    Q_OBJECT

public:
    explicit data_source_scratch_buffer_t(QObject* parent = nullptr);

    QString name() const override { return tr("Scratch buffer"); }
    QString text() const override { return {}; }
    QString error_message() const override { return {}; }
};
} // namespace flan
