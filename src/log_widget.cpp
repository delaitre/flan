
#include <flan/log_widget.hpp>
#include <flan/rule_highlighter.hpp>
#include <QAction>
#include <QFont>
#include <QMimeData>
#include <QTextDocumentFragment>
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

void log_widget_t::append_text(const QString& text)
{
    if (_is_paused)
        return;

    const QTextCursor old_cursor = textCursor();
    const int old_scrollbar_value = verticalScrollBar()->value();
    const bool is_scrolled_down = (old_scrollbar_value == verticalScrollBar()->maximum());

    // Move the cursor to the end of the document and insert the text at the end.
    moveCursor(QTextCursor::End);
    textCursor().insertText(text);

    if (old_cursor.hasSelection() || !is_scrolled_down)
    {
        // The user has selected some text or scrolled away from the bottom so restore the previous
        // position so that when text is appended the user can still move to a different area of the
        // log.
        setTextCursor(old_cursor);
        verticalScrollBar()->setValue(old_scrollbar_value);
    }
    else
    {
        // The user hasn't selected any text and the scrollbar is at the bottom so scroll to the
        // bottom.
        moveCursor(QTextCursor::End);
        verticalScrollBar()->setValue(verticalScrollBar()->maximum());
    }
}

void log_widget_t::set_paused(bool is_paused)
{
    _is_paused = is_paused;
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

QMimeData* log_widget_t::createMimeDataFromSelection() const
{
    QMimeData* data = new QMimeData();
    data->setText(plain_text_with_rules_applied());
    return data;
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

        if (!styled_rule.rule.rule.isValid())
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
            if (!styled_rule.rule.rule.isValid())
                continue;

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

QString log_widget_t::plain_text_with_rules_applied() const
{
    if (document()->isEmpty())
        return {};

    // We create a new document and then iterate over the source document selection block by block
    // and copy over only the ones that are visible (the ones marked as invisible are not kept as it
    // means they have been filtered out by a matching rule).

    // Get the cursor positionned at the beginning of the selected content in the source document.
    QTextCursor src_cursor = textCursor();

    // If the cursor position is before the anchor, swap them to simplify iterating block by block
    // below (this means we can just use NextBlock/StartOfBlock all the time rather than changing
    // to PreviousBlock/EndOfBlock depending on the direction).
    auto original_position = src_cursor.position();
    auto original_anchor = src_cursor.anchor();
    bool is_direction_inverted = (original_position < original_anchor);
    if (is_direction_inverted)
        std::swap(original_position, original_anchor);

    // The new document which will contain only the selected and visible content.
    QTextDocument dst_document;

    // Position the cursor in the destination document at the very end.
    QTextCursor dst_cursor{&dst_document};
    dst_cursor.movePosition(QTextCursor::End);

    // Iterate over each block in the source document from the original anchor position.
    // We iterate up to the last block and then deal with the last block separately.
    src_cursor.setPosition(original_anchor, QTextCursor::MoveAnchor);
    auto last_block = document()->findBlock(original_position);
    while (src_cursor.block() != last_block)
    {
        // If the block is visible, copy it in the destination. Otherwise skip it.
        if (src_cursor.block().isVisible())
        {
            // First, select the whole block by moving the position to the start of the next
            // block but keeping the anchor at the beginning of the current one.
            src_cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);

            // Copy the now selected content into the destination.
            QTextDocumentFragment selection = src_cursor.selection();
            dst_cursor.insertFragment(selection);

            // Move the anchor position to the start of the next block to be processed (which is
            // the current block the source cursor is on due to the movePosition call earlier).
            // This moves the anchor to the current cursor position in the source so that it is
            // ready to select the next block at the next iteration.
            src_cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
        }
        else
        {
            // Skip the block by moving both the position and the anchor to the next block.
            src_cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor);
        }
    }

    // Process the last block.
    if (src_cursor.block().isVisible())
    {
        // Make sure for the last block, we copy only up to the last selected character rather
        // than the end of the block.
        src_cursor.setPosition(original_position, QTextCursor::KeepAnchor);

        // Copy the now selected content into the destination.
        QTextDocumentFragment selection = src_cursor.selection();
        dst_cursor.insertFragment(selection);
    }

    // Restore original anchor/position in source document.
    if (is_direction_inverted)
        std::swap(original_position, original_anchor);
    src_cursor.setPosition(original_anchor, QTextCursor::MoveAnchor);
    src_cursor.setPosition(original_position, QTextCursor::KeepAnchor);

    return dst_document.toPlainText();
}
} // namespace flan
