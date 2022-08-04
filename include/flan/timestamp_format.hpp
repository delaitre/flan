
#pragma once

#include <QMetaType>
#include <QRegularExpression>
#include <QString>
#include <vector>

namespace flan
{
struct timestamp_format_t
{
    bool is_enabled = true;
    QString name;
    QRegularExpression regexp;
    int hour_index = -1;
    int minute_index = -1;
    int second_index = -1;
    int millisecond_index = -1;
};

QDataStream& operator<<(QDataStream& out, const timestamp_format_t& format);
QDataStream& operator>>(QDataStream& in, timestamp_format_t& format);

using timestamp_format_list_t = std::vector<timestamp_format_t>;

timestamp_format_list_t get_default_timestamp_formats();
} // namespace flan

Q_DECLARE_METATYPE(flan::timestamp_format_t);
