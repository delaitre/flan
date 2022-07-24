
#pragma once

#include <flan/matching_rule.hpp>
#include <flan/style.hpp>
#include <vector>

namespace flan
{
//! A styled_matching_rule_t contains a matching rule with the style which should be applied if the
//! rule matches.
//!
//! The style is the computed style (i.e. it is inherited from a parent if not overriden).
struct styled_matching_rule_t
{
    matching_rule_t rule;
    matching_style_list_t styles;
};

using styled_matching_rule_list_t = std::vector<styled_matching_rule_t>;
} // namespace flan
