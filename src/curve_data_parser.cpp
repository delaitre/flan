
#include <flan/curve_data_parser.hpp>

namespace flan
{
void curve_data_parser_t::set_pattern(QString pattern_string)
{
    QRegularExpression pattern{pattern_string};
    if (!pattern.isValid())
        return;

    _pattern = pattern;
    _pattern.optimize();

    _curve_names.clear();
    _curve_data.clear();
    _captured_indices.clear();

    // We will create one curve per named, non empty capture group
    auto named_groups = _pattern.namedCaptureGroups();
    for (int i = 0; i < named_groups.size(); ++i)
    {
        auto name = named_groups[i];
        if (!name.isEmpty())
        {
            _curve_names.push_back(name);
            _captured_indices.push_back(i);
        }
    }

    _curve_data.resize(_curve_names.size());
}

void curve_data_parser_t::set_data(QString line)
{
    QRegularExpressionMatch match = _pattern.match(line);

    for (int i = 0; i < _captured_indices.size(); ++i)
    {
        if (QString s = match.captured(_captured_indices[i]); !s.isNull())
        {
            bool ok = false;
            double value = s.toDouble(&ok);

            // Keep the previous value if a double couldn't be parsed.
            if (ok)
            {
                _curve_data[i] = value;
            }
        }
    }
}
} // namespace flan
