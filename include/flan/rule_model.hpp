
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
    static constexpr int tooltip_column_index = 1;
    static constexpr int pattern_column_index = 2;
    static constexpr int remove_column_index = 3;
    static constexpr int keep_column_index = 4;
    static constexpr int highlight_column_index = 5;
    static constexpr int styles_column_index = 6;
    static constexpr int computed_styles_column_index = 7;
    static constexpr int column_count = 8;

public:
    rule_model_t(QObject* parent = nullptr);

    inline const base_node_t* root() const { return _root; }

    //! Insert a new rule node before the \a row in the child items of the \a parent.
    //!
    //! Returns \c true if the rule is inserted; otherwise returns \c false.
    bool insert_rule(int row, const QModelIndex& parent);

    //! Insert a new goup node before the \a row in the child items of the \a parent.
    //!
    //! Returns \c true if the group is inserted; otherwise returns \c false.
    bool insert_group(int row, const QModelIndex& parent);

    //! Remove the node at \a row from the child of the \a parent.
    //!
    //! Returns \c true if the node was successfuly removed; otherwise returns \a false.
    bool remove_node(int row, const QModelIndex& parent);

public slots:

    void set_root(flan::base_node_t* root);

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
    template <typename PreFunctor, typename PostFunctor>
    void visit(const QModelIndex& index, PreFunctor pre_visitor, PostFunctor post_visitor) const
    {
        assert(index.column() == 0);

        pre_visitor(index);
        for (int child_row = 0; child_row < rowCount(index); ++child_row)
            visit(this->index(child_row, 0, index), pre_visitor, post_visitor);
        post_visitor(index);
    }

    base_node_t* node_at(const QModelIndex& index) const;

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
