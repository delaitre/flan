
#pragma once

#include <flan/rule_set.hpp>
#include <QString>

namespace flan
{
QString get_default_settings_directory();
QString get_default_settings_file_for_rules();

void save_rules_to_json(const base_node_t& node, QString file_path);

base_node_uniq_t load_rules_from_json(QString file_path);
} // namespace flan
