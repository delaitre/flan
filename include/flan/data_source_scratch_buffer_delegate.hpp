
#pragma once

#include <flan/data_source_delegate.hpp>
#include <QDialog>

namespace flan
{
class data_source_scratch_buffer_t;

class data_source_scratch_buffer_delegate_t : public data_source_delegate_t
{
    Q_OBJECT

public:
    data_source_scratch_buffer_delegate_t(
        data_source_scratch_buffer_t& data_source,
        QObject* parent = nullptr);

    bool is_settings_supported() const override { return false; }
    QDialog* settings_dialog(QWidget* parent = nullptr) override { return nullptr; }
};
} // namespace flan
