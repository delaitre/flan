
#include <flan/data_source_delegate.hpp>
#include <flan/data_source_delegate_provider_scratch_buffer.hpp>
#include <flan/data_source_delegate_provider_serial_port.hpp>
#include <flan/data_source_delegate_provider_stdin.hpp>
#include <flan/main_widget.hpp>
#include <flan/matching_rule.hpp>
#include <flan/rule_model.hpp>
#include <flan/settings.hpp>
#include <flan/timestamp_format.hpp>
#include <QApplication>
#include <QMainWindow>
#include <QSettings>
#include <QStyleFactory>

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

timestamp_format_list_t get_initial_timestamp_formats()
{
    timestamp_format_list_t formats =
        load_timestamp_formats_from_json(get_default_settings_file_for_timestamp_formats());
    if (formats.empty())
        formats = get_default_timestamp_formats();

    return formats;
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

    // app.setStyle("Fusion");
    // app.setPalette(QStyleFactory::create("Fusion")->standardPalette());
    // app.setStyle("Adwaita-Dark");
    // app.setPalette(QStyleFactory::create("Adwaita-Dark")->standardPalette());
    app.setStyle("Adwaita");
    app.setPalette(QStyleFactory::create("Adwaita")->standardPalette());

    rule_model_t rule_model;
    auto rule_root = get_initial_rules();
    rule_model.set_root(rule_root.get());

    auto main_widget = new main_widget_t;
    main_widget->set_model(&rule_model);
    main_widget->set_content(get_initial_text_log());
    main_widget->set_timestamp_formats(get_initial_timestamp_formats());

    std::vector<data_source_delegate_provider_t*> provider_list;
    data_source_delegate_provider_scratch_buffer_t scratch_buffer_provider;
    provider_list.push_back(&scratch_buffer_provider);
    data_source_delegate_provider_stdin_t stdin_provider;
    provider_list.push_back(&stdin_provider);
    data_source_delegate_provider_serial_port_t serial_port_provider;
    provider_list.push_back(&serial_port_provider);

    auto update_aggregated_delegate = [&]() {
        std::vector<data_source_delegate_t*> delegates;
        auto append_delegates = [&](data_source_delegate_provider_t& provider) {
            auto new_delegates = provider.delegates();
            delegates.insert(delegates.end(), new_delegates.begin(), new_delegates.end());
        };

        for (auto provider: provider_list)
            append_delegates(*provider);

        main_widget->set_data_sources(std::move(delegates));
    };

    for (auto provider: provider_list)
    {
        QObject::connect(
            provider,
            &data_source_delegate_provider_t::data_source_delegates_changed,
            main_widget,
            update_aggregated_delegate);
    }

    update_aggregated_delegate();

    QMainWindow main_window;
    main_window.setWindowIcon(QIcon(":/icons/application"));
    main_window.setCentralWidget(main_widget);
    main_window.resize(1200, 700);
    main_window.show();

    app.connect(
        &app, &QApplication::aboutToQuit, &app, [rule_root = rule_root.get(), main_widget]() {
            save_rules_to_json(*rule_root, get_default_settings_file_for_rules());
            save_timestamp_formats_to_json(
                main_widget->timestamp_formats(),
                get_default_settings_file_for_timestamp_formats());
        });

    return app.exec();
}
