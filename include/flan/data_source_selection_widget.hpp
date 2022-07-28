
#pragma once

#include <QComboBox>
#include <QToolButton>
#include <QWidget>

namespace flan
{
class elided_label_t;
class data_source_t;
class data_source_delegate_t;

class data_source_selection_widget_t : public QWidget
{
    Q_OBJECT

public:
    using data_source_delegate_list_t = std::vector<data_source_delegate_t*>;

public:
    explicit data_source_selection_widget_t(QWidget* parent = nullptr);

    void set_data_sources(data_source_delegate_list_t delegates);

signals:
    void current_data_source_changed(flan::data_source_t* data_source);

private:
    data_source_delegate_t* current_delegate() const;

private slots:
    void update_info_and_settings();
    void show_configuration_dialog();

private:
    QComboBox* _data_source_combobox = nullptr;
    QToolButton* _configuration_button = nullptr;
    elided_label_t* _info_label = nullptr;
    elided_label_t* _error_label = nullptr;

    data_source_delegate_list_t _delegates;
};
} // namespace flan
