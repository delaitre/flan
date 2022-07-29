
#include <flan/rule_editor.hpp>
#include <flan/rule_model.hpp>
#include <flan/rule_tree_view.hpp>
#include <flan/rule_tree_widget.hpp>
#include <QHeaderView>
#include <QVBoxLayout>

namespace flan
{
rule_tree_widget_t::rule_tree_widget_t(QWidget* parent)
    : QWidget{parent}
    , _tree{new tree_view_t}
    , _rule_editor{new rule_editor_t}
{
    auto main_layout = new QVBoxLayout;
    main_layout->setContentsMargins(0, 0, 0, 0);
    main_layout->addWidget(_tree);
    main_layout->addWidget(_rule_editor);
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
    _rule_editor->set_model(model);

    if (model)
    {
        // Resize the headers so that the rule name stretches while all the other columns share the
        // same fixed size.
        auto header = _tree->header();
        header->setStretchLastSection(false);
        header->setSectionResizeMode(rule_model_t::name_column_index, QHeaderView::Stretch);
        header->setSectionHidden(rule_model_t::tooltip_column_index, true);
        header->setSectionHidden(rule_model_t::pattern_column_index, true);
        header->setSectionResizeMode(rule_model_t::remove_column_index, QHeaderView::Fixed);
        header->setSectionResizeMode(rule_model_t::keep_column_index, QHeaderView::Fixed);
        header->setSectionResizeMode(rule_model_t::highlight_column_index, QHeaderView::Fixed);
        header->setSectionHidden(rule_model_t::styles_column_index, true);
        header->setSectionHidden(rule_model_t::computed_styles_column_index, true);

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

    connect(
        _tree->selectionModel(),
        &QItemSelectionModel::currentRowChanged,
        _rule_editor,
        &rule_editor_t::set_current_model_index);
}
} // namespace flan
