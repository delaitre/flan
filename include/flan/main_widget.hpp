
#pragma once

#include <flan/matching_rule.hpp>
#include <QWidget>

namespace flan
{
class rule_model_t;
class rule_tree_widget_t;
class log_widget_t;

class main_widget_t : public QWidget
{
    Q_OBJECT

public:
    main_widget_t(QWidget* parent = nullptr);

public slots:

    void set_content(const QString& text);
    void set_model(rule_model_t* model);

private:
    rule_tree_widget_t* _rules = nullptr;
    log_widget_t* _log = nullptr;
};
} // namespace flan
