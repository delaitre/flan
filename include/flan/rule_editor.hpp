
#pragma once

#include <QDataWidgetMapper>
#include <QWidget>

class QLineEdit;
class QDialog;

namespace flan
{
class style_list_widget_t;
class validated_lineedit_t;

class rule_editor_t : public QWidget
{
    Q_OBJECT

public:
    rule_editor_t(QWidget* parent = nullptr);

    void set_model(QAbstractItemModel* model);

public slots:
    void set_current_model_index(const QModelIndex& index);

private slots:
    void show_pcre_cheatsheet_dialog();

private:
    QLineEdit* _name_edit = nullptr;
    validated_lineedit_t* _pattern_edit = nullptr;
    QLineEdit* _tooltip_edit = nullptr;
    style_list_widget_t* _style_list = nullptr;
    QDialog* _pcre_cheatsheet_dialog = nullptr;

    QDataWidgetMapper _mapper;
};
} // namespace flan
