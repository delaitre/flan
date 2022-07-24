
#include <flan/log_widget.hpp>
#include <flan/rule_highlighter.hpp>
#include <QFont>
#include <QToolTip>

namespace flan
{
log_widget_t::log_widget_t(const QString& text, QWidget* parent)
    : QPlainTextEdit{text, parent}
    , _highlighter{new rule_highlighter_t{document()}}
{
    QFont font;
    font.setFamily("monospace");
    font.setFixedPitch(true);
    font.setPointSize(10);
    setFont(font);

    connect(this, &QPlainTextEdit::textChanged, this, &log_widget_t::apply_rules);

    setMouseTracking(true);
}

void log_widget_t::set_rules(styled_matching_rule_list_t rules)
{
    _rules = std::move(rules);
    _highlighter->set_rules(_rules);
}

void log_widget_t::mouseMoveEvent(QMouseEvent* event)
{
    // If buttons are pressed, use the base class implementation.
    if (event->buttons() != Qt::NoButton)
    {
        QPlainTextEdit::mouseMoveEvent(event);
        return;
    }

    // If no button are pressed, show a tooltip for the match under the mouse pointer.
    QPoint local_position;
    QPoint global_position;
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    local_position = event->position().toPoint();
    global_position = event->globalPosition().toPoint();
#else
    local_position = event->pos();
    global_position = event->globalPos();
#endif

    auto tooltip_text = tooltip_at(local_position);
    QToolTip::showText(global_position, tooltip_text, this);

    event->accept();
}

QString log_widget_t::tooltip_at(QPoint position)
{
    QTextCursor cursor = cursorForPosition(position);

    int position_in_block = cursor.positionInBlock();
    QString text = cursor.block().text();
    QString tooltip_text; // An empty text will hide the tooltip.

    // This first rule matching has higher priority and is used for the tooltip.
    for (const auto& styled_rule: _rules)
    {
        if (!styled_rule.rule.highlight_match)
            continue;

        QRegularExpressionMatchIterator it = styled_rule.rule.rule.globalMatch(text);
        while (it.hasNext() && tooltip_text.isEmpty())
        {
            QRegularExpressionMatch match = it.next();

            int start_index = (match.lastCapturedIndex() == 0) ? 0 : 1;
            for (int i = start_index; (i <= match.lastCapturedIndex()) && tooltip_text.isEmpty();
                 ++i)
            {
                if (match.capturedStart(i) <= position_in_block
                    && position_in_block < match.capturedEnd(i))
                {
                    // Use the rule tooltip if non empty, otherwise default to the rule name.
                    tooltip_text = styled_rule.rule.tooltip.isEmpty() ? styled_rule.rule.name :
                                                                        styled_rule.rule.tooltip;
                }
            }
        }

        if (!tooltip_text.isEmpty())
            break;
    }

    return tooltip_text;
}

void log_widget_t::apply_rules()
{
    auto doc = document();

    for (auto block = doc->begin(); block.isValid(); block = block.next())
    {
        // Iterate over the rules in order and determine if the block should be visible or not.
        for (auto& styled_rule: _rules)
        {
            bool has_matched = false;

            switch (styled_rule.rule.behaviour)
            {
            case filtering_behaviour_t::none:
                // Don't change the block visibility.
                break;
            case filtering_behaviour_t::remove_line:
                // Mark the block as not visible if the rule matches.
                if (styled_rule.rule.rule.globalMatch(block.text()).hasNext())
                {
                    block.setVisible(false);
                    has_matched = true;
                }
                break;
            case filtering_behaviour_t::keep_line:
                // Mark the block as visible if the rule matches.
                if (styled_rule.rule.rule.globalMatch(block.text()).hasNext())
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
