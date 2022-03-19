
cmake_minimum_required(VERSION 3.22)
project(flan LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

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
)

add_library(flan SHARED ${SOURCES})
target_include_directories(flan PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(flan PUBLIC Qt6::Widgets)

option(FLAN_BUILD_APP "Build the flan application" TRUE)

if(FLAN_BUILD_APP)
    add_subdirectory(app)
endif()
