
#include <flan/find_controller.hpp>
#include <QRegularExpression>
#include <QTextBlock>
#include <QTextCursor>

namespace flan
{
find_controller_t::find_controller_t(QPlainTextEdit* text_edit, QObject* parent)
    : QObject{parent}
    , _text_edit{text_edit}
    , _find_action{new QAction{tr("Find"), this}}
    , _next_action{new QAction{tr("Next")}}
    , _previous_action{new QAction{tr("Previous")}}
    , _is_case_sensitive_action{new QAction{tr("Case sensitive"), this}}
    , _use_regexp_action{new QAction{tr("Use regular expressions"), this}}
{
    _find_action->setShortcut(QKeySequence::Find);
    connect(_find_action, &QAction::triggered, this, [this]() {
        set_pattern(_text_edit->textCursor().selectedText());
    });

    _next_action->setShortcut(QKeySequence::FindNext);
    connect(_next_action, &QAction::triggered, this, [this]() { find(false); });

    _previous_action->setShortcut(QKeySequence::FindPrevious);
    connect(_previous_action, &QAction::triggered, this, [this]() { find(true); });

    _is_case_sensitive_action->setCheckable(true);
    _use_regexp_action->setCheckable(true);
}

bool find_controller_t::is_pattern_valid() const
{
    if (use_regexp())
        return QRegularExpression{_pattern}.isValid();

    return true;
}

void find_controller_t::set_pattern(QString pattern)
{
    if (_pattern != pattern)
    {
        // Check if by any change the new pattern is just the old one with new data prepended to it.
        // In this case, we would do a backward search as it makes for a better incremental search
        // experience.
        bool new_pattern_ends_with_old_pattern = pattern.endsWith(_pattern);
        _pattern = pattern;
        emit pattern_changed(_pattern);

        // Move the cursor at the beginning of the current selection if any. This allows to update
        // the current selection if the pattern is extended but is still matching the current match
        // (e.g. do incremental search as the user type the pattern to search for).
        //
        // This can't be done in the find() function as otherwise search for the next match using
        // the same pattern (e.g. when click a "next" button) would always lead in the same match
        // being selected.
        auto start_of_search_cursor = _text_edit->textCursor();
        start_of_search_cursor.setPosition(
            start_of_search_cursor.selectionStart(), QTextCursor::MoveAnchor);
        _text_edit->setTextCursor(start_of_search_cursor);

        find(new_pattern_ends_with_old_pattern ? true : false);
    }
}

bool find_controller_t::is_case_sensitive() const
{
    return _is_case_sensitive_action->isChecked();
}

bool find_controller_t::use_regexp() const
{
    return _use_regexp_action->isChecked();
}

void find_controller_t::find(bool search_backward)
{
    const auto original_cursor = _text_edit->textCursor();
    bool found = false;

    QTextDocument::FindFlags flags{};
    if (is_case_sensitive())
        flags |= QTextDocument::FindCaseSensitively;
    if (search_backward)
        flags |= QTextDocument::FindBackward;

    if (use_regexp())
    {
        QRegularExpression::PatternOptions options{};

        // find with regexp ignores FindCaseSensitively so use the regexp option instead.
        if (!is_case_sensitive())
            options |= QRegularExpression::CaseInsensitiveOption;
        QRegularExpression regexp{_pattern, options};

        // find with regexp only searches in the current block. This means if no match is found we
        // need to iterate over each block until either a match is found, or we looped back to the
        // starting cursor position.
        do
        {
            found = _text_edit->find(regexp, flags);
            if (!found || _text_edit->textCursor().selectedText().isEmpty())
            {
                found = false;
                if (search_backward)
                {
                    // Move to the start of the block to ensure next find() call will search in the
                    // previous block.
                    _text_edit->moveCursor(QTextCursor::StartOfBlock);

                    // Start over from the end if we reached the start of the document.
                    if (_text_edit->textCursor().atStart())
                        _text_edit->moveCursor(QTextCursor::End);
                }
                else
                {
                    // Move first to the end of the current block to ensure that if there is no next
                    // block, the atEnd() checks succeeds and we loop over.
                    _text_edit->moveCursor(QTextCursor::EndOfBlock);

                    // Move to the next block (in any) to ensure next find() call will search in the
                    // next block.
                    _text_edit->moveCursor(QTextCursor::NextBlock);

                    // Start over from the start if we reached the end of the document.
                    if (_text_edit->textCursor().atEnd())
                        _text_edit->moveCursor(QTextCursor::Start);
                }
            }
        } while (!found && (_text_edit->textCursor().block() != original_cursor.block()));
    }
    else
    {
        // find with no regexp already searches in all blocks. We just need to handle loop back when
        // the start/end of the document is reached.
        found = _text_edit->find(_pattern, flags);
        if (!found)
        {
            // If the search failed from the current position, try again from the beginning/end.
            if (search_backward)
                _text_edit->moveCursor(QTextCursor::End);
            else
                _text_edit->moveCursor(QTextCursor::Start);

            found = _text_edit->find(_pattern, flags);
        }
    }

    // Restore the original cursor position if no match.
    if (!found)
        _text_edit->setTextCursor(original_cursor);
}
} // namespace flan
