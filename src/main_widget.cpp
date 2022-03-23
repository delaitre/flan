
#include <flan/log_widget.hpp>
#include <flan/main_widget.hpp>
#include <flan/rule_model.hpp>
#include <flan/rule_set.hpp>
#include <flan/rule_tree_widget.hpp>
#include <QSplitter>
#include <QVBoxLayout>

namespace flan
{
namespace
{
matching_rule_list_t get_rules_from_node(const base_node_t* node)
{
    if (!node)
        return {};

    matching_rule_list_t rules;

    // Apply the rules of the children first, then the parent.
    // This is because often parents have generic rules while children have more specific ones.
    node->visit(
        [](auto&) {},
        [&rules](const base_node_t& node) {
            switch (node.type())
            {
            case node_type_t::base:
                [[fallthrough]];
            case node_type_t::group:
                break;
            case node_type_t::rule:
                rules.push_back(static_cast<const rule_node_t&>(node).rule());
                break;
            }
        });

    return rules;
}
} // namespace

main_widget_t::main_widget_t(QWidget* parent)
    : QWidget{parent}
    , _rules{new rule_tree_widget_t}
    , _log{new log_widget_t}
{
    auto splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(_rules);
    splitter->addWidget(_log);

    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);

    auto main_layout = new QVBoxLayout;
    main_layout->addWidget(splitter);
    setLayout(main_layout);
}

void main_widget_t::set_content(const QString& text)
{
    _log->setPlainText(text);
}

void main_widget_t::set_model(rule_model_t* model)
{
    if (_rules->model())
        disconnect(_rules->model(), nullptr, this, nullptr);

    auto update_rules = [this, model]() {
        if (model)
            _log->set_rules(get_rules_from_node(model->root()));
        else
            _log->set_rules({});
    };

    if (model)
    {
        connect(model, &rule_model_t::dataChanged, this, update_rules);
        connect(model, &rule_model_t::layoutChanged, this, update_rules);
        connect(model, &rule_model_t::modelReset, this, update_rules);
        connect(model, &rule_model_t::rowsInserted, this, update_rules);
        connect(model, &rule_model_t::rowsMoved, this, update_rules);
        connect(model, &rule_model_t::rowsRemoved, this, update_rules);
    }

    _rules->set_model(model);
    update_rules();
}

void main_widget_t::set_highlighting_style(matching_style_list_t styles)
{
    _log->set_highlighting_style(std::move(styles));
}
} // namespace flan
