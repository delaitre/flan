
#include <flan/data_source.hpp>
#include <flan/find_controller.hpp>
#include <flan/find_widget.hpp>
#include <flan/log_margin_area_widget.hpp>
#include <flan/log_widget.hpp>
#include <flan/main_widget.hpp>
#include <flan/rule_model.hpp>
#include <flan/rule_set.hpp>
#include <flan/rule_tree_widget.hpp>
#include <QAction>
#include <QPushButton>
#include <QSplitter>
#include <QVBoxLayout>

namespace flan
{
namespace
{
styled_matching_rule_list_t get_styled_rules_from_node(const base_node_t* node)
{
    if (!node)
        return {};

    styled_matching_rule_list_t rules;

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
                rules.push_back(
                    {static_cast<const rule_node_t&>(node).rule(), node.computed_styles()});
                break;
            }
        });

    return rules;
}
} // namespace

main_widget_t::main_widget_t(QWidget* parent)
    : QWidget{parent}
    , _data_source{new data_source_selection_widget_t}
    , _rules{new rule_tree_widget_t}
    , _log{new log_widget_t}
    , _log_margin{new log_margin_area_widget_t{_log}}
    , _find_controller{new find_controller_t{_log, this}}
    , _find{new find_widget_t{_find_controller}}
{
    connect(
        _data_source,
        &data_source_selection_widget_t::current_data_source_changed,
        this,
        &main_widget_t::on_current_data_source_changed);

    auto data_source_and_rules_separator = new QFrame;
    data_source_and_rules_separator->setFrameShape(QFrame::HLine);
    data_source_and_rules_separator->setFrameShadow(QFrame::Sunken);
    data_source_and_rules_separator->setFixedWidth(100);

    auto left_layout = new QVBoxLayout;
    left_layout->setContentsMargins(0, 0, 2, 0);
    left_layout->addWidget(_rules);

    auto left_widget = new QWidget;
    left_widget->setLayout(left_layout);

    auto clear_button = new QPushButton{tr("Clear")};
    connect(clear_button, &QPushButton::clicked, _log, &log_widget_t::clear);

    auto bottom_layout = new QHBoxLayout;
    bottom_layout->setContentsMargins(0, 0, 0, 0);
    bottom_layout->addWidget(_data_source, 1);
    bottom_layout->addWidget(clear_button);

    _find->setVisible(false);

    auto right_layout = new QVBoxLayout;
    right_layout->setContentsMargins(2, 0, 0, 0);
    right_layout->addWidget(_find);
    right_layout->addWidget(_log);
    right_layout->addLayout(bottom_layout);

    auto right_widget = new QWidget;
    right_widget->setLayout(right_layout);

    auto splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(left_widget);
    splitter->setStretchFactor(0, 0);
    splitter->addWidget(right_widget);
    splitter->setStretchFactor(1, 1);

    auto main_layout = new QVBoxLayout;
    main_layout->addWidget(splitter);
    setLayout(main_layout);

    addAction(_find_controller->find_action());
    addAction(_find_controller->next_action());
    addAction(_find_controller->previous_action());

    connect(_find_controller->find_action(), &QAction::triggered, this, [this]() {
        _find->setVisible(true);
    });

    auto escape_action = new QAction{this};
    escape_action->setShortcut(Qt::Key_Escape);
    addAction(escape_action);
    connect(escape_action, &QAction::triggered, this, [this]() { _find->setVisible(false); });
}

void main_widget_t::set_data_sources(
    data_source_selection_widget_t::data_source_delegate_list_t data_source_list)
{
    _data_source->set_data_sources(std::move(data_source_list));
}

const timestamp_format_list_t& main_widget_t::timestamp_formats() const
{
    return _log_margin->timestamp_formats();
}

void main_widget_t::set_timestamp_formats(timestamp_format_list_t formats)
{
    _log_margin->set_timestamp_formats(std::move(formats));
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
            _log->set_rules(get_styled_rules_from_node(model->root()));
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

void main_widget_t::on_current_data_source_changed(data_source_t* data_source)
{
    if (_current_data_source)
        disconnect(_current_data_source, nullptr, _log, nullptr);

    _current_data_source = data_source;
    if (_current_data_source)
    {
        connect(_current_data_source, &data_source_t::new_text, _log, [this](auto text) {
            _log->append_text(text);
        });
        _log->clear();
        _log->append_text(_current_data_source->text());
    }
}
} // namespace flan
