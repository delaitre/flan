
#pragma once

#include <flan/matching_rule.hpp>
#include <QAbstractItemModel>

namespace flan
{
class rule_model_t : public QAbstractItemModel
{
    Q_OBJECT

public:
    static constexpr int rule_column_index = 0;
    static constexpr int remove_column_index = 1;
    static constexpr int keep_column_index = 2;
    static constexpr int highlight_column_index = 3;

public:
    rule_model_t(QObject* parent = nullptr);

    inline const QVector<matching_rule_t>& rules() const { return _rules; }

public slots:

    void set_rules(QVector<matching_rule_t> rules);

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
    QVector<matching_rule_t> _rules;
};
} // namespace flan
