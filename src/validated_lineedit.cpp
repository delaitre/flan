
#include <flan/validated_lineedit.hpp>
#include <QValidator>

namespace flan
{
validated_lineedit_t::validated_lineedit_t(QWidget* parent)
    : QLineEdit{parent}
{
    connect(this, &validated_lineedit_t::textChanged, this, &validated_lineedit_t::update_validity);
}

void validated_lineedit_t::set_validity_check_enabled(bool is_enabled)
{
    _is_validity_check_enabled = is_enabled;
    update_validity();
}

void validated_lineedit_t::update_validity()
{
    bool is_valid = true;

    if (auto v = validator(); _is_validity_check_enabled && v)
    {
        QString t = text();
        int position = cursorPosition();
        switch (v->validate(t, position))
        {
        case QValidator::Invalid:
            [[fallthrough]];
        case QValidator::Intermediate:
            is_valid = false;
            break;
        case QValidator::Acceptable:
            is_valid = true;
            break;
        }
    }

    // Update the base color depending on validity.
    QPalette palette = parentWidget()->palette();
    if (!is_valid)
    {
        constexpr QColor invalid_color{0xff, 0x80, 0x80};
        palette.setColor(QPalette::Active, QPalette::Base, invalid_color);
        palette.setColor(QPalette::Inactive, QPalette::Base, invalid_color);
    }

    setPalette(palette);
}
} // namespace flan
