
#include <flan/settings.hpp>
#include <QStringLiteral>

namespace
{
static const auto settings_key_node_type{"node_type"};
static const auto settings_key_node_type_base{"base"};
static const auto settings_key_node_type_group{"group"};
static const auto settings_key_node_type_rule{"rule"};
static const auto settings_key_node_data{"data"};
static const auto settings_key_node_children{"children"};

static const auto settings_key_group_name{"name"};

static const auto settings_key_rule_name{"name"};
static const auto settings_key_rule_pattern{"pattern"};
static const auto settings_key_rule_behaviour{"behaviour"};
static const auto settings_key_rule_highlight_match{"highlight_match"};

//! Cast an enum class value \a to its underlying type
template <typename Enum>
[[maybe_unused]] constexpr auto to_underlying(Enum e) noexcept
{
    return static_cast<std::underlying_type_t<Enum>>(e);
}
} // namespace

namespace flan
{
QSettings get_default_settings()
{
    return QSettings{QSettings::UserScope};
}

void save_rules_to_settings(const base_node_t& node, QSettings& settings)
{
    // Write the current node and its data according to its type.
    switch (node.type())
    {
    case node_type_t::base:
    {
        settings.setValue(settings_key_node_type, settings_key_node_type_base);
        break;
    }
    case node_type_t::group:
    {
        auto& group_node = static_cast<const group_node_t&>(node);

        settings.setValue(settings_key_node_type, settings_key_node_type_group);
        settings.beginGroup(settings_key_node_data);
        settings.setValue(settings_key_group_name, group_node.name());
        settings.endGroup();
        break;
    }
    case node_type_t::rule:
    {
        auto& rule_node = static_cast<const rule_node_t&>(node);

        settings.setValue(settings_key_node_type, settings_key_node_type_rule);
        settings.beginGroup(settings_key_node_data);
        settings.setValue(settings_key_rule_name, rule_node.rule().name);
        settings.setValue(settings_key_rule_pattern, rule_node.rule().rule.pattern());
        settings.setValue(settings_key_rule_behaviour, to_underlying(rule_node.rule().behaviour));
        settings.setValue(settings_key_rule_highlight_match, rule_node.rule().highlight_match);
        settings.endGroup();
        break;
    }
    }

    // Process recursively the children of the node.
    settings.beginWriteArray(settings_key_node_children, node.child_count());
    for (int i = 0; i < node.child_count(); ++i)
    {
        settings.setArrayIndex(i);
        save_rules_to_settings(node.child(i), settings);
    }
    settings.endArray();
}

base_node_uniq_t load_rules_from_settings(QSettings& settings)
{
    base_node_uniq_t node;

    // Read the current node and its data according to its type.
    // Create the corresponding node.
    auto node_type = settings.value(settings_key_node_type).toString();
    if (node_type == settings_key_node_type_base)
    {
        node = std::make_unique<base_node_t>();
    }
    else if (node_type == settings_key_node_type_group)
    {
        settings.beginGroup(settings_key_node_data);
        auto name = settings.value(settings_key_group_name).toString();
        settings.endGroup();

        node = std::make_unique<group_node_t>(name);
    }
    else if (node_type == settings_key_node_type_rule)
    {
        settings.beginGroup(settings_key_node_data);
        auto name = settings.value(settings_key_rule_name).toString();
        auto pattern = settings.value(settings_key_rule_pattern).toString();
        filtering_behaviour_t behaviour{settings.value(settings_key_rule_behaviour).toInt()};
        auto highlight_match = settings.value(settings_key_rule_highlight_match).toBool();
        settings.endGroup();

        node = std::make_unique<rule_node_t>(
            matching_rule_t{name, QRegularExpression{pattern}, behaviour, highlight_match});
    }

    // If the file was basically invalid, just create a default node.
    if (!node)
        node = std::make_unique<base_node_t>();

    // Process recursively the children of the node.
    int size = settings.beginReadArray(settings_key_node_children);
    for (int i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        node->add_child(load_rules_from_settings(settings));
    }
    settings.endArray();

    return node;
}
} // namespace flan
