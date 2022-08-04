
#include <flan/timestamp_format.hpp>

namespace flan
{
QDataStream& operator<<(QDataStream& out, const timestamp_format_t& format)
{
    return out << format.is_enabled << format.name << format.regexp << format.hour_index
               << format.minute_index << format.second_index << format.millisecond_index;
}

QDataStream& operator>>(QDataStream& in, timestamp_format_t& format)
{
    return in >> format.is_enabled >> format.name >> format.regexp >> format.hour_index
        >> format.minute_index >> format.second_index >> format.millisecond_index;
}

timestamp_format_list_t get_default_timestamp_formats()
{
    timestamp_format_list_t list;

    // Match ss.msec
    auto default_format_ss_msec = timestamp_format_t{
        false,
        QObject::tr("ss.msec"),
        QRegularExpression{"(?:^|\\s)(\\d+)\\.(\\d+)(?:$|\\s)"},
        -1,
        -1,
        1,
        2};
    list.push_back(default_format_ss_msec);

    // Match mm:ss(.msec)
    auto default_format_mm_ss_msec = timestamp_format_t{
        true,
        QObject::tr("mm:ss(.msec)"),
        QRegularExpression{"(?:^|\\s)(\\d+):(\\d+)(?:\\.(\\d+))?(?:$|\\s)"},
        -1,
        1,
        2,
        3};
    list.push_back(default_format_mm_ss_msec);

    // Match hh:mm:ss(.msec)
    auto default_format_hh_mm_ss_msec = timestamp_format_t{
        true,
        QObject::tr("hh:mm:ss(.msec)"),
        QRegularExpression{"(?:^|\\s)(\\d+):(\\d+):(\\d+)(?:\\.(\\d+))?(?:$|\\s)"},
        1,
        2,
        3,
        4};
    list.push_back(default_format_hh_mm_ss_msec);

    return list;
}
} // namespace flan
