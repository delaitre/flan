
#pragma once

#include <QDataWidgetMapper>
#include <QWidget>

class QLineEdit;
class QDialog;

namespace flan
{
class rule_editor_t : public QWidget
{
    Q_OBJECT

public:
    rule_editor_t(QWidget* parent = nullptr);

    void set_model(QAbstractItemModel* model);

public slots:
    void set_current_model_index(const QModelIndex& index);

private slots:
    void update_pattern_validity();
    void show_pcre_cheatsheet_dialog();

private:
    void update_pattern_palette(bool is_pattern_valid);

private:
    QLineEdit* _name_edit = nullptr;
    QLineEdit* _pattern_edit = nullptr;
    QLineEdit* _tooltip_edit = nullptr;
    QDialog* _pcre_cheatsheet_dialog = nullptr;

    QDataWidgetMapper _mapper;
};
} // namespace flan
