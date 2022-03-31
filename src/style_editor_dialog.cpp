
#include <flan/color_button.hpp>
#include <flan/style_editor_dialog.hpp>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>

namespace flan
{
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
    // FIXME: set current value
    _font_style_combo->addItems(QStringList() << tr("normal") << tr("italic"));
    _font_weight_combo = new QComboBox;
    // FIXME: set current value
    _font_weight_combo->addItems(QStringList() << tr("normal") << tr("bold"));
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

        // FIXME
        //        switch (font_style_combo->currentIndex())
        //        {
        //        case 0:
        //            _style.font_style = "normal";
        //            break;
        //        case 1:
        //            _style.font_style = "italic";
        //            break;
        //        default:
        //            break;
        //        }

        //        switch (font_weight_combo->currentIndex())
        //        {
        //        case 0:
        //            _style.font_weight = "normal";
        //            break;
        //        case 1:
        //            _style.font_weight = "bold";
        //            break;
        //        default:
        //            break;
        //        }
    };

    auto update_preview = [this]() {
        _preview->setStyleSheet(
            QString("QLabel { %1 %2 %3 %4 }")
                .arg(
                    _style.foreground_color.isValid() ?
                        QString("color: %1;").arg(_style.foreground_color.name(QColor::HexArgb)) :
                        "")
                .arg(
                    _style.background_color.isValid() ?
                        QString("background-color: %1;")
                            .arg(_style.background_color.name(QColor::HexArgb)) :
                        "")
                .arg("")
                .arg(""));
        //                .arg(QString("font-style: %1;").arg(_style.font_style == 0 ? "normal" :
        //                "italic")) .arg(QString("font-weight: %1").arg(_style.font_weight == 0 ?
        //                "normal" : "bold")));
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
