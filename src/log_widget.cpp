
#include <flan/log_widget.hpp>
#include <flan/rule_highlighter.hpp>
#include <QFont>
#include <QPlainTextEdit>
#include <QVBoxLayout>

namespace flan
{
log_widget_t::log_widget_t(QWidget* parent)
    : QWidget{parent}
    , _text_edit{new QPlainTextEdit}
    , _highlighter{new rule_highlighter_t{_text_edit->document()}}
{
    QFont font;
    font.setFamily("monospace");
    font.setFixedPitch(true);
    font.setPointSize(10);
    _text_edit->setFont(font);

    auto main_layout = new QVBoxLayout;
    main_layout->addWidget(_text_edit);
    setLayout(main_layout);

    connect(_text_edit, &QPlainTextEdit::textChanged, this, &log_widget_t::apply_rules);
}

void log_widget_t::set_content(const QString& text)
{
    _text_edit->setPlainText(text);
}

void log_widget_t::append_content(const QString& text)
{
    _text_edit->appendPlainText(text);
}

void log_widget_t::set_rules(QVector<matching_rule_t> rules)
{
    _rules = std::move(rules);
    _highlighter->set_rules(_rules);
}

void log_widget_t::apply_rules()
{
    auto doc = _text_edit->document();

    for (auto block = doc->begin(); block.isValid(); block = block.next())
    {
        // Iterate over the rules in order and determine if the block should be visible or not.
        for (auto& rule: _rules)
        {
            bool has_matched = false;

            switch (rule.behaviour)
            {
            case filtering_behaviour_t::none:
                // Don't change the block visibility.
                break;
            case filtering_behaviour_t::remove_line:
                // Mark the block as not visible if the rule matches.
                if (rule.rule.globalMatch(block.text()).hasNext())
                {
                    block.setVisible(false);
                    has_matched = true;
                }
                break;
            case filtering_behaviour_t::keep_line:
                // Mark the block as visible if the rule matches.
                if (rule.rule.globalMatch(block.text()).hasNext())
                {
                    block.setVisible(true);
                    has_matched = true;
                }
                break;
            }

            if (has_matched)
            {
                // A matching rule was found, so don't check remaining rules.
                break;
            }
            else
            {
                // No rule matched, so ensure the block is visible (which might not be the case if
                // it was removed by a rule which is now not applying because unchecked or the block
                // content changed.
                block.setVisible(true);
            }
        }
    }
}
} // namespace flan
