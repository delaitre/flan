
#pragma once

#include <QRegularExpression>
#include <QString>
#include <vector>

namespace flan
{
//! Extract curve names and data from lines using regular expressions.
class curve_data_parser_t
{
public:
    //! Change the \a pattern_string used to extract data (\sa set_data) into curves.
    //!
    //! Each named captured group represents a curve with the same name, and the data captured
    //! (which must be a number) represent the data for this curve.
    void set_pattern(QString pattern_string);

    //! Parse the \a line to extract new data for each curves according to the pattern set by
    //! set_pattern().
    //!
    //! The parser only keeps the last parsed data for each curve and is returned by curve_data().
    void set_data(QString line);

    //! Get the name of each curve the parser will parse.
    const std::vector<QString>& curve_names() const { return _curve_names; }

    //! Get the last parsed data for each curve the parser will parse.
    const std::vector<double>& curve_data() const { return _curve_data; }

private:
    QRegularExpression _pattern;
    std::vector<int> _captured_indices;
    std::vector<QString> _curve_names;
    std::vector<double> _curve_data;
};
} // namespace flan
