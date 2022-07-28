
#pragma once

#include <flan/data_source_selection_widget.hpp>
#include <flan/matching_rule.hpp>
#include <flan/style.hpp>
#include <QWidget>

namespace flan
{
class rule_model_t;
class data_source_selection_widget_t;
class rule_tree_widget_t;
class log_widget_t;
class data_source_t;

class main_widget_t : public QWidget
{
    Q_OBJECT

public:
    main_widget_t(QWidget* parent = nullptr);
    void set_data_sources(
        data_source_selection_widget_t::data_source_delegate_list_t data_source_list);

public slots:
    void set_content(const QString& text);
    void set_model(flan::rule_model_t* model);

private slots:
    void on_current_data_source_changed(flan::data_source_t* data_source);

private:
    data_source_t* _current_data_source = nullptr;
    data_source_selection_widget_t* _data_source = nullptr;
    rule_tree_widget_t* _rules = nullptr;
    log_widget_t* _log = nullptr;
};
} // namespace flan
