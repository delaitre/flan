
#include <flan/data_source_scratch_buffer.hpp>
#include <flan/data_source_scratch_buffer_delegate.hpp>
#include <flan/data_source_serial_port.hpp>
#include <flan/data_source_serial_port_delegate.hpp>
#include <flan/data_source_stdin.hpp>
#include <flan/data_source_stdin_delegate.hpp>
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
base_node_uniq_t get_initial_rules()
{
    base_node_uniq_t root;

    // Define FLAN_USE_DEFAULT_RULES to load a default set of rules instead of loading the settings.
    // This is useful for development.
//#define FLAN_USE_DEFAULT_RULES
#ifdef FLAN_USE_DEFAULT_RULES
    root = std::make_unique<base_node_t>();

    auto add_group_node = [](base_node_t& parent, QString name) -> base_node_t& {
        return parent.add_child(std::make_unique<group_node_t>(std::move(name)));
    };

    auto add_rule_node = [](base_node_t& parent, matching_rule_t rule) -> base_node_t& {
        return parent.add_child(std::make_unique<rule_node_t>(std::move(rule)));
    };

    add_rule_node(
        *root,
        matching_rule_t{
            "flan", QRegularExpression{"flan"}, filtering_behaviour_t::keep_line, false});

    auto& group_coding = add_group_node(*root, "Coding");
    add_rule_node(
        group_coding,
        matching_rule_t{
            "Brackets", QRegularExpression{"[(){}[\\]]"}, filtering_behaviour_t::none, true});
    add_rule_node(
        group_coding,
        matching_rule_t{
            "Strings", QRegularExpression{"\"[^\"]*\""}, filtering_behaviour_t::none, true});

    auto& group_general = add_group_node(*root, "General");
    add_rule_node(
        group_general,
        matching_rule_t{
            "Empty line", QRegularExpression{"^\\w*$"}, filtering_behaviour_t::none, false});
    add_rule_node(
        group_general,
        matching_rule_t{"Anything", QRegularExpression{".*"}, filtering_behaviour_t::none, false});
#else
    root = load_rules_from_json(get_default_settings_file_for_rules());
#endif

    return root;
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
    QApplication app(argc, argv);

    rule_model_t rule_model;
    auto rule_root = get_initial_rules();
    rule_model.set_root(rule_root.get());

    app.connect(&app, &QApplication::aboutToQuit, &app, [rule_root = rule_root.get()]() {
        save_rules_to_json(*rule_root, get_default_settings_file_for_rules());
    });

    auto main_widget = new main_widget_t;
    main_widget->set_model(&rule_model);
    main_widget->set_content(get_initial_text_log());

    data_source_selection_widget_t::data_source_delegate_list_t data_source_list;
    data_source_list.push_back(new data_source_scratch_buffer_delegate_t{
        *(new data_source_scratch_buffer_t{main_widget}), main_widget});
    data_source_list.push_back(
        new data_source_stdin_delegate_t{*(new data_source_stdin_t{main_widget}), main_widget});
    data_source_list.push_back(new data_source_serial_port_delegate_t{
        *(new data_source_serial_port_t{main_widget}), main_widget});
    main_widget->set_data_sources(std::move(data_source_list));

    QMainWindow main_window;
    main_window.setWindowIcon(QIcon(":/icons/application"));
    main_window.setCentralWidget(main_widget);
    main_window.resize(1200, 700);
    main_window.show();

    return app.exec();
}
