
#include <flan/main_widget.hpp>
#include <flan/matching_rule.hpp>
#include <flan/rule_model.hpp>
#include <flan/settings.hpp>
#include <QApplication>
#include <QMainWindow>
#include <QSettings>

using namespace flan;

namespace
{
QVector<matching_rule_t> get_initial_rules()
{
    QVector<matching_rule_t> rules;

    // Define FLAN_USE_DEFAULT_RULES to load a default set of rules instead of loading the settings.
    // This is useful for development.
//#define FLAN_USE_DEFAULT_RULES
#ifdef FLAN_USE_DEFAULT_RULES
    rules << matching_rule_t{
        "flan", QRegularExpression{"flan"}, filtering_behaviour_t::keep_line, true};
    rules << matching_rule_t{
        "Brackets", QRegularExpression{"[(){}[\\]]"}, filtering_behaviour_t::none, true};
    rules << matching_rule_t{
        "Strings", QRegularExpression{"\"[^\"]*\""}, filtering_behaviour_t::none, true};
    rules << matching_rule_t{
        "Empty line", QRegularExpression{"^\\w*$"}, filtering_behaviour_t::none, false};
    rules << matching_rule_t{
        "Anything", QRegularExpression{".*"}, filtering_behaviour_t::none, false};
#else
    auto settings = get_default_settings();
    rules = load_rules_from_settings(settings);
#endif

    return rules;
}

QString get_initial_text_log()
{
    // Define FLAN_USE_DEFAULT_TEXT_LOG to load a default text log.
    // This is useful for development.
//#define FLAN_USE_DEFAULT_TEXT_LOG
#ifdef FLAN_USE_DEFAULT_TEXT_LOG
    return QString{"int main(int argc, char** argv)\n"
                   "{\n"
                   "    QCoreApplication::setOrganizationName(\"flan\");\n"
                   "    QCoreApplication::setOrganizationDomain(\"flan\");\n"
                   "    QCoreApplication::setApplicationName(\"flan\");\n"
                   "\n"
                   "    QApplication app(argc, argv);\n"
                   "\n"
                   "    rule_model_t rule_model;\n"
                   "    rule_model.set_rules(get_initial_rules());\n"
                   "\n"
                   "    app.connect(&app, &QApplication::aboutToQuit, &app, [&rule_model]() {\n"
                   "        auto settings = get_default_settings();\n"
                   "        save_rules_to_settings(rule_model.rules(), settings);\n"
                   "    });\n"
                   "\n"
                   "    auto main_widget = new main_widget_t;\n"
                   "    main_widget->set_model(&rule_model);\n"
                   "    main_widget->set_content(get_initial_text_log());\n"
                   "\n"
                   "    QMainWindow main_window;\n"
                   "    main_window.setWindowIcon(QIcon(\":/icons/application\"));\n"
                   "    main_window.setCentralWidget(main_widget);\n"
                   "    main_window.show();\n"
                   "\n"
                   "    return app.exec();\n"
                   "}\n"};
#else
    return {};
#endif
}
} // namespace

int main(int argc, char** argv)
{
    QCoreApplication::setOrganizationName("flan");
    QCoreApplication::setOrganizationDomain("flan");
    QCoreApplication::setApplicationName("flan");

    QApplication app(argc, argv);

    rule_model_t rule_model;
    rule_model.set_rules(get_initial_rules());

    app.connect(&app, &QApplication::aboutToQuit, &app, [&rule_model]() {
        auto settings = get_default_settings();
        save_rules_to_settings(rule_model.rules(), settings);
    });

    auto main_widget = new main_widget_t;
    main_widget->set_model(&rule_model);
    main_widget->set_content(get_initial_text_log());

    QMainWindow main_window;
    main_window.setWindowIcon(QIcon(":/icons/application"));
    main_window.setCentralWidget(main_widget);
    main_window.show();

    return app.exec();
}
