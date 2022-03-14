
#pragma once

#include <QAbstractItemModel>

namespace flan
{
class base_node_t;
class group_node_t;
class rule_node_t;

class rule_model_t : public QAbstractItemModel
{
    Q_OBJECT

public:
    static constexpr int name_column_index = 0;
    static constexpr int remove_column_index = 1;
    static constexpr int keep_column_index = 2;
    static constexpr int highlight_column_index = 3;

public:
    rule_model_t(QObject* parent = nullptr);

    inline const base_node_t* root() const { return _root; }

public slots:

    void set_root(base_node_t* root);

public:
    QModelIndex index(int row, int column, const QModelIndex& parent) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

private:
    QVariant data_for_base_node(const base_node_t& node, const QModelIndex& index, int role) const;
    QVariant data_for_group_node(const group_node_t& node, const QModelIndex& index, int role)
        const;
    QVariant data_for_rule_node(const rule_node_t& node, const QModelIndex& index, int role) const;
    bool set_data_for_base_node(
        base_node_t& node,
        const QModelIndex& index,
        const QVariant& value,
        int role);
    bool set_data_for_group_node(
        group_node_t& node,
        const QModelIndex& index,
        const QVariant& value,
        int role);
    bool set_data_for_rule_node(
        rule_node_t& node,
        const QModelIndex& index,
        const QVariant& value,
        int role);
    Qt::ItemFlags flags_for_base_node(const base_node_t& node, const QModelIndex& index) const;
    Qt::ItemFlags flags_for_group_node(const group_node_t& node, const QModelIndex& index) const;
    Qt::ItemFlags flags_for_rule_node(const rule_node_t& node, const QModelIndex& index) const;

private:
    base_node_t* _root = nullptr;
};
} // namespace flan
