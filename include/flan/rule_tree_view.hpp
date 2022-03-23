
#pragma once

#include <QTreeView>

namespace flan
{
class tree_view_t : public QTreeView
{
    Q_OBJECT

public:
    tree_view_t(QWidget* parent = nullptr);

    void contextMenuEvent(QContextMenuEvent* event) override;
};
} // namespace flan
