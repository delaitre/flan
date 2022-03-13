
#include <flan/rule_model.hpp>

namespace
{
constexpr quintptr rule_node_id = 1;
} // namespace

namespace flan
{
rule_model_t::rule_model_t(QObject* parent)
    : QAbstractItemModel{parent}
{
}

void rule_model_t::set_rules(QVector<matching_rule_t> rules)
{
    if (_rules == rules)
        return;

    beginResetModel();
    _rules = rules;
    endResetModel();
}

QModelIndex rule_model_t::index(int row, int column, const QModelIndex& parent) const
{
    if (parent.isValid())
    {
        switch (parent.internalId())
        {
        case rule_node_id:
            return {};
        default:
            return {};
        }
    }
    else
    {
        // The child of the root is a rule node.
        return createIndex(row, column, rule_node_id);
    }

    return {};
}

QModelIndex rule_model_t::parent(const QModelIndex& child) const
{
    if (child.isValid())
    {
        switch (child.internalId())
        {
        case rule_node_id:
            // The parent of a rule node is the root
            return {};
        default:
            return {};
        }
    }

    return {};
}

int rule_model_t::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
    {
        switch (parent.internalId())
        {
        case rule_node_id:
            return 0;
        default:
            return 0;
        }
    }
    else
    {
        // The root node has one row per rule under it.
        return _rules.size();
    }

    return 0;
}

int rule_model_t::columnCount(const QModelIndex& parent) const
{
    (void)parent;
    return 4;
}

QVariant rule_model_t::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (orientation)
    {
    case Qt::Horizontal:
        switch (section)
        {
        case rule_column_index:
            switch (role)
            {
            case Qt::DisplayRole:
                return tr("Rule");
            default:
                return {};
            }

            return {};
        case remove_column_index:
            switch (role)
            {
            case Qt::DisplayRole:
                return tr("Remove");
            default:
                return {};
            }

            return {};
        case keep_column_index:
            switch (role)
            {
            case Qt::DisplayRole:
                return tr("Keep");
            default:
                return {};
            }

            return {};
        case highlight_column_index:
            switch (role)
            {
            case Qt::DisplayRole:
                return tr("Highlight");
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

QVariant rule_model_t::data(const QModelIndex& index, int role) const
{
    auto& rule = _rules[index.row()];

    switch (index.column())
    {
    case rule_column_index:
        switch (role)
        {
        case Qt::DisplayRole:
            [[fallthrough]];
        case Qt::EditRole:
            return rule.name;
        case Qt::ToolTipRole:
            return rule.rule.pattern();
        default:
            return {};
        }
        return {};
    case remove_column_index:
        switch (role)
        {
        case Qt::CheckStateRole:
            return (rule.behaviour == filtering_behaviour_t::remove_line) ? Qt::Checked :
                                                                            Qt::Unchecked;
        case Qt::ToolTipRole:
            return tr("Line with a match will be marked for removal.");
        default:
            return {};
        }
        return {};
    case keep_column_index:
        switch (role)
        {
        case Qt::CheckStateRole:
            return (rule.behaviour == filtering_behaviour_t::keep_line) ? Qt::Checked :
                                                                          Qt::Unchecked;
        case Qt::ToolTipRole:
            return tr("Line with a match will be marked for keeping.");
        default:
            return {};
        }
        return {};
    case highlight_column_index:
        switch (role)
        {
        case Qt::CheckStateRole:
            return rule.highlight_match ? Qt::Checked : Qt::Unchecked;
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

bool rule_model_t::setData(const QModelIndex& index, const QVariant& value, int role)
{
    auto& rule = _rules[index.row()];

    switch (index.column())
    {
    case rule_column_index:
        switch (role)
        {
        case Qt::DisplayRole:
            [[fallthrough]];
        case Qt::EditRole:
            rule.name = value.toString();
            emit dataChanged(index, index, QVector<int>{} << role);
            return true;
        default:
            return false;
        }
        return false;
    case remove_column_index:
        switch (role)
        {
        case Qt::CheckStateRole:
        {
            // If remove just got checked, apply this as the new behaviour which will uncheck keep
            // if it was checked (we don't want both keep and remove checked at the same time).
            // If remove just got unchecked, since keep should not be checked at this time (as
            // remove was checked before), we can apply no behaviour.
            if (value.toInt() == Qt::Checked)
                rule.behaviour = filtering_behaviour_t::remove_line;
            else
                rule.behaviour = filtering_behaviour_t::none;

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
            // If keep just got checked, apply this as the new behaviour which will uncheck remove
            // if it was checked (we don't want both keep and remove checked at the same time).
            // If keep just got unchecked, since remove should not be checked at this time (as
            // keep was checked before), we can apply no behaviour.
            if (value.toInt() == Qt::Checked)
                rule.behaviour = filtering_behaviour_t::keep_line;
            else
                rule.behaviour = filtering_behaviour_t::none;

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
            rule.highlight_match = (value.toInt() == Qt::Checked);
            emit dataChanged(index, index, QVector<int>{} << role);
            return true;
        default:
            return false;
        }
        return false;
    default:
        return false;
    }

    return false;
}

Qt::ItemFlags rule_model_t::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    switch (index.internalId())
    {
    case rule_node_id:
        switch (index.column())
        {
        case rule_column_index:
            return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled
                | Qt::ItemNeverHasChildren;
        case remove_column_index:
            [[fallthrough]];
        case keep_column_index:
            [[fallthrough]];
        case highlight_column_index:
            return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
        default:
            return Qt::NoItemFlags;
        }
        return Qt::NoItemFlags;
    default:
        return Qt::NoItemFlags;
    }

    return Qt::NoItemFlags;
}
} // namespace flan
