
#include <flan/log_widget.hpp>
#include <flan/main_widget.hpp>
#include <flan/rule_model.hpp>
#include <flan/rule_tree_widget.hpp>
#include <QSplitter>
#include <QVBoxLayout>

namespace flan
{
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
            _log->set_rules(model->rules());
        });
    }

    _rules->set_model(model);
    if (model)
        _log->set_rules(model->rules());
    else
        _log->set_rules({});
}
} // namespace flan
