
#pragma once

#include <QLineEdit>

namespace flan
{
class validated_lineedit_t : public QLineEdit
{
    Q_OBJECT

public:
    validated_lineedit_t(QWidget* parent = nullptr);

    void set_validity_check_enabled(bool is_enabled);

private slots:
    void update_validity();

private:
    bool _is_validity_check_enabled = true;
};
} // namespace flan
