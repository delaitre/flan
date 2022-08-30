
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

QString timestamp_format_t::match_hour(const QString& s) const
{
    return match_index(s, hour_index);
}

QString timestamp_format_t::match_minute(const QString& s) const
{
    return match_index(s, minute_index);
}

QString timestamp_format_t::match_second(const QString& s) const
{
    return match_index(s, second_index);
}

QString timestamp_format_t::match_millisecond(const QString& s) const
{
    return match_index(s, millisecond_index);
}

QTime timestamp_format_t::time_for(const QString& s) const
{
    if (!regexp.isValid())
        return {};

    if (auto match = regexp.match(s); !match.hasMatch())
        return {};

    // If the capture index does not correspond to anything, a null string is returned.
    // If the conversion to int fails, 0 is returned.
    // So in all cases, we end up with 0 in case something goes wrong which is what we
    // need.
    auto hours = std::chrono::hours{match_hour(s).toInt()};
    auto minutes = std::chrono::minutes{match_minute(s).toInt()};
    auto seconds = std::chrono::seconds{match_second(s).toInt()};
    auto milliseconds = std::chrono::milliseconds{match_millisecond(s).toInt()};

    auto total = std::chrono::milliseconds{hours + minutes + seconds + milliseconds};
    return QTime::fromMSecsSinceStartOfDay(total.count());
}

QString timestamp_format_t::match_index(const QString& s, int index) const
{
    if (!regexp.isValid())
        return {};

    if (auto match = regexp.match(s); match.hasMatch())
        return match.captured(index);

    return {};
}
} // namespace flan
