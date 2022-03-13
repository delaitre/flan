
#pragma once

#include <QRegularExpression>

namespace flan
{
//! The filtering behaviour indicates what type of filtering should be applied to a line.
//!
//! Several filtering rules with conflicting behaviours might be used for the same line. Rules are
//! applied in order and the final state is used to determine if the line should be kept or removed.
enum class filtering_behaviour_t
{
    //! When a rule with the none behaviour applies, the line keep/remove state stays untouched.
    none,

    //! When a rule with the remove_line behaviour applies, the line is marked for removal.
    remove_line,

    //! When a rule with the keep_line behaviour applies, the line is marked for keeping.
    keep_line,
};

//! Describe a matching_rule_t and its associated behaviour.
struct matching_rule_t
{
    //! The user visible name of the rule.
    QString name;

    //! The regexp used to determine if this rule applies for a line of content.
    QRegularExpression rule;

    //! Indicates if a line matching the rule should be kept or remove.
    filtering_behaviour_t behaviour = filtering_behaviour_t::none;

    //! Indicates if the matching content should be highlighted.
    bool highlight_match = false;
};

inline bool operator==(const matching_rule_t& lhs, const matching_rule_t& rhs)
{
    return (lhs.name == rhs.name) && (lhs.rule == rhs.rule) && (lhs.behaviour == rhs.behaviour)
        && (lhs.highlight_match == rhs.highlight_match);
}

inline bool operator!=(const matching_rule_t& lhs, const matching_rule_t& rhs)
{
    return !(lhs == rhs);
}
} // namespace flan
