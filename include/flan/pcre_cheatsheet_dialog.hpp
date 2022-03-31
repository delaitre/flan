
#pragma once

#include <QDialog>

namespace flan
{
class pcre_cheatsheet_dialog_t : public QDialog
{
    Q_OBJECT

public:
    pcre_cheatsheet_dialog_t(QWidget* parent = nullptr);
};
} // namespace flan
