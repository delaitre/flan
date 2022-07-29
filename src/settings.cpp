
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
static const auto settings_key_node_styles{"styles"};

static const auto settings_key_group_name{"name"};

static const auto settings_key_rule_name{"name"};
static const auto settings_key_rule_pattern{"pattern"};
static const auto settings_key_rule_behaviour{"behaviour"};
static const auto settings_key_rule_behaviour_none{"none"};
static const auto settings_key_rule_behaviour_remove_line{"remove_line"};
static const auto settings_key_rule_behaviour_keep_line{"keep_line"};
static const auto settings_key_rule_highlight_match{"highlight_match"};
static const auto settings_key_rule_tooltip{"tooltip"};

static const auto settings_key_style_foreground_color{"foreground_color"};
static const auto settings_key_style_background_color{"background_color"};
static const auto settings_key_style_font_style{"font_style"};
static const auto settings_key_style_font_style_normal{"normal"};
static const auto settings_key_style_font_style_italic{"italic"};
static const auto settings_key_style_font_weight{"font_weight"};
static const auto settings_key_style_font_weight_normal{"normal"};
static const auto settings_key_style_font_weight_bold{"bold"};

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

QVariant style_to_variant(const matching_style_t& style)
{
    QVariantMap map;

    if (style.foreground_color.isValid())
        map[settings_key_style_foreground_color] = style.foreground_color.name(QColor::HexArgb);
    if (style.background_color.isValid())
        map[settings_key_style_background_color] = style.background_color.name(QColor::HexArgb);

    QString font_style_name;
    switch (style.font_style)
    {
    case font_style_t::normal:
        // Don't actually write the style if it is normal.
        break;
    case font_style_t::italic:
        font_style_name = settings_key_style_font_style_italic;
        break;
    }

    if (!font_style_name.isEmpty())
        map[settings_key_style_font_style] = font_style_name;

    QString font_weight_name;
    switch (style.font_weight)
    {
    case font_weight_t::normal:
        // Don't actually write the style if it is normal.
        break;
    case font_weight_t::bold:
        font_weight_name = settings_key_style_font_weight_bold;
        break;
    }

    if (!font_weight_name.isEmpty())
        map[settings_key_style_font_weight] = font_weight_name;

    return {map};
}

QVariant styles_to_variant(const matching_style_list_t& styles)
{
    QVariantList styles_variant_list;
    for (const auto& style: styles)
        styles_variant_list.append(style_to_variant(style));

    return {styles_variant_list};
}

QVariant rule_to_variant(const base_node_t& node)
{
    QVariantMap map;

    // Type specific attributes
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

    // Generic attributes
    if (auto& styles = node.styles(); !styles.empty())
        map[settings_key_node_styles] = styles_to_variant(styles);

    QVariantList children;
    for (int i = 0; i < node.child_count(); ++i)
        children.append(rule_to_variant(node.child(i)));

    if (!children.empty())
        map[settings_key_node_children] = children;

    return {map};
}

QVariant rules_to_variant(const base_node_t& node)
{
    // Only save the styles and children for the root node.
    QVariantMap root;
    root[settings_key_node_styles] = styles_to_variant(node.styles());

    QVariantList children;
    for (int i = 0; i < node.child_count(); ++i)
        children.append(rule_to_variant(node.child(i)));

    root[settings_key_node_children] = children;

    return {root};
}

std::optional<matching_style_t> style_from_variant(const QVariant& variant)
{
    if (!variant.canConvert<QVariantMap>())
        return {};
    auto map = variant.value<QVariantMap>();

    auto foreground = map.value(settings_key_style_foreground_color).toString();
    auto background = map.value(settings_key_style_background_color).toString();
    auto font_style = map.value(settings_key_style_font_style).toString();
    auto font_weight = map.value(settings_key_style_font_weight).toString();

    matching_style_t style;

    style.foreground_color = QColor{foreground};
    style.background_color = QColor{background};

    if (font_style == settings_key_style_font_style_normal)
        style.font_style = font_style_t::normal;
    else if (font_style == settings_key_style_font_style_italic)
        style.font_style = font_style_t::italic;
    else
        style.font_style = font_style_t::normal;

    if (font_weight == settings_key_style_font_weight_normal)
        style.font_weight = font_weight_t::normal;
    else if (font_weight == settings_key_style_font_weight_bold)
        style.font_weight = font_weight_t::bold;
    else
        style.font_weight = font_weight_t::normal;

    return style;
}

matching_style_list_t styles_from_variant(const QVariant& variant)
{
    // Extract the list of style matches.
    if (!variant.canConvert<QVariantList>())
        return {};
    auto styles_variant_list = variant.value<QVariantList>();

    matching_style_list_t styles;
    for (const auto& style_variant: styles_variant_list)
    {
        if (auto style = style_from_variant(style_variant))
            styles.push_back(*style);
    }

    return styles;
}

base_node_uniq_t rule_from_variant(const QVariant& variant)
{
    if (!variant.canConvert<QVariantMap>())
        return {};
    auto map = variant.value<QVariantMap>();

    auto type = map.value(settings_key_node_type);
    if (!type.isValid())
        return {};

    // Type specific attributes
    base_node_uniq_t node;

    if (type == settings_key_node_type_base)
    {
        node = std::make_unique<base_node_t>();
    }
    else if (type == settings_key_node_type_group)
    {
        auto name = map.value(settings_key_group_name).toString();
        if (name.isEmpty())
            name = QObject::tr("New group");

        node = std::make_unique<group_node_t>(name);
    }
    else if (type == settings_key_node_type_rule)
    {
        auto name = map.value(settings_key_rule_name).toString();
        auto pattern = map.value(settings_key_rule_pattern).toString();
        auto behaviour = map.value(settings_key_rule_behaviour).toString();
        auto highlight = map.value(settings_key_rule_highlight_match).toBool();
        auto tooltip = map.value(settings_key_rule_tooltip).toString();

        if (name.isEmpty())
            name = QObject::tr("New rule");

        filtering_behaviour_t filtering_behaviour = filtering_behaviour_t::none;
        if (behaviour == settings_key_rule_behaviour_none)
            filtering_behaviour = filtering_behaviour_t::none;
        else if (behaviour == settings_key_rule_behaviour_remove_line)
            filtering_behaviour = filtering_behaviour_t::remove_line;
        else if (behaviour == settings_key_rule_behaviour_keep_line)
            filtering_behaviour = filtering_behaviour_t::keep_line;
        else
            filtering_behaviour = filtering_behaviour_t::none;

        node = std::make_unique<rule_node_t>(matching_rule_t{
            name, QRegularExpression{pattern}, filtering_behaviour, highlight, tooltip});
    }

    // Generic attributes
    auto styles = styles_from_variant(map.value(settings_key_node_styles));
    node->set_styles(std::move(styles));

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
    // Extract the root node first which should only have styles and children.
    if (!variant.canConvert<QVariantMap>())
        return {};
    auto root_map = variant.value<QVariantMap>();

    auto styles_variant = root_map.value(settings_key_node_styles);
    auto root_styles = styles_from_variant(styles_variant);

    auto children_variant = root_map.value(settings_key_node_children);
    if (!children_variant.canConvert<QVariantList>())
        return {};
    auto children_list = children_variant.value<QVariantList>();

    auto root = std::make_unique<base_node_t>();
    root->set_styles(std::move(root_styles));
    for (const auto& child_variant: children_list)
    {
        if (auto child = rule_from_variant(child_variant))
            root->add_child(std::move(child));
    }

    return root;
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

base_node_uniq_t load_rules_from_json(QString file_path)
{
    QFile file{file_path};
    if (!file.exists() || !file.open(QIODevice::ReadOnly))
        return std::make_unique<base_node_t>();
    auto json = file.readAll();
    file.close();
    auto variant = QJsonDocument::fromJson(json).toVariant();
    return rules_from_variant(variant);
}
} // namespace flan
