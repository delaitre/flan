
#pragma once

#include <QWidget>

namespace flan
{
class rule_model_t;
class rule_editor_t;
class tree_view_t;

class rule_tree_widget_t : public QWidget
{
    Q_OBJECT

public:
    rule_tree_widget_t(QWidget* parent = nullptr);

    rule_model_t* model() const;
    void set_model(rule_model_t* model);

private:
    tree_view_t* _tree = nullptr;
    rule_editor_t* _rule_editor = nullptr;
};
} // namespace flan
