
#pragma once

#include <QAction>
#include <QObject>
#include <QPlainTextEdit>
#include <QString>

namespace flan
{
class find_controller_t : public QObject
{
    Q_OBJECT

public:
    find_controller_t(QPlainTextEdit* text_edit, QObject* parent = nullptr);

    QString pattern() const { return _pattern; }
    bool is_pattern_valid() const;

    QAction* find_action() { return _find_action; }
    QAction* next_action() { return _next_action; }
    QAction* previous_action() { return _previous_action; }
    QAction* is_case_sensitive_action() { return _is_case_sensitive_action; }
    QAction* use_regexp_action() { return _use_regexp_action; }

public slots:
    void set_pattern(QString pattern);

signals:
    void pattern_changed(QString pattern);

private:
    bool is_case_sensitive() const;
    bool use_regexp() const;
    void find(bool search_backward);

private:
    QPlainTextEdit* _text_edit;
    QString _pattern;

    QAction* _find_action;
    QAction* _next_action;
    QAction* _previous_action;
    QAction* _is_case_sensitive_action;
    QAction* _use_regexp_action;
};
} // namespace flan
