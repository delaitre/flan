
#include <flan/settings.hpp>
#include <QStringLiteral>

namespace
{
static const auto settings_key_rules{"rules"};
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

void save_rules_to_settings(const QVector<matching_rule_t>& rules, QSettings& settings)
{
    settings.beginWriteArray(settings_key_rules, rules.size());
    for (int i = 0; i < rules.size(); ++i)
    {
        const auto& rule = rules[i];

        settings.setArrayIndex(i);
        settings.setValue(settings_key_rule_name, rule.name);
        settings.setValue(settings_key_rule_pattern, rule.rule.pattern());
        settings.setValue(settings_key_rule_behaviour, to_underlying(rule.behaviour));
        settings.setValue(settings_key_rule_highlight_match, rule.highlight_match);
    }
    settings.endArray();
}

QVector<matching_rule_t> load_rules_from_settings(QSettings& settings)
{
    QVector<matching_rule_t> rules;

    int size = settings.beginReadArray(settings_key_rules);
    rules.reserve(size);
    for (int i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);

        auto name = settings.value(settings_key_rule_name).toString();
        auto pattern = settings.value(settings_key_rule_pattern).toString();
        filtering_behaviour_t behaviour{settings.value(settings_key_rule_behaviour).toInt()};
        auto highlight_match = settings.value(settings_key_rule_highlight_match).toBool();

        rules << matching_rule_t{name, QRegularExpression{pattern}, behaviour, highlight_match};
    }
    settings.endArray();

    return rules;
}
} // namespace flan
