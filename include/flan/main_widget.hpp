
#pragma once

#include <flan/data_source_selection_widget.hpp>
#include <flan/matching_rule.hpp>
#include <flan/style.hpp>
#include <flan/timestamp_format.hpp>
#include <QWidget>

namespace flan
{
class rule_model_t;
class data_source_selection_widget_t;
class rule_tree_widget_t;
class log_widget_t;
class log_margin_area_widget_t;
class find_widget_t;
class data_source_t;
class find_controller_t;
class plot_t;

class main_widget_t : public QWidget
{
    Q_OBJECT

public:
    main_widget_t(QWidget* parent = nullptr);
    void set_data_sources(
        data_source_selection_widget_t::data_source_delegate_list_t data_source_list);

    const timestamp_format_list_t& timestamp_formats() const;
    void set_timestamp_formats(timestamp_format_list_t formats);

    //! Change the \a pattern used to extract data into curves for the plot.
    //!
    //! \sa plot_t::set_pattern for more info.
    void set_plot_pattern(QString pattern);

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
    log_margin_area_widget_t* _log_margin = nullptr;
    find_controller_t* _find_controller = nullptr;
    find_widget_t* _find = nullptr;
    plot_t* _plot = nullptr;
};
} // namespace flan
