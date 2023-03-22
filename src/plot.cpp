
#include <flan/plot.hpp>
#include <flan/valid_regular_expression_validator.hpp>
#include <flan/validated_lineedit.hpp>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QPalette>
#include <QPen>
#include <QRandomGenerator>
#include <QVBoxLayout>
#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
#include <qwt_point_data.h>
#include <qwt_scale_widget.h>

namespace flan
{
namespace
{
//! Return \a count different colors.
std::vector<QColor> get_colors(int count)
{
    std::vector<QColor> colors;
    colors.reserve(count);

    auto hue_increment = 1.0f / count;
    while (colors.size() < count)
        colors.push_back(QColor::fromHsvF(hue_increment * colors.size(), 0.9f, 0.8f));

    return colors;
}
} // namespace

class plot_series_data_t : public QwtSeriesData<QPointF>
{
public:
    size_t size() const final { return _xx.size(); }
    QPointF sample(size_t i) const final { return {_xx[i], _yy[i]}; }
    QRectF boundingRect() const final { return _bounding_rect; }

    void append_sample(QPointF sample)
    {
        if (_xx.isEmpty())
        {
            _bounding_rect.setCoords(sample.x(), sample.y(), sample.x(), sample.y());
        }
        else
        {
            _bounding_rect.setLeft(std::min(sample.x(), _bounding_rect.left()));
            _bounding_rect.setRight(std::max(sample.x(), _bounding_rect.right()));
            _bounding_rect.setTop(std::min(sample.y(), _bounding_rect.top()));
            _bounding_rect.setBottom(std::max(sample.y(), _bounding_rect.bottom()));
        }

        _xx.append(sample.x());
        _yy.append(sample.y());
    }

    void clear()
    {
        _xx.clear();
        _yy.clear();
        _bounding_rect = QRectF{};
    }

private:
    QVector<double> _xx;
    QVector<double> _yy;
    QRectF _bounding_rect = {};
};

plot_t::plot_t(QWidget* parent)
    : QWidget{parent}
    , _plot{new QwtPlot}
    , _pattern_lineedit{new validated_lineedit_t}
    , _window_checkbox{new QCheckBox{tr("Fixed window")}}
    , _window_spinbox{new QDoubleSpinBox}
{
    // Pattern
    _pattern_lineedit->setValidator(new valid_regular_expression_validator_t{_pattern_lineedit});
    _pattern_lineedit->setToolTip(tr(
        "Regular expression used to extract data for each curve.\n\nEach named captured group "
        "will convert the captured\nnumber and add it to the curve which name is the name\nof the "
        "corresponding capture group (e.g. \"(?<Time>\\d+)\"\nwill extract an integer and add it "
        "to the \"Time\" curve"));
    connect(_pattern_lineedit, &QLineEdit::textChanged, this, &plot_t::set_pattern);

    // Window size
    _window_checkbox->setChecked(true);
    _window_spinbox->setRange(0, 100000);
    _window_spinbox->setValue(100);
    _window_spinbox->setSuffix(tr(" units"));
    _window_spinbox->setEnabled(_window_checkbox->isChecked());
    connect(_window_checkbox, &QCheckBox::toggled, _window_spinbox, &QDoubleSpinBox::setEnabled);
    connect(_window_checkbox, &QCheckBox::toggled, this, &plot_t::rescale);
    connect(_window_spinbox, &QDoubleSpinBox::valueChanged, this, &plot_t::rescale);

    // Create bottom layout
    auto bottom_layout = new QHBoxLayout;
    bottom_layout->addWidget(new QLabel{tr("Data pattern")});
    bottom_layout->addWidget(_pattern_lineedit);
    bottom_layout->addWidget(_window_checkbox);
    bottom_layout->addWidget(_window_spinbox);

    // Plot
    QwtPlotCanvas* canvas = new QwtPlotCanvas();
    canvas->setLineWidth(1);
    canvas->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);

    QPalette canvasPalette(Qt::white);
    canvas->setPalette(canvasPalette);

    _plot->setCanvas(canvas);

    new QwtPlotMagnifier{_plot->canvas()};
    new QwtPlotPanner{_plot->canvas()};

    // Add pattern and plot to layout
    auto main_layout = new QVBoxLayout;
    main_layout->setContentsMargins(8, 0, 0, 0);
    main_layout->addWidget(_plot);
    main_layout->addLayout(bottom_layout);
    setLayout(main_layout);
}

void plot_t::set_pattern(QString pattern_string)
{
    _curve_parser.set_pattern(pattern_string);
    _pattern_lineedit->setText(pattern_string);

    _curves_data.clear();
    _plot->detachItems(QwtPlotItem::Rtti_PlotItem, true);

    // Get the name of the curves.
    auto& curve_names = _curve_parser.curve_names();
    auto curve_count = curve_names.size();

    // Ensure the plot has enough axes
    _plot->setAxesCount(QwtAxis::XBottom, 1);
    QwtAxisId x_axis_id(QwtAxis::XBottom, 0);
    _plot->setAxisAutoScale(x_axis_id, true);
    _plot->setAxesCount(QwtAxis::YLeft, curve_count);

    // Generate different colors for each curve
    auto colors = get_colors(curve_count);

    // Create each curve with its own axis
    for (auto i = 0; i < curve_count; ++i)
    {
        auto name = curve_names[i];

        QwtPlotCurve* curve = new QwtPlotCurve();
        curve->setTitle(name);
        curve->setPen(colors[i], 2);
        curve->setRenderHint(QwtPlotItem::RenderAntialiased, false);

        auto curve_data = new plot_series_data_t;
        _curves_data.append(curve_data);
        curve->setSamples(curve_data);

        QwtAxisId y_axis_id(QwtAxis::YLeft, i);
        _plot->setAxisAutoScale(y_axis_id, true);
        assign_curve_to_axis(curve, y_axis_id);

        curve->attach(_plot);
    }

    _plot->replot();
}

void plot_t::add_data(const QString& line)
{
    _curve_parser.set_data(line);
    auto& curve_data = _curve_parser.curve_data();

    for (int i = 0; i < curve_data.size(); ++i)
    {
        _curves_data[i]->append_sample(
            {static_cast<double>(_curves_data[i]->size()), curve_data[i]});
    }

    rescale();
    _plot->replot();
}

void plot_t::clear()
{
    for (auto curve_data: _curves_data)
        curve_data->clear();
}

void plot_t::assign_curve_to_axis(QwtPlotCurve* curve, QwtAxisId axis)
{
    curve->setYAxis(axis);

    // Update axis color to match curve
    auto axis_widget = _plot->axisWidget(axis);
    auto axis_palette = axis_widget->palette();
    auto color = curve->pen().color();
    axis_palette.setColor(QPalette::Text, color); // label
    axis_palette.setColor(QPalette::WindowText, color); // axis and ticks
    axis_widget->setPalette(axis_palette);

    // Update axis title to match curve
    axis_widget->setTitle(curve->title());
}

void plot_t::rescale()
{
    auto x_axis = QwtAxisId{QwtAxis::XBottom, 0};

    if (_window_checkbox->isChecked())
    {
        if (!_curves_data.isEmpty())
        {
            auto bounding_rect = _curves_data.first()->boundingRect();
            _plot->setAxisScale(
                x_axis, bounding_rect.right() - _window_spinbox->value(), bounding_rect.right());
        }
    }
    else
    {
        _plot->setAxisAutoScale(x_axis, true);
    }
}
} // namespace flan
