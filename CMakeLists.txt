
cmake_minimum_required(VERSION 3.22)
project(flan LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Check if a versionless version of Qt is already defined. If so, use it, otherwise search for Qt6.
# This allows projects using flan to search for their own Qt version first and flan will happily use
# the same.
if(NOT TARGET Qt::Core)
    find_package(Qt6 REQUIRED COMPONENTS Widgets SerialPort)
endif()

set(SOURCES
    include/flan/data_source.hpp
    src/data_source.cpp
    include/flan/data_source_scratch_buffer.hpp
    src/data_source_scratch_buffer.cpp
    include/flan/data_source_serial_port.hpp
    src/data_source_serial_port.cpp
    include/flan/data_source_stdin.hpp
    src/data_source_stdin.cpp
    include/flan/data_source_delegate.hpp
    src/data_source_delegate.cpp
    include/flan/data_source_scratch_buffer_delegate.hpp
    src/data_source_scratch_buffer_delegate.cpp
    include/flan/data_source_serial_port_delegate.hpp
    src/data_source_serial_port_delegate.cpp
    include/flan/data_source_stdin_delegate.hpp
    src/data_source_stdin_delegate.cpp
    include/flan/data_source_selection_widget.hpp
    src/data_source_selection_widget.cpp
    include/flan/elided_label.hpp
    src/elided_label.cpp
    include/flan/find_controller.hpp
    src/find_controller.cpp
    include/flan/find_widget.hpp
    src/find_widget.cpp
    include/flan/main_widget.hpp
    src/main_widget.cpp
    include/flan/rule_tree_widget.hpp
    src/rule_tree_widget.cpp
    include/flan/rule_model.hpp
    src/rule_model.cpp
    include/flan/log_widget.hpp
    src/log_widget.cpp
    include/flan/log_margin_area_widget.hpp
    src/log_margin_area_widget.cpp
    include/flan/matching_rule.hpp
    include/flan/rule_highlighter.hpp
    src/rule_highlighter.cpp
    include/flan/settings.hpp
    src/settings.cpp
    include/flan/styled_matching_rule.hpp
    include/flan/rule_set.hpp
    src/rule_set.cpp
    include/flan/style.hpp
    src/style.cpp
    include/flan/rule_editor.hpp
    src/rule_editor.cpp
    include/flan/valid_regular_expression_validator.hpp
    src/libflan.qrc
    include/flan/rule_tree_view.hpp
    src/rule_tree_view.cpp
    include/flan/color_button.hpp
    src/color_button.cpp
    include/flan/style_button.hpp
    src/style_button.cpp
    include/flan/style_editor_dialog.hpp
    src/style_editor_dialog.cpp
    include/flan/style_list_widget.hpp
    src/style_list_widget.cpp
    include/flan/pcre_cheatsheet_dialog.hpp
    src/pcre_cheatsheet_dialog.cpp
    src/stdin_socket_notifier.hpp
    src/stdin_socket_notifier.cpp
)

add_library(flan SHARED ${SOURCES})
target_include_directories(flan PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(flan PUBLIC Qt::Widgets PRIVATE Qt::SerialPort)

option(FLAN_TEST_RULE_MODEL "Test the rule model as it is being exercised" FALSE)

if(FLAN_TEST_RULE_MODEL)
    if(NOT TARGET Qt::Test)
        find_package(Qt6 REQUIRED COMPONENTS Test)
    endif()

    target_compile_definitions(flan PRIVATE FLAN_TEST_RULE_MODEL)
    target_link_libraries(flan PRIVATE Qt::Test)
endif()

option(FLAN_BUILD_APP "Build the flan application" TRUE)

if(FLAN_BUILD_APP)
    add_subdirectory(app)
endif()
