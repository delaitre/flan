
#include <flan/color_button.hpp>
#include <flan/style_editor_dialog.hpp>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>

namespace flan
{
namespace
{
//! Cast an enum class value \a e to its underlying type.
template <typename Enum>
[[maybe_unused]] constexpr auto to_underlying(Enum e) noexcept
{
    return static_cast<std::underlying_type_t<Enum>>(e);
}

constexpr int to_combo_box_index(font_style_t font_style)
{
    return to_underlying(font_style);
}

constexpr int to_combo_box_index(font_weight_t font_weight)
{
    return to_underlying(font_weight);
}
} // namespace

style_editor_dialog_t::style_editor_dialog_t(const matching_style_t& initial_style, QWidget* parent)
    : QDialog{parent}
    , _style{initial_style}
{
    setWindowTitle(tr("Edit style"));

    _foreground_button = new color_button_t;
    _foreground_button->set_color(_style.foreground_color);
    _background_button = new color_button_t;
    _background_button->set_color(_style.background_color);
    _font_style_combo = new QComboBox;
    _font_style_combo->addItem(tr("normal"), QVariant::fromValue(font_style_t::normal));
    _font_style_combo->addItem(tr("italic"), QVariant::fromValue(font_style_t::italic));
    _font_style_combo->setCurrentIndex(to_combo_box_index(_style.font_style));
    _font_weight_combo = new QComboBox;
    _font_weight_combo->addItem(tr("normal"), QVariant::fromValue(font_weight_t::normal));
    _font_weight_combo->addItem(tr("bold"), QVariant::fromValue(font_weight_t::bold));
    _font_weight_combo->setCurrentIndex(to_combo_box_index(_style.font_weight));
    _preview = new QLabel(tr("This is what it will look like!"));

    auto button_box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(button_box, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto form_layout = new QFormLayout;
    form_layout->addRow(tr("Foreground"), _foreground_button);
    form_layout->addRow(tr("Background"), _background_button);
    form_layout->addRow(tr("Font style"), _font_style_combo);
    form_layout->addRow(tr("Font weight"), _font_weight_combo);
    form_layout->addRow(tr("Preview"), _preview);
    form_layout->addWidget(button_box);

    setLayout(form_layout);

    auto update_style = [this]() {
        _style.foreground_color = _foreground_button->color();
        _style.background_color = _background_button->color();
        _style.font_style = _font_style_combo->currentData().value<font_style_t>();
        _style.font_weight = _font_weight_combo->currentData().value<font_weight_t>();
    };

    auto update_preview = [this]() {
        _preview->setStyleSheet(QString("QLabel { %1 }").arg(to_css_string(_style)));
    };

    auto update = [=]() {
        update_style();
        update_preview();
    };

    connect(_foreground_button, &color_button_t::color_changed, this, update);
    connect(_background_button, &color_button_t::color_changed, this, update);
    connect(_font_style_combo, &QComboBox::currentIndexChanged, this, update);
    connect(_font_weight_combo, &QComboBox::currentIndexChanged, this, update);

    update_preview();
}

std::optional<matching_style_t> style_editor_dialog_t::get_style(
    const matching_style_t& initial_style,
    QWidget* parent,
    const QString& title)
{
    style_editor_dialog_t dialog{initial_style, parent};
    if (!title.isNull())
        dialog.setWindowTitle(title);
    dialog.setModal(true);

    if (dialog.exec() == QDialog::Accepted)
        return dialog.style();

    return std::nullopt;
}
} // namespace flan
