
#include <flan/rule_model.hpp>
#include <flan/rule_tree_view.hpp>
#include <QContextMenuEvent>
#include <QMenu>

// Q_INIT_RESOURCE has to be called from the global namespace, hence this workaround.
inline void init_resource()
{
    Q_INIT_RESOURCE(libflan);
}

namespace flan
{
tree_view_t::tree_view_t(QWidget* parent)
    : QTreeView{parent}
{
    init_resource();

    setSelectionMode(QAbstractItemView::ExtendedSelection);
}

void tree_view_t::contextMenuEvent(QContextMenuEvent* event)
{
    if (auto indices = selectedIndexes(); indices.isEmpty())
    {
        QMenu menu{this};

        auto rule_menu = menu.addMenu(QIcon{":/icons/regex"}, tr("Rule"));
        auto action_add_rule = rule_menu->addAction(tr("Add"));

        auto group_menu = menu.addMenu(QIcon{":/icons/group"}, tr("Group"));
        auto action_add_group = group_menu->addAction(tr("Add"));

        auto model = static_cast<rule_model_t*>(this->model());

        connect(action_add_rule, &QAction::triggered, &menu, [this, model]() {
            model->insert_rule(model->rowCount({}), {});
        });

        connect(action_add_group, &QAction::triggered, &menu, [this, model]() {
            model->insert_group(model->rowCount({}), {});
        });

        menu.exec(event->globalPos());

        event->accept();
    }
    else if (indices.size() == 1)
    {
        auto index = indices.first();

        QMenu menu{this};

        auto rule_menu = menu.addMenu(QIcon{":/icons/regex"}, tr("Rule"));
        auto action_add_rule_before = rule_menu->addAction(tr("Insert before"));
        auto action_add_rule_after = rule_menu->addAction(tr("Insert after"));
        auto action_add_rule_child = rule_menu->addAction(tr("Insert child"));

        auto group_menu = menu.addMenu(QIcon{":/icons/group"}, tr("Group"));
        auto action_add_group_before = group_menu->addAction(tr("Insert before"));
        auto action_add_group_after = group_menu->addAction(tr("Insert after"));
        auto action_add_group_child = group_menu->addAction(tr("Insert child"));

        auto action_remove =
            menu.addAction(style()->standardIcon(QStyle::SP_TrashIcon), tr("Remove"));

        auto model = static_cast<rule_model_t*>(this->model());

        connect(action_remove, &QAction::triggered, &menu, [this, model, index]() {
            model->remove_node(index.row(), index.parent());
        });

        connect(action_add_rule_before, &QAction::triggered, &menu, [this, model, index]() {
            model->insert_rule(index.row(), index.parent());
        });
        connect(action_add_rule_after, &QAction::triggered, &menu, [this, model, index]() {
            model->insert_rule(
                std::min(index.row() + 1, model->rowCount(index.parent())), index.parent());
        });
        connect(action_add_rule_child, &QAction::triggered, &menu, [this, model, index]() {
            model->insert_rule(model->rowCount(index), index);
        });

        connect(action_add_group_before, &QAction::triggered, &menu, [this, model, index]() {
            model->insert_group(index.row(), index.parent());
        });
        connect(action_add_group_after, &QAction::triggered, &menu, [this, model, index]() {
            model->insert_group(
                std::min(index.row() + 1, model->rowCount(index.parent())), index.parent());
        });
        connect(action_add_group_child, &QAction::triggered, &menu, [this, model, index]() {
            model->insert_group(model->rowCount(index), index);
        });

        menu.exec(event->globalPos());

        event->accept();
    }
    else
    {
        QMenu menu{this};

        auto check_menu = menu.addMenu(QIcon{":/icons/checked"}, tr("Check"));
        auto uncheck_menu = menu.addMenu(QIcon{":/icons/unchecked"}, tr("Uncheck"));
        auto action_check_remove_matches = check_menu->addAction(tr("Hide matches"));
        auto action_uncheck_remove_matches = uncheck_menu->addAction(tr("Hide matches"));
        auto action_check_keep_matches = check_menu->addAction(tr("Keep matches"));
        auto action_uncheck_keep_matches = uncheck_menu->addAction(tr("Keep matches"));
        auto action_check_highlight_matches = check_menu->addAction(tr("Highlight matches"));
        auto action_uncheck_highlight_matches = uncheck_menu->addAction(tr("Highlight matches"));

        auto action_remove =
            menu.addAction(style()->standardIcon(QStyle::SP_TrashIcon), tr("Remove"));

        auto model = static_cast<rule_model_t*>(this->model());

        connect(action_remove, &QAction::triggered, &menu, [this, model, indices]() {
            // Need to use persistent indices to ensure indices stay stable when removing items
            // one by one (as otherwise indices would get invalidated).
            QList<QPersistentModelIndex> persistent_indices{indices.begin(), indices.end()};
            for (auto& index: persistent_indices)
                model->remove_node(index.row(), index.parent());
        });

        auto set_check_state_role = [model, indices](Qt::CheckState state, int column_index) {
            for (auto index: indices)
            {
                model->setData(index.siblingAtColumn(column_index), state, Qt::CheckStateRole);
            }
        };

        connect(action_check_remove_matches, &QAction::triggered, &menu, [=]() {
            set_check_state_role(Qt::Checked, rule_model_t::remove_column_index);
        });

        connect(action_uncheck_remove_matches, &QAction::triggered, &menu, [=]() {
            set_check_state_role(Qt::Unchecked, rule_model_t::remove_column_index);
        });

        connect(action_check_keep_matches, &QAction::triggered, &menu, [=]() {
            set_check_state_role(Qt::Checked, rule_model_t::keep_column_index);
        });

        connect(action_uncheck_keep_matches, &QAction::triggered, &menu, [=]() {
            set_check_state_role(Qt::Unchecked, rule_model_t::keep_column_index);
        });

        connect(action_check_highlight_matches, &QAction::triggered, &menu, [=]() {
            set_check_state_role(Qt::Checked, rule_model_t::highlight_column_index);
        });

        connect(action_uncheck_highlight_matches, &QAction::triggered, &menu, [=]() {
            set_check_state_role(Qt::Unchecked, rule_model_t::highlight_column_index);
        });

        menu.exec(event->globalPos());

        event->accept();
    }
}
} // namespace flan
