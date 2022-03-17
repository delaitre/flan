
#include <flan/settings.hpp>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QStandardPaths>

namespace
{
static const auto settings_key_rules{"rules"};
static const auto settings_key_node_type{"type"};
static const auto settings_key_node_type_base{"base"};
static const auto settings_key_node_type_group{"group"};
static const auto settings_key_node_type_rule{"rule"};
static const auto settings_key_node_children{"children"};

static const auto settings_key_group_name{"name"};

static const auto settings_key_rule_name{"name"};
static const auto settings_key_rule_pattern{"pattern"};
static const auto settings_key_rule_behaviour{"behaviour"};
static const auto settings_key_rule_behaviour_none{"none"};
static const auto settings_key_rule_behaviour_remove_line{"remove_line"};
static const auto settings_key_rule_behaviour_keep_line{"keep_line"};
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
QString get_default_settings_file()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    return QFileInfo{QDir{path}, "rules.json"}.filePath();
}

QVariant rule_to_variant(const base_node_t& node)
{
    QVariantMap map;

    switch (node.type())
    {
    case node_type_t::base:
        map[settings_key_node_type] = settings_key_node_type_base;
        break;
    case node_type_t::group:
    {
        auto& group_node = static_cast<const group_node_t&>(node);

        map[settings_key_node_type] = settings_key_node_type_group;
        map[settings_key_group_name] = group_node.name();
        break;
    }
    case node_type_t::rule:
    {
        auto& rule_node = static_cast<const rule_node_t&>(node);

        const auto& rule = rule_node.rule();

        map[settings_key_node_type] = settings_key_node_type_rule;
        map[settings_key_rule_name] = rule.name;
        map[settings_key_rule_pattern] = rule.rule.pattern();
        switch (rule.behaviour)
        {
        case filtering_behaviour_t::none:
            map[settings_key_rule_behaviour] = settings_key_rule_behaviour_none;
            break;
        case filtering_behaviour_t::remove_line:
            map[settings_key_rule_behaviour] = settings_key_rule_behaviour_remove_line;
            break;
        case filtering_behaviour_t::keep_line:
            map[settings_key_rule_behaviour] = settings_key_rule_behaviour_keep_line;
            break;
        }
        map[settings_key_rule_highlight_match] = rule.highlight_match;
        break;
    }
    }

    QVariantList children;
    for (int i = 0; i < node.child_count(); ++i)
        children.append(rule_to_variant(node.child(i)));

    if (!children.empty())
        map[settings_key_node_children] = children;

    return {map};
}

QVariant rules_to_variant(const base_node_t& node)
{
    // Skip the root node which shouldn't contain any data, and process each child directly to
    // add them in a list.
    QVariantList rules;
    for (int i = 0; i < node.child_count(); ++i)
        rules.append(rule_to_variant(node.child(i)));

    // Add the list to a root node. This allows the root node to only have a list, but none of the
    // other attribute.
    QVariantMap root;
    root[settings_key_rules] = rules;

    return {root};
}

void save_rules_to_json(const base_node_t& node, QString file_path)
{
    auto variant = rules_to_variant(node);
    auto json = QJsonDocument::fromVariant(variant).toJson(QJsonDocument::JsonFormat::Indented);
    QFile file{file_path};
    file.open(QIODevice::WriteOnly);
    file.write(json);
    file.close();
}

base_node_uniq_t rule_from_variant(const QVariant& variant)
{
    if (!variant.canConvert<QVariantMap>())
        return {};
    auto map = variant.value<QVariantMap>();

    auto type = map.value(settings_key_node_type);
    if (!type.isValid())
        return {};

    base_node_uniq_t node;

    if (type == settings_key_node_type_base)
    {
        node = std::make_unique<base_node_t>();
    }
    else if (type == settings_key_node_type_group)
    {
        auto name = map.value(settings_key_group_name).toString();
        if (name.isEmpty())
            return {};

        node = std::make_unique<group_node_t>(name);
    }
    else if (type == settings_key_node_type_rule)
    {
        auto name = map.value(settings_key_rule_name).toString();
        auto pattern = map.value(settings_key_rule_pattern).toString();
        auto behaviour = map.value(settings_key_rule_behaviour).toString();
        auto highlight = map.value(settings_key_rule_highlight_match).toBool();
        if (name.isEmpty() || pattern.isEmpty() || behaviour.isEmpty())
            return {};

        filtering_behaviour_t filtering_behaviour = filtering_behaviour_t::none;
        if (behaviour == settings_key_rule_behaviour_none)
            filtering_behaviour = filtering_behaviour_t::none;
        else if (behaviour == settings_key_rule_behaviour_remove_line)
            filtering_behaviour = filtering_behaviour_t::remove_line;
        else if (behaviour == settings_key_rule_behaviour_keep_line)
            filtering_behaviour = filtering_behaviour_t::keep_line;
        else
            return {};

        node = std::make_unique<rule_node_t>(
            matching_rule_t{name, QRegularExpression{pattern}, filtering_behaviour, highlight});
    }

    auto children = map.value(settings_key_node_children);
    if (children.canConvert<QVariantList>())
    {
        auto child_list = children.value<QVariantList>();
        for (const auto& child: child_list)
        {
            if (auto child_node = rule_from_variant(child))
                node->add_child(std::move(child_node));
        }
    }

    return node;
}

base_node_uniq_t rules_from_variant(const QVariant& variant)
{
    // Top level variant should contain a map with an entry containing a list of rules.
    if (!variant.canConvert<QVariantMap>())
        return {};
    auto map = variant.value<QVariantMap>();

    // Extract the list of rules.
    auto rules = map.value(settings_key_rules);
    if (!rules.canConvert<QVariantList>())
        return {};

    auto rule_list = rules.value<QVariantList>();
    auto root = std::make_unique<base_node_t>();
    for (const auto& rule: rule_list)
    {
        if (auto child = rule_from_variant(rule))
            root->add_child(std::move(child));
    }

    return root;
}

base_node_uniq_t load_rules_from_json(QString file_path)
{
    QFile file{file_path};
    file.open(QIODevice::ReadOnly);
    auto json = file.readAll();
    file.close();
    auto variant = QJsonDocument::fromJson(json).toVariant();
    return rules_from_variant(variant);
}
} // namespace flan
