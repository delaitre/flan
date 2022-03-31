
#pragma once

#include <flan/style.hpp>
#include <QDialog>

class QComboBox;
class QLabel;

namespace flan
{
class color_button_t;

class style_editor_dialog_t : public QDialog
{
    Q_OBJECT

public:
    style_editor_dialog_t(const matching_style_t& initial_style, QWidget* parent = nullptr);

    static std::optional<matching_style_t> get_style(
        const matching_style_t& initial_style,
        QWidget* parent = nullptr,
        const QString& title = {});

    const matching_style_t& style() const { return _style; }

private:
    matching_style_t _style;

    color_button_t* _foreground_button = nullptr;
    color_button_t* _background_button = nullptr;
    QComboBox* _font_style_combo = nullptr;
    QComboBox* _font_weight_combo = nullptr;
    QLabel* _preview = nullptr;
};
} // namespace flan
