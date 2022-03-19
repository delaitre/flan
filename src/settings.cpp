
#include <flan/settings.hpp>
#include <QColor>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QStandardPaths>
#include <optional>

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
static const auto settings_key_rule_tooltip{"tooltip"};

static const auto settings_key_style_matches{"style_matches"};
static const auto settings_key_style_foreground_color{"foreground_color"};
static const auto settings_key_style_background_color{"background_color"};
static const auto settings_key_style_underline_color{"underline_color"};
static const auto settings_key_style_underline_style{"underline_style"};
static const auto settings_key_style_underline_style_none{"none"};
static const auto settings_key_style_underline_style_solid{"solid"};
static const auto settings_key_style_underline_style_dash{"dash"};
static const auto settings_key_style_underline_style_dot{"dot"};
static const auto settings_key_style_underline_style_wave{"wave"};

//! Cast an enum class value \a to its underlying type
template <typename Enum>
[[maybe_unused]] constexpr auto to_underlying(Enum e) noexcept
{
    return static_cast<std::underlying_type_t<Enum>>(e);
}
} // namespace

namespace flan
{
QString get_default_settings_directory()
{
    return QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
}

QString get_default_settings_file_for_rules()
{
    return QFileInfo{QDir{get_default_settings_directory()}, "rules.json"}.filePath();
}

QString get_default_settings_file_for_styles()
{
    return QFileInfo{QDir{get_default_settings_directory()}, "styles.json"}.filePath();
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
        map[settings_key_rule_tooltip] = rule.tooltip;
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
        auto tooltip = map.value(settings_key_rule_tooltip).toString();
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

        node = std::make_unique<rule_node_t>(matching_rule_t{
            name, QRegularExpression{pattern}, filtering_behaviour, highlight, tooltip});
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

QVariant style_to_variant(const matching_style_t& style)
{
    QVariantMap map;

    if (style.foreground_color.isValid())
        map[settings_key_style_foreground_color] = style.foreground_color.name(QColor::HexArgb);
    if (style.background_color.isValid())
        map[settings_key_style_background_color] = style.background_color.name(QColor::HexArgb);
    if (style.underline_color.isValid())
        map[settings_key_style_underline_color] = style.underline_color.name(QColor::HexArgb);

    QString underline_style_name;
    switch (style.underline_style)
    {
    case underline_style_t::none:
        // Don't actually write the style if it is none.
        break;
    case underline_style_t::solid:
        underline_style_name = settings_key_style_underline_style_solid;
        break;
    case underline_style_t::dash:
        underline_style_name = settings_key_style_underline_style_dash;
        break;
    case underline_style_t::dot:
        underline_style_name = settings_key_style_underline_style_dot;
        break;
    case underline_style_t::wave:
        underline_style_name = settings_key_style_underline_style_wave;
        break;
    }

    if (!underline_style_name.isEmpty())
        map[settings_key_style_underline_style] = underline_style_name;

    return {map};
}

QVariant styles_to_variant(const matching_style_list_t& styles)
{
    QVariantList styles_variant_list;
    for (const auto& style: styles)
        styles_variant_list.append(style_to_variant(style));

    QVariantMap root;
    root[settings_key_style_matches] = styles_variant_list;

    return {root};
}

void save_styles_to_json(const matching_style_list_t& styles, QString file_path)
{
    auto variant = styles_to_variant(styles);
    auto json = QJsonDocument::fromVariant(variant).toJson(QJsonDocument::JsonFormat::Indented);
    QFile file{file_path};
    file.open(QIODevice::WriteOnly);
    file.write(json);
    file.close();
}

std::optional<matching_style_t> style_from_variant(const QVariant& variant)
{
    if (!variant.canConvert<QVariantMap>())
        return {};
    auto map = variant.value<QVariantMap>();

    auto foreground = map.value(settings_key_style_foreground_color).toString();
    auto background = map.value(settings_key_style_background_color).toString();
    auto underline_color = map.value(settings_key_style_underline_color).toString();
    auto underline_style = map.value(settings_key_style_underline_style).toString();

    matching_style_t style;

    style.foreground_color = QColor{foreground};
    style.background_color = QColor{background};
    style.underline_color = QColor{underline_color};

    if (underline_style == settings_key_style_underline_style_none)
        style.underline_style = underline_style_t::none;
    else if (underline_style == settings_key_style_underline_style_solid)
        style.underline_style = underline_style_t::solid;
    else if (underline_style == settings_key_style_underline_style_dash)
        style.underline_style = underline_style_t::dash;
    else if (underline_style == settings_key_style_underline_style_dot)
        style.underline_style = underline_style_t::dot;
    else if (underline_style == settings_key_style_underline_style_wave)
        style.underline_style = underline_style_t::wave;
    else
        style.underline_style = underline_style_t::none;

    return style;
}

matching_style_list_t styles_from_variant(const QVariant& variant)
{
    // Top level variant should contain a map with an entry containing a list of styles.
    if (!variant.canConvert<QVariantMap>())
        return {};
    auto map = variant.value<QVariantMap>();

    // Extract the list of style matches.
    auto style_matches_variant = map.value(settings_key_style_matches);
    if (!style_matches_variant.canConvert<QVariantList>())
        return {};
    auto style_matches_variant_list = style_matches_variant.value<QVariantList>();

    matching_style_list_t style_matches;
    for (const auto& style_match_variant: style_matches_variant_list)
    {
        if (auto style = style_from_variant(style_match_variant))
            style_matches.push_back(*style);
    }

    return style_matches;
}

matching_style_list_t load_styles_from_json(QString file_path)
{
    QFile file{file_path};
    file.open(QIODevice::ReadOnly);
    auto json = file.readAll();
    file.close();
    auto variant = QJsonDocument::fromJson(json).toVariant();
    return styles_from_variant(variant);
}
} // namespace flan
