
#pragma once

#include <flan/rule_set.hpp>
#include <flan/timestamp_format.hpp>
#include <QString>

namespace flan
{
QString get_default_settings_directory();
QString get_default_settings_file_for_rules();
QString get_default_settings_file_for_timestamp_formats();

void save_rules_to_json(const base_node_t& node, QString file_path);
base_node_uniq_t load_rules_from_json(QString file_path);

void save_timestamp_formats_to_json(const timestamp_format_list_t& formats, QString file_path);
timestamp_format_list_t load_timestamp_formats_from_json(QString file_path);
} // namespace flan
