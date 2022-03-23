
#include <flan/rule_model.hpp>
#include <flan/rule_set.hpp>

#ifdef FLAN_TEST_RULE_MODEL
#include <QAbstractItemModelTester>
#endif

namespace flan
{
rule_model_t::rule_model_t(QObject* parent)
    : QAbstractItemModel{parent}
{
#ifdef FLAN_TEST_RULE_MODEL
    new QAbstractItemModelTester{this, QAbstractItemModelTester::FailureReportingMode::Fatal, this};
#endif
}

void rule_model_t::set_root(base_node_t* root)
{
    if (_root == root)
        return;

    beginResetModel();
    _root = root;
    endResetModel();
}

base_node_t* rule_model_t::node_at(const QModelIndex& index) const
{
    base_node_t* node = nullptr;
    if (index.isValid())
        node = static_cast<base_node_t*>(index.internalPointer());
    else
        node = _root;

    return node;
}

QModelIndex rule_model_t::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return {};

    auto node = node_at(parent);
    if (row < 0 || row >= node->child_count())
        return {};

    return createIndex(row, column, &node->child(row));
}

QModelIndex rule_model_t::parent(const QModelIndex& child) const
{
    if (!child.isValid())
        return {};

    auto node = node_at(child);
    auto parent_node = node->parent();

    if (!parent_node || parent_node == _root)
        return {};

    return createIndex(parent_node->index(), 0, parent_node);
}

int rule_model_t::rowCount(const QModelIndex& parent) const
{
    if (parent.column() > 0)
        return 0;

    auto node = node_at(parent);
    if (!node)
        return 0;

    return node->child_count();
}

int rule_model_t::columnCount(const QModelIndex& parent) const
{
    (void)parent;
    return column_count;
}

QVariant rule_model_t::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (orientation)
    {
    case Qt::Horizontal:
        switch (section)
        {
        case name_column_index:
            switch (role)
            {
            case Qt::DisplayRole:
                return tr("Name");
            case Qt::ToolTipRole:
                return tr("User friendly name of the rule or group");
            default:
                return {};
            }

            return {};
        case tooltip_column_index:
            switch (role)
            {
            case Qt::DisplayRole:
                return tr("Tooltip");
            case Qt::ToolTipRole:
                return tr(
                    "Text used in the tooltip when the mouse pointer is over a matched pattern");
            default:
                return {};
            }

            return {};
        case pattern_column_index:
            switch (role)
            {
            case Qt::DisplayRole:
                return tr("Pattern");
            case Qt::ToolTipRole:
                return tr("Pattern used to check if the behaviour applies to a line, or to "
                          "highlight matches");
            default:
                return {};
            }

            return {};
        case remove_column_index:
            switch (role)
            {
            case Qt::DisplayRole:
                return tr("-");
            case Qt::TextAlignmentRole:
                return Qt::AlignCenter;
            case Qt::ToolTipRole:
                return tr("Line with a match will be marked for removal");
            default:
                return {};
            }

            return {};
        case keep_column_index:
            switch (role)
            {
            case Qt::DisplayRole:
                return tr("+");
            case Qt::TextAlignmentRole:
                return Qt::AlignCenter;
            case Qt::ToolTipRole:
                return tr("Line with a match will be marked for keeping");
            default:
                return {};
            }

            return {};
        case highlight_column_index:
            switch (role)
            {
            case Qt::DisplayRole:
                return tr("^");
            case Qt::TextAlignmentRole:
                return Qt::AlignCenter;
            case Qt::ToolTipRole:
                return tr("Match will be highlighted");
            default:
                return {};
            }

            return {};

        default:
            return {};
        }

        return {};
    case Qt::Vertical:
        return {};
    }

    return {};
}

QVariant rule_model_t::data_for_base_node(
    const base_node_t& node,
    const QModelIndex& index,
    int role) const
{
    (void)node;
    (void)index;
    (void)role;

    return {};
}

QVariant rule_model_t::data_for_group_node(
    const group_node_t& node,
    const QModelIndex& index,
    int role) const
{
    switch (index.column())
    {
    case name_column_index:
        switch (role)
        {
        case Qt::DisplayRole:
            [[fallthrough]];
        case Qt::EditRole:
            return node.name();
        default:
            return {};
        }
        return {};
    default:
        return {};
    }

    return {};
}

QVariant rule_model_t::data_for_rule_node(
    const rule_node_t& node,
    const QModelIndex& index,
    int role) const
{
    switch (index.column())
    {
    case name_column_index:
        switch (role)
        {
        case Qt::DisplayRole:
            [[fallthrough]];
        case Qt::EditRole:
            return node.rule().name;
        case Qt::ToolTipRole:
            return tr("User friendly name of the rule");
        default:
            return {};
        }
        return {};
    case tooltip_column_index:
        switch (role)
        {
        case Qt::DisplayRole:
            [[fallthrough]];
        case Qt::EditRole:
            return node.rule().tooltip;
        case Qt::ToolTipRole:
            return tr("Text used in the tooltip when the mouse pointer is over a matched pattern");
        default:
            return {};
        }
        return {};
    case pattern_column_index:
        switch (role)
        {
        case Qt::DisplayRole:
            [[fallthrough]];
        case Qt::EditRole:
            return node.rule().rule.pattern();
        case Qt::ToolTipRole:
            return tr("Pattern used to check if the behaviour applies to a line, or to highlight "
                      "matches");
        default:
            return {};
        }
        return {};
    case remove_column_index:
        switch (role)
        {
        case Qt::CheckStateRole:
            return (node.rule().behaviour == filtering_behaviour_t::remove_line) ? Qt::Checked :
                                                                                   Qt::Unchecked;
        case Qt::ToolTipRole:
            return tr("Line with a match will be marked for removal");
        default:
            return {};
        }
        return {};
    case keep_column_index:
        switch (role)
        {
        case Qt::CheckStateRole:
            return (node.rule().behaviour == filtering_behaviour_t::keep_line) ? Qt::Checked :
                                                                                 Qt::Unchecked;
        case Qt::ToolTipRole:
            return tr("Line with a match will be marked for keeping");
        default:
            return {};
        }
        return {};
    case highlight_column_index:
        switch (role)
        {
        case Qt::CheckStateRole:
            return node.rule().highlight_match ? Qt::Checked : Qt::Unchecked;
        case Qt::ToolTipRole:
            return tr("Match will be highlighted");
        default:
            return {};
        }
        return {};
    default:
        return {};
    }

    return {};
}

QVariant rule_model_t::data(const QModelIndex& index, int role) const
{
    auto node = static_cast<base_node_t*>(index.internalPointer());
    if (!node)
        return {};

    switch (node->type())
    {
    case node_type_t::base:
        return data_for_base_node(*static_cast<const base_node_t*>(node), index, role);
    case node_type_t::group:
        return data_for_group_node(*static_cast<const group_node_t*>(node), index, role);
    case node_type_t::rule:
        return data_for_rule_node(*static_cast<const rule_node_t*>(node), index, role);
    }

    return {};
}

bool rule_model_t::set_data_for_base_node(
    base_node_t& node,
    const QModelIndex& index,
    const QVariant& value,
    int role)
{
    (void)node;
    (void)index;
    (void)value;
    (void)role;

    return false;
}

bool rule_model_t::set_data_for_group_node(
    group_node_t& node,
    const QModelIndex& index,
    const QVariant& value,
    int role)
{
    switch (index.column())
    {
    case name_column_index:
        switch (role)
        {
        case Qt::DisplayRole:
            [[fallthrough]];
        case Qt::EditRole:
        {
            node.set_name(value.toString());
            emit dataChanged(index, index, QVector<int>{} << role);
            return true;
        }
        default:
            return false;
        }
        return false;
    default:
        return false;
    }

    return false;
}

bool rule_model_t::set_data_for_rule_node(
    rule_node_t& node,
    const QModelIndex& index,
    const QVariant& value,
    int role)
{
    switch (index.column())
    {
    case name_column_index:
        switch (role)
        {
        case Qt::DisplayRole:
            [[fallthrough]];
        case Qt::EditRole:
        {
            auto rule = node.rule();
            rule.name = value.toString();
            node.set_rule(std::move(rule));
            emit dataChanged(index, index, QVector<int>{} << role);
            return true;
        }
        default:
            return false;
        }
        return false;
    case tooltip_column_index:
        switch (role)
        {
        case Qt::DisplayRole:
            [[fallthrough]];
        case Qt::EditRole:
        {
            auto rule = node.rule();
            rule.tooltip = value.toString();
            node.set_rule(std::move(rule));
            emit dataChanged(index, index, QVector<int>{} << role);
            return true;
        }
        default:
            return false;
        }
        return false;
    case pattern_column_index:
        switch (role)
        {
        case Qt::DisplayRole:
            [[fallthrough]];
        case Qt::EditRole:
        {
            auto rule = node.rule();
            rule.rule.setPattern(value.toString());
            node.set_rule(std::move(rule));
            emit dataChanged(index, index, QVector<int>{} << role);
            return true;
        }
        default:
            return false;
        }
        return false;
    case remove_column_index:
        switch (role)
        {
        case Qt::CheckStateRole:
        {
            auto rule = node.rule();

            // If remove just got checked, apply this as the new behaviour which will uncheck keep
            // if it was checked (we don't want both keep and remove checked at the same time).
            // If remove just got unchecked, since keep should not be checked at this time (as
            // remove was checked before), we can apply no behaviour.
            if (value.toInt() == Qt::Checked)
                rule.behaviour = filtering_behaviour_t::remove_line;
            else
                rule.behaviour = filtering_behaviour_t::none;

            node.set_rule(std::move(rule));

            auto keep_index = this->index(index.row(), keep_column_index, index.parent());
            emit dataChanged(index, index, QVector<int>{} << role);
            emit dataChanged(keep_index, keep_index, QVector<int>{} << role);
            return true;
        }
        default:
            return false;
        }
        return false;
    case keep_column_index:
        switch (role)
        {
        case Qt::CheckStateRole:
        {
            auto rule = node.rule();

            // If keep just got checked, apply this as the new behaviour which will uncheck remove
            // if it was checked (we don't want both keep and remove checked at the same time).
            // If keep just got unchecked, since remove should not be checked at this time (as
            // keep was checked before), we can apply no behaviour.
            if (value.toInt() == Qt::Checked)
                rule.behaviour = filtering_behaviour_t::keep_line;
            else
                rule.behaviour = filtering_behaviour_t::none;

            node.set_rule(std::move(rule));

            auto remove_index = this->index(index.row(), remove_column_index, index.parent());
            emit dataChanged(index, index, QVector<int>{} << role);
            emit dataChanged(remove_index, remove_index, QVector<int>{} << role);
            return true;
        }
        default:
            return false;
        }
        return false;
    case highlight_column_index:
        switch (role)
        {
        case Qt::CheckStateRole:
        {
            auto rule = node.rule();
            rule.highlight_match = (value.toInt() == Qt::Checked);
            node.set_rule(std::move(rule));
            emit dataChanged(index, index, QVector<int>{} << role);
            return true;
        }
        default:
            return false;
        }
        return false;
    default:
        return false;
    }

    return false;
}

bool rule_model_t::setData(const QModelIndex& index, const QVariant& value, int role)
{
    auto node = static_cast<base_node_t*>(index.internalPointer());
    if (!node)
        return false;

    switch (node->type())
    {
    case node_type_t::base:
        return set_data_for_base_node(*static_cast<base_node_t*>(node), index, value, role);
    case node_type_t::group:
        return set_data_for_group_node(*static_cast<group_node_t*>(node), index, value, role);
    case node_type_t::rule:
        return set_data_for_rule_node(*static_cast<rule_node_t*>(node), index, value, role);
    }

    return false;
}

Qt::ItemFlags rule_model_t::flags_for_base_node(const base_node_t& node, const QModelIndex& index)
    const
{
    (void)node;
    (void)index;

    return Qt::NoItemFlags;
}

Qt::ItemFlags rule_model_t::flags_for_group_node(const group_node_t& node, const QModelIndex& index)
    const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    switch (index.column())
    {
    case name_column_index:
        return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
    default:
        return Qt::NoItemFlags;
    }
    return Qt::NoItemFlags;
}

Qt::ItemFlags rule_model_t::flags_for_rule_node(const rule_node_t& node, const QModelIndex& index)
    const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    switch (index.column())
    {
    case name_column_index:
        [[fallthrough]];
    case tooltip_column_index:
        [[fallthrough]];
    case pattern_column_index:
        return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
    case remove_column_index:
        [[fallthrough]];
    case keep_column_index:
        [[fallthrough]];
    case highlight_column_index:
        return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
    default:
        return Qt::NoItemFlags;
    }
    return Qt::NoItemFlags;
}

Qt::ItemFlags rule_model_t::flags(const QModelIndex& index) const
{
    auto node = static_cast<base_node_t*>(index.internalPointer());
    if (!node)
        return Qt::NoItemFlags;

    switch (node->type())
    {
    case node_type_t::base:
        return flags_for_base_node(*static_cast<const base_node_t*>(node), index);
    case node_type_t::group:
        return flags_for_group_node(*static_cast<const group_node_t*>(node), index);
    case node_type_t::rule:
        return flags_for_rule_node(*static_cast<const rule_node_t*>(node), index);
    }

    return Qt::NoItemFlags;
}
} // namespace flan
