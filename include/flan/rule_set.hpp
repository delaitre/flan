
#pragma once

#include <flan/matching_rule.hpp>
#include <memory>
#include <vector>

namespace flan
{
enum class node_type_t
{
    base,
    group,
    rule,
};

struct base_node_t;
using base_node_uniq_t = std::unique_ptr<base_node_t>;
using node_list_t = std::vector<base_node_uniq_t>;

class base_node_t
{
protected:
    explicit base_node_t(node_type_t type)
        : _type{type}
    {
    }

public:
    explicit base_node_t()
        : _type{node_type_t::base}
    {
    }

    base_node_t(const base_node_t&) = delete;
    base_node_t& operator=(const base_node_t&) = delete;

    virtual ~base_node_t() = default;

    base_node_t& add_child(base_node_uniq_t child)
    {
        assert(child);
        assert(!child->_parent);

        child->_parent = this;
        auto raw = child.get();
        _children.push_back(std::move(child));
        return *raw;
    }

    base_node_uniq_t remove_child(base_node_t& child)
    {
        assert(child._parent == this);
        auto it = find(child);
        assert(it != _children.end());

        base_node_uniq_t c;
        c.swap(*it);
        _children.erase(it);
        c->_parent = nullptr;

        return c;
    }

    base_node_t& child(int index)
    {
        assert(index >= 0);
        assert(index < child_count());

        return *_children[index];
    }

    const base_node_t& child(int index) const
    {
        assert(index >= 0);
        assert(index < child_count());

        return *_children[index];
    }

    int child_count() const { return _children.size(); }

    int index() const
    {
        if (_parent)
        {
            auto it = _parent->find(*this);
            assert(it != _parent->_children.end());
            return std::distance(_parent->_children.cbegin(), it);
        }

        return 0;
    }

    base_node_t* parent() const { return _parent; }

    node_type_t type() const { return _type; }

    template <typename PreFunctor, typename PostFunctor>
    void visit(PreFunctor pre_visitor, PostFunctor post_visitor) const
    {
        pre_visitor(*this);
        std::for_each(_children.begin(), _children.end(), [&](auto& child) {
            child->visit(pre_visitor, post_visitor);
        });
        post_visitor(*this);
    }

private:
    node_list_t::const_iterator find(const base_node_t& child) const
    {
        return std::find_if(
            _children.cbegin(), _children.cend(), [&child](auto& c) { return &child == c.get(); });
    }

    node_list_t::iterator find(base_node_t& child)
    {
        return std::find_if(
            _children.begin(), _children.end(), [&child](auto& c) { return &child == c.get(); });
    }

private:
    //! The parent of this node or nullptr if no parent.
    base_node_t* _parent = nullptr;

    //! Children
    node_list_t _children;

    //! The type of this node.
    node_type_t _type;
};

class group_node_t : public base_node_t
{
public:
    explicit group_node_t(QString name)
        : base_node_t{node_type_t::group}
        , _name{name}
    {
    }

    QString name() const { return _name; }
    void set_name(QString name) { _name = name; }

private:
    //! Name of the group
    QString _name;
};

class rule_node_t : public base_node_t
{
public:
    explicit rule_node_t(matching_rule_t rule)
        : base_node_t{node_type_t::rule}
        , _rule{std::move(rule)}
    {
    }

    const matching_rule_t& rule() const { return _rule; }
    void set_rule(matching_rule_t rule) { _rule = std::move(rule); }

private:
    //! The rule of the node.
    matching_rule_t _rule;
};
} // namespace flan
