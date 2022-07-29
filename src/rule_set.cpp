
#include <flan/rule_set.hpp>

namespace flan
{
void base_node_t::set_styles(matching_style_list_t styles)
{
    _styles = styles;
}

const matching_style_list_t& base_node_t::computed_styles() const
{
    // If the node has custom styles, use them.
    if (!_styles.empty())
        return _styles;

    // Otherwise, return the styles from the node's parent.
    if (parent())
        return parent()->computed_styles();

    // Or return default styles if this is the root node.
    return get_default_styles();
}
} // namespace flan
