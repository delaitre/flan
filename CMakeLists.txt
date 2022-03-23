
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
    find_package(Qt6 REQUIRED COMPONENTS Widgets)
endif()

set(SOURCES
    include/flan/main_widget.hpp
    src/main_widget.cpp
    include/flan/rule_tree_widget.hpp
    src/rule_tree_widget.cpp
    include/flan/rule_model.hpp
    src/rule_model.cpp
    include/flan/log_widget.hpp
    src/log_widget.cpp
    include/flan/matching_rule.hpp
    include/flan/rule_highlighter.hpp
    src/rule_highlighter.cpp
    include/flan/settings.hpp
    src/settings.cpp
    include/flan/rule_set.hpp
    src/rule_set.cpp
    include/flan/style.hpp
    src/style.cpp
    include/flan/rule_editor.hpp
    src/rule_editor.cpp
    include/flan/valid_regular_expression_validator.hpp
    src/libflan.qrc
)

add_library(flan SHARED ${SOURCES})
target_include_directories(flan PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(flan PUBLIC Qt::Widgets)

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
