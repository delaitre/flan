
#include <flan/rule_model.hpp>
#include <flan/rule_tree_widget.hpp>
#include <QHeaderView>
#include <QTreeView>
#include <QVBoxLayout>

namespace flan
{
rule_tree_widget_t::rule_tree_widget_t(QWidget* parent)
    : QWidget{parent}
    , _tree{new QTreeView}
{
    auto main_layout = new QVBoxLayout;
    main_layout->addWidget(_tree);
    setLayout(main_layout);

    resize(400, 600);
}

rule_model_t* rule_tree_widget_t::model() const
{
    return static_cast<rule_model_t*>(_tree->model());
}

void rule_tree_widget_t::set_model(rule_model_t* model)
{
    bool was_empty = (!_tree->model() || (_tree->model()->rowCount() == 0));
    _tree->setModel(model);

    if (model)
    {
        // Resize the headers so that the rule name stretches while all the other columns share the
        // same fixed size.
        auto header = _tree->header();
        header->setStretchLastSection(false);
        header->setSectionResizeMode(rule_model_t::name_column_index, QHeaderView::Stretch);
        header->setSectionResizeMode(rule_model_t::remove_column_index, QHeaderView::Fixed);
        header->setSectionResizeMode(rule_model_t::keep_column_index, QHeaderView::Fixed);
        header->setSectionResizeMode(rule_model_t::highlight_column_index, QHeaderView::Fixed);

        auto biggest_section_size = std::max(
            {header->sectionSizeHint(rule_model_t::remove_column_index),
             header->sectionSizeHint(rule_model_t::keep_column_index),
             header->sectionSizeHint(rule_model_t::highlight_column_index)});

        header->resizeSection(rule_model_t::remove_column_index, biggest_section_size);
        header->resizeSection(rule_model_t::keep_column_index, biggest_section_size);
        header->resizeSection(rule_model_t::highlight_column_index, biggest_section_size);

        if (was_empty)
            _tree->expandAll();
    }
}
} // namespace flan
