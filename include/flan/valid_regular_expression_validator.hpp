
#pragma once

#include <QRegularExpression>
#include <QValidator>

namespace flan
{
class valid_regular_expression_validator_t : public QValidator
{
    Q_OBJECT

public:
    valid_regular_expression_validator_t(QObject* parent = nullptr)
        : QValidator{parent}
    {
    }

    inline State validate(QString& input, int&) const final
    {
        return QRegularExpression{input}.isValid() ? State::Acceptable : State::Intermediate;
    }
};
} // namespace flan
