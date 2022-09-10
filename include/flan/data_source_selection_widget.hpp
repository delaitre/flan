
#pragma once

#include <QComboBox>
#include <QHBoxLayout>
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
    void rebuild_custom_widgets();
    void update_error();

private:
    QComboBox* _data_source_combobox = nullptr;
    QHBoxLayout* _current_source_widget_layout = nullptr;
    elided_label_t* _error_label = nullptr;

    data_source_delegate_list_t _delegates;
};
} // namespace flan
