
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
QVector<matching_rule_t> get_rules_from_node(const base_node_t* node)
{
    if (!node)
        return {};

    QVector<matching_rule_t> rules;

    node->visit([&rules](const base_node_t& node) {
        switch (node.type())
        {
        case node_type_t::base:
            [[fallthrough]];
        case node_type_t::group:
            break;
        case node_type_t::rule:
            rules.append(static_cast<const rule_node_t&>(node).rule());
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
    _log->set_content(text);
}

void main_widget_t::set_model(rule_model_t* model)
{
    if (_rules->model())
        disconnect(_rules->model(), nullptr, this, nullptr);

    if (model)
    {
        connect(model, &rule_model_t::dataChanged, this, [this, model]() {
            _log->set_rules(get_rules_from_node(model->root()));
        });
    }

    _rules->set_model(model);
    if (model)
        _log->set_rules(get_rules_from_node(model->root()));
    else
        _log->set_rules({});
}
} // namespace flan
