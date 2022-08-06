
#include <flan/timestamp_format.hpp>
#include <chrono>

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

QTime timestamp_format_t::time_for(const QString& s) const
{
    if (!regexp.isValid())
        return {};

    if (auto match = regexp.match(s); match.hasMatch())
    {
        // If the capture index does not correspond to anything, a null string is returned.
        // If the conversion to int fails, 0 is returned.
        // So in all cases, we end up with 0 in case something goes wrong which is what we
        // need.
        auto hours = std::chrono::hours{match.captured(hour_index).toInt()};
        auto minutes = std::chrono::minutes{match.captured(minute_index).toInt()};
        auto seconds = std::chrono::seconds{match.captured(second_index).toInt()};
        auto milliseconds = std::chrono::milliseconds{match.captured(millisecond_index).toInt()};

        auto total = std::chrono::milliseconds{hours + minutes + seconds + milliseconds};
        return QTime::fromMSecsSinceStartOfDay(total.count());
    }

    return {};
}

} // namespace flan
