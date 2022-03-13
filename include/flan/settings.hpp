
#pragma once

#include <flan/matching_rule.hpp>
#include <QSettings>
#include <QVector>

namespace flan
{
QSettings get_default_settings();

void save_rules_to_settings(const QVector<matching_rule_t>& rules, QSettings& settings);

QVector<matching_rule_t> load_rules_from_settings(QSettings& settings);
} // namespace flan
