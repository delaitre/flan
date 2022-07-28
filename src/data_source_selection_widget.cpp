
#include <flan/data_source_delegate.hpp>
#include <flan/data_source_selection_widget.hpp>
#include <flan/elided_label.hpp>
#include <QAction>
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace flan
{
data_source_selection_widget_t::data_source_selection_widget_t(QWidget* parent)
    : QWidget{parent}
    , _data_source_combobox{new QComboBox}
    , _configuration_button{new QToolButton}
    , _info_label{new elided_label_t}
    , _error_label{new elided_label_t}
{
    auto first_row_layout = new QHBoxLayout;
    first_row_layout->addWidget(_data_source_combobox);
    first_row_layout->addWidget(_configuration_button);

    auto main_layout = new QVBoxLayout;
    main_layout->addLayout(first_row_layout);
    main_layout->addWidget(_info_label);
    main_layout->addWidget(_error_label);
    setLayout(main_layout);

    auto info_label_font = _info_label->font();
    info_label_font.setItalic(true);
    _info_label->setFont(info_label_font);

    auto error_label_font = _error_label->font();
    error_label_font.setItalic(true);
    _error_label->setFont(error_label_font);
    _error_label->setStyleSheet("color: red;");

    auto configuration_action = new QAction{
        QIcon{":/icons/settings"}, tr("Configure the data source"), _configuration_button};
    _configuration_button->setDefaultAction(configuration_action);
    _configuration_button->setAutoRaise(true);

    connect(
        _configuration_button,
        &QToolButton::triggered,
        this,
        &data_source_selection_widget_t::show_configuration_dialog);

    connect(
        _data_source_combobox,
        &QComboBox::currentIndexChanged,
        this,
        &data_source_selection_widget_t::update_info_and_settings);

    update_info_and_settings();
}

void data_source_selection_widget_t::set_data_sources(data_source_delegate_list_t delegates)
{
    if (_delegates == delegates)
    {
        for (auto delegate: _delegates)
            disconnect(delegate, nullptr, this, nullptr);
        return;
    }

    _delegates = delegates;

    _data_source_combobox->clear();
    for (auto delegate: _delegates)
    {
        _data_source_combobox->addItem(delegate->name());
        connect(
            delegate,
            &data_source_delegate_t::info_changed,
            this,
            &data_source_selection_widget_t::update_info_and_settings);

        connect(
            delegate,
            &data_source_delegate_t::error_changed,
            this,
            &data_source_selection_widget_t::update_info_and_settings);
    }

    if (_delegates.empty())
    {
        _data_source_combobox->setEnabled(false);
        _data_source_combobox->setCurrentIndex(-1);
    }
    else
    {
        _data_source_combobox->setCurrentIndex(0);
        _data_source_combobox->setEnabled(true);
    }

    update_info_and_settings();
}

data_source_delegate_t* data_source_selection_widget_t::current_delegate() const
{
    if (_data_source_combobox->currentIndex() < _delegates.size())
        return _delegates[_data_source_combobox->currentIndex()];

    return nullptr;
}

void data_source_selection_widget_t::show_configuration_dialog()
{
    if (auto delegate = current_delegate(); delegate->is_settings_supported())
    {
        auto dialog = delegate->settings_dialog(this);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        dialog->open();
    }
}

void data_source_selection_widget_t::update_info_and_settings()
{
    data_source_t* current_data_source = nullptr;
    QString info_text;
    QString error_text;
    bool is_configuration_supported = false;

    if (auto delegate = current_delegate())
    {
        current_data_source = &delegate->data_source();
        info_text = delegate->info();
        error_text = delegate->error_message();
        is_configuration_supported = delegate->is_settings_supported();
    }

    _info_label->set_text(info_text);
    _info_label->setToolTip(info_text);
    _error_label->set_text(error_text);
    _error_label->setToolTip(error_text);
    _configuration_button->setEnabled(is_configuration_supported);

    emit current_data_source_changed(current_data_source);
}
} // namespace flan
