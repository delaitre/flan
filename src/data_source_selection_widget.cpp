
#include <flan/data_source.hpp>
#include <flan/data_source_delegate.hpp>
#include <flan/data_source_selection_widget.hpp>
#include <flan/elided_label.hpp>
#include <QAction>

namespace flan
{
data_source_selection_widget_t::data_source_selection_widget_t(QWidget* parent)
    : QWidget{parent}
    , _data_source_combobox{new QComboBox}
    , _current_source_widget_layout{new QHBoxLayout}
    , _error_label{new elided_label_t}
{
    auto main_layout = new QHBoxLayout;
    main_layout->setContentsMargins(0, 0, 0, 0);
    main_layout->addWidget(_data_source_combobox);
    main_layout->addLayout(_current_source_widget_layout);
    main_layout->addWidget(_error_label);
    main_layout->addStretch();
    setLayout(main_layout);

    // Just use this layout act as a container for the custom widget (which might be a set of
    // widgets in their own layout and so on) so ensure we don't put any margins by default.
    _current_source_widget_layout->setContentsMargins(0, 0, 0, 0);

    auto error_label_font = _error_label->font();
    error_label_font.setItalic(true);
    _error_label->setFont(error_label_font);
    _error_label->setStyleSheet("color: red;");

    connect(
        _data_source_combobox,
        &QComboBox::currentIndexChanged,
        this,
        &data_source_selection_widget_t::rebuild_custom_widgets);

    set_data_sources({});
}

void data_source_selection_widget_t::set_data_sources(data_source_delegate_list_t delegates)
{
    if (_delegates == delegates)
        return;

    for (auto delegate: _delegates)
        disconnect(delegate, nullptr, this, nullptr);

    _delegates = delegates;

    _data_source_combobox->clear();
    for (auto delegate: _delegates)
    {
        auto& data_source = delegate->data_source();
        _data_source_combobox->addItem(data_source.name());

        connect(
            &data_source,
            &data_source_t::error_changed,
            this,
            &data_source_selection_widget_t::update_error);
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
}

data_source_delegate_t* data_source_selection_widget_t::current_delegate() const
{
    if (_data_source_combobox->currentIndex() < _delegates.size())
        return _delegates[_data_source_combobox->currentIndex()];

    return nullptr;
}

void data_source_selection_widget_t::rebuild_custom_widgets()
{
    // Remove any custom widgets from the previous data source
    while (auto item = _current_source_widget_layout->takeAt(0))
    {
        delete item->widget();
        delete item;
    }

    data_source_t* current_data_source = nullptr;
    if (auto delegate = current_delegate())
    {
        current_data_source = &delegate->data_source();

        // Add the custom widget if any from the new data source
        if (auto custom_widget = delegate->create_view())
            _current_source_widget_layout->addWidget(custom_widget);
    }

    update_error();
    emit current_data_source_changed(current_data_source);
}

void data_source_selection_widget_t::update_error()
{
    data_source_t* current_data_source = nullptr;
    QString error_text;

    if (auto delegate = current_delegate())
    {
        current_data_source = &delegate->data_source();
        error_text = current_data_source->error_message();
    }

    _error_label->set_text(error_text);
    _error_label->setToolTip(error_text);
    _error_label->setVisible(!error_text.isEmpty());
}
} // namespace flan
