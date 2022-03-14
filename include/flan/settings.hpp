
#pragma once

#include <flan/rule_set.hpp>
#include <QSettings>

namespace flan
{
QSettings get_default_settings();

void save_rules_to_settings(const base_node_t& rules, QSettings& settings);

base_node_uniq_t load_rules_from_settings(QSettings& settings);
} // namespace flan
