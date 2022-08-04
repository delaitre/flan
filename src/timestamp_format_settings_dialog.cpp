
#include <flan/timestamp_format_settings_dialog.hpp>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QTime>
#include <QToolButton>
#include <QVBoxLayout>
#include <chrono>

// Q_INIT_RESOURCE has to be called from the global namespace, hence this workaround.
inline void init_resource()
{
    Q_INIT_RESOURCE(libflan);
}

namespace flan
{
namespace
{
enum format_item_data_role_t
{
    format = Qt::UserRole,
};
} // namespace

timestamp_format_settings_dialog_t::component_capture_widgets_t::component_capture_widgets_t(
    QString component_name,
    QWidget* parent)
    : QHBoxLayout{parent}
    , captured_index_spinbox{new QSpinBox}
    , preview_label{new QLabel}
{
    captured_index_spinbox->setToolTip(
        tr("Captured index corresponding to the %1").arg(component_name));
    captured_index_spinbox->setSpecialValueText(tr("Not applicable"));
    preview_label = new QLabel;
    preview_label->setToolTip(tr("Captured content from the test string"));

    auto preview_font = preview_label->font();
    preview_font.setItalic(true);
    preview_label->setFont(preview_font);

    addWidget(captured_index_spinbox);
    addWidget(preview_label);
}

void timestamp_format_settings_dialog_t::component_capture_widgets_t::update(
    bool is_enabled,
    int capture_count,
    int captured_index,
    QString match)
{
    captured_index_spinbox->setEnabled(is_enabled);
    captured_index_spinbox->setRange(-1, capture_count);
    captured_index_spinbox->setValue(captured_index);
    preview_label->setText(match);
}

timestamp_format_settings_dialog_t::timestamp_format_settings_dialog_t(
    QString test_string,
    QWidget* parent)
    : QDialog{parent}
{
    init_resource();

    setWindowTitle(tr("Edit timestamp formats"));

    auto formats_groupbox = new QGroupBox{tr("Timestamp formats")};
    _format_list = new QListWidget;
    _format_list->setToolTip(
        tr("List of all supported formats to extract timestamps in order of priority."));
    _format_list->setSelectionMode(QAbstractItemView::SingleSelection);
    _format_list->setDragEnabled(true);
    _format_list->viewport()->setAcceptDrops(true);
    _format_list->setDropIndicatorShown(true);
    _format_list->setDragDropMode(QAbstractItemView::InternalMove);

    _add_format_button = new QToolButton;
    _add_format_button->setAutoRaise(true);
    _add_format_action = new QAction{QIcon{":/icons/plus"}, tr("Add format"), _add_format_button};
    _add_format_button->setDefaultAction(_add_format_action);
    _remove_format_button = new QToolButton;
    _remove_format_button->setAutoRaise(true);
    _remove_format_action =
        new QAction{QIcon{":/icons/minus"}, tr("Remove format"), _remove_format_button};
    _remove_format_button->setDefaultAction(_remove_format_action);
    auto add_remove_layout = new QVBoxLayout;
    add_remove_layout->addWidget(_add_format_button);
    add_remove_layout->addWidget(_remove_format_button);
    add_remove_layout->addStretch();

    auto formats_layout = new QHBoxLayout;
    formats_layout->addWidget(_format_list);
    formats_layout->addLayout(add_remove_layout);
    formats_groupbox->setLayout(formats_layout);

    auto current_groupbox = new QGroupBox{tr("Current format")};
    _name_lineedit = new QLineEdit;
    _name_lineedit->setToolTip(tr("Name"));
    _pattern_lineedit = new QLineEdit;
    _pattern_lineedit->setToolTip(tr("Pattern used to extract individual timestamp information"));
    _hour_widgets = new component_capture_widgets_t{tr("hours")};
    _minute_widgets = new component_capture_widgets_t{tr("minutes")};
    _second_widgets = new component_capture_widgets_t{tr("seconds")};
    _millisecond_widgets = new component_capture_widgets_t{tr("milliseconds")};
    auto current_layout = new QFormLayout;
    current_layout->addRow(tr("Name"), _name_lineedit);
    current_layout->addRow(tr("Pattern"), _pattern_lineedit);
    current_layout->addRow(tr("Hour"), _hour_widgets);
    current_layout->addRow(tr("Minute"), _minute_widgets);
    current_layout->addRow(tr("Second"), _second_widgets);
    current_layout->addRow(tr("Millisecond"), _millisecond_widgets);
    current_groupbox->setLayout(current_layout);

    auto preview_groupbox = new QGroupBox{tr("Preview")};
    _test_string_lineedit = new QLineEdit{test_string};
    _test_string_lineedit->setToolTip(tr("Test string to verify the entered format"));
    _preview_label = new QLabel;
    _preview_label->setToolTip(tr("Preview of the extracted and formatted timestamp"));
    auto preview_layout = new QFormLayout;
    preview_layout->addRow(tr("Test string"), _test_string_lineedit);
    preview_layout->addRow(tr("Preview"), _preview_label);
    preview_groupbox->setLayout(preview_layout);

    auto button_box = new QDialogButtonBox{QDialogButtonBox::Ok | QDialogButtonBox::Cancel};
    connect(button_box, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto main_layout = new QGridLayout;
    main_layout->addWidget(formats_groupbox, 0, 0, 2, 1);
    main_layout->addWidget(current_groupbox, 0, 1, 1, 1);
    main_layout->addWidget(preview_groupbox, 1, 1, 1, 1);
    main_layout->addWidget(button_box, 2, 0, 1, 2);

    setLayout(main_layout);

    connect(
        _format_list,
        &QListWidget::currentItemChanged,
        this,
        &timestamp_format_settings_dialog_t::update_widgets_from_current_format);
    connect(
        _add_format_action,
        &QAction::triggered,
        this,
        qOverload<>(&timestamp_format_settings_dialog_t::add_format));
    connect(
        _remove_format_action,
        &QAction::triggered,
        this,
        &timestamp_format_settings_dialog_t::remove_format);
    connect(
        _name_lineedit,
        &QLineEdit::textChanged,
        this,
        &timestamp_format_settings_dialog_t::update_current_format_from_widgets);
    connect(
        _pattern_lineedit,
        &QLineEdit::textChanged,
        this,
        &timestamp_format_settings_dialog_t::update_current_format_from_widgets);
    connect(
        _hour_widgets->captured_index_spinbox,
        &QSpinBox::valueChanged,
        this,
        &timestamp_format_settings_dialog_t::update_current_format_from_widgets);
    connect(
        _minute_widgets->captured_index_spinbox,
        &QSpinBox::valueChanged,
        this,
        &timestamp_format_settings_dialog_t::update_current_format_from_widgets);
    connect(
        _second_widgets->captured_index_spinbox,
        &QSpinBox::valueChanged,
        this,
        &timestamp_format_settings_dialog_t::update_current_format_from_widgets);
    connect(
        _millisecond_widgets->captured_index_spinbox,
        &QSpinBox::valueChanged,
        this,
        &timestamp_format_settings_dialog_t::update_current_format_from_widgets);
    connect(
        _test_string_lineedit,
        &QLineEdit::textChanged,
        this,
        &timestamp_format_settings_dialog_t::update_widgets_from_current_format);

    update_widgets_from_current_format();
}

timestamp_format_list_t timestamp_format_settings_dialog_t::formats() const
{
    timestamp_format_list_t list;

    for (auto i = 0; i < _format_list->count(); ++i)
        list.push_back(*format(i));

    return list;
}

void timestamp_format_settings_dialog_t::set_formats(const timestamp_format_list_t& formats)
{
    _format_list->clear();
    for (auto& format: formats)
        add_format(format);

    _format_list->setCurrentRow(0);
}

std::optional<timestamp_format_t> timestamp_format_settings_dialog_t::format(int i) const
{
    if (auto item = _format_list->item(i); item)
        return item->data(format_item_data_role_t::format).value<timestamp_format_t>();
    return {};
}

std::optional<timestamp_format_t> timestamp_format_settings_dialog_t::current_format() const
{
    return format(_format_list->currentRow());
}

void timestamp_format_settings_dialog_t::add_format()
{
    timestamp_format_t format;
    format.name = tr("New format");
    add_format(format);
}

void timestamp_format_settings_dialog_t::add_format(timestamp_format_t format)
{
    auto item = new QListWidgetItem{format.name, _format_list};
    item->setData(format_item_data_role_t::format, QVariant::fromValue(format));
    item->setFlags(
        Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);
    item->setCheckState(format.is_enabled ? Qt::Checked : Qt::Unchecked);
    _format_list->setCurrentItem(item);
}

void timestamp_format_settings_dialog_t::remove_format()
{
    delete _format_list->currentItem();
}

void timestamp_format_settings_dialog_t::update_widgets_from_current_format()
{
    auto format = current_format();

    _name_lineedit->setEnabled(format.has_value());
    _pattern_lineedit->setEnabled(format.has_value());

    _test_string_lineedit->setEnabled(format.has_value());
    _preview_label->setEnabled(format.has_value());
    QString combined_preview;

    if (format)
    {
        auto update_lineedit_and_restore_cursor = [](QLineEdit* lineedit, QString text) {
            auto position = lineedit->cursorPosition();
            lineedit->setText(text);
            lineedit->setCursorPosition(position);
        };

        update_lineedit_and_restore_cursor(_name_lineedit, format->name);
        update_lineedit_and_restore_cursor(_pattern_lineedit, format->regexp.pattern());

        QString hour_preview;
        QString minute_preview;
        QString second_preview;
        QString millisecond_preview;

        // The spinboxes use -1 for ignore/invalid which is also the value returned by
        // captureCount() if the regexp is invalid.
        QRegularExpression regexp{format->regexp};
        int capture_count = regexp.captureCount();

        if (regexp.isValid())
        {
            if (auto match = regexp.match(_test_string_lineedit->text()); match.hasMatch())
            {
                // If the capture index does not correspond to anything, a null string is returned.
                // If the conversion to int fails, 0 is returned.
                // So in all cases, we end up with 0 in case something goes wrong which is what we
                // need.
                hour_preview = match.captured(format->hour_index);
                auto hours = std::chrono::hours{hour_preview.toInt()};

                minute_preview = match.captured(format->minute_index);
                auto minutes = std::chrono::minutes{minute_preview.toInt()};

                second_preview = match.captured(format->second_index);
                auto seconds = std::chrono::seconds{second_preview.toInt()};

                millisecond_preview = match.captured(format->millisecond_index);
                auto milliseconds = std::chrono::milliseconds{millisecond_preview.toInt()};

                auto total = std::chrono::milliseconds{hours + minutes + seconds + milliseconds};
                auto time = QTime::fromMSecsSinceStartOfDay(total.count());

                combined_preview = time.toString(Qt::ISODateWithMs);
            }
        }

        _hour_widgets->update(true, capture_count, format->hour_index, hour_preview);
        _minute_widgets->update(true, capture_count, format->minute_index, minute_preview);
        _second_widgets->update(true, capture_count, format->second_index, second_preview);
        _millisecond_widgets->update(
            true, capture_count, format->millisecond_index, millisecond_preview);
    }
    else
    {
        _hour_widgets->update(false, -1, -1, {});
        _minute_widgets->update(false, -1, -1, {});
        _second_widgets->update(false, -1, -1, {});
        _millisecond_widgets->update(false, -1, -1, {});
    }

    auto preview_label_font = _preview_label->font();
    preview_label_font.setItalic(combined_preview.isNull());
    _preview_label->setFont(preview_label_font);
    _preview_label->setText(combined_preview.isNull() ? tr("no match") : combined_preview);
}

void timestamp_format_settings_dialog_t::update_current_format_from_widgets()
{
    if (auto item = _format_list->currentItem(); item)
    {
        auto format = timestamp_format_t{
            item->checkState() == Qt::Checked, // Don't change the current check state
            _name_lineedit->text(),
            QRegularExpression{_pattern_lineedit->text()},
            _hour_widgets->captured_index_spinbox->value(),
            _minute_widgets->captured_index_spinbox->value(),
            _second_widgets->captured_index_spinbox->value(),
            _millisecond_widgets->captured_index_spinbox->value()};

        item->setData(format_item_data_role_t::format, QVariant::fromValue(format));
        item->setText(format.name);
    }

    update_widgets_from_current_format();
}
} // namespace flan
