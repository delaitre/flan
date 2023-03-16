
# This CMakeLists downloads the qwt source code from the official git repo and adds CMake support to build and use it.
#
# This is a temporary solution until qwt officially supports CMake; see: https://sourceforge.net/p/qwt/feature-requests/95
cmake_minimum_required(VERSION 3.11)

project(qwt-cmake)

include(FetchContent)

FetchContent_Declare(
    qwt
    GIT_REPOSITORY https://git.code.sf.net/p/qwt/git
    GIT_TAG 1ac94a8dadcb82b8a8172ccd2a222175a736580b # qwt-multiaxes branch
)

FetchContent_GetProperties(qwt)
if(NOT ${qwt_POPULATED})
    message(STATUS "Populating target qwt...")
    FetchContent_Populate(qwt)
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets Concurrent Svg PrintSupport)

file(GLOB QWT_HEADERS ${qwt_SOURCE_DIR}/src/*.h)
file(GLOB QWT_SOURCES ${qwt_SOURCE_DIR}/src/*.cpp)

# QGLWidget and friends are not supported anymore in Qt6, so exclude related files from build.
LIST(REMOVE_ITEM QWT_HEADERS ${qwt_SOURCE_DIR}/src/qwt_plot_glcanvas.h)
LIST(REMOVE_ITEM QWT_SOURCES ${qwt_SOURCE_DIR}/src/qwt_plot_glcanvas.cpp)

add_library(qwt STATIC ${QWT_HEADERS} ${QWT_SOURCES})

set_target_properties(qwt
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${qwt_BINARY_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${qwt_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${qwt_BINARY_DIR}"
    AUTOMOC ON
    POSITION_INDEPENDENT_CODE ON
)

target_compile_definitions(qwt PRIVATE QWT_MOC_INCLUDE)

target_include_directories(qwt PUBLIC ${qwt_SOURCE_DIR}/src/)

target_link_libraries(qwt PUBLIC Qt6::Core Qt6::Gui Qt6::Widgets Qt6::OpenGL Qt6::OpenGLWidgets Qt6::Concurrent Qt6::Svg Qt6::PrintSupport)
