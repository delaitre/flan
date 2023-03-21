
#pragma once

#include <QRegularExpression>
#include <QVector>
#include <QWidget>
#include <qwt_axis_id.h>

class QwtPlot;
class QwtPlotCurve;
class QDoubleSpinBox;
class QCheckBox;

namespace flan
{
class plot_series_data_t;
class validated_lineedit_t;

//! The plot widget area contains the plot and pattern linedit.
class plot_t : public QWidget
{
    Q_OBJECT

public:
    plot_t(QWidget* parent = nullptr);

public slots:
    //! Change the \a pattern_string used to extract data (\sa add_data) into curves.
    //!
    //! Each named captured group represents a curve with the same name, and the data captured
    //! (which mush be a number) is added to this curve's data.
    void set_pattern(QString pattern_string);

    //! Parse the \a line to extract new data for each curves according to the pattern set by
    //! set_pattern().
    void add_data(const QString& line);

    //! Clear all the curves data.
    void clear();

private:
    void assign_curve_to_axis(QwtPlotCurve* curve, QwtAxisId axis);

private slots:
    void rescale();

private:
    QwtPlot* _plot = nullptr;
    validated_lineedit_t* _pattern_lineedit = nullptr;
    QCheckBox* _window_checkbox = nullptr;
    QDoubleSpinBox* _window_spinbox = nullptr;

    QRegularExpression _pattern;
    QVector<int> _captured_indices;
    QVector<plot_series_data_t*> _curves_data;
};
} // namespace flan
