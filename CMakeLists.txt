cmake_minimum_required(VERSION 3.14...3.18)
project(capi VERSION "2.0.0")

# Currently, Scikit-build does not support FindPython, so we convert the
# provided hints ourselves.
if(SKBUILD)
    set(Python_EXECUTABLE "${PYTHON_EXECUTABLE}")
    set(Python_INCLUDE_DIR "${PYTHON_INCLUDE_DIR}")
    set(Python_LIBRARY "${PYTHON_LIBRARY}")
    set(DUMMY "${PYTHON_VERSION_STRING}")  # Not needed, silences a warning
endif()

find_package(Python REQUIRED COMPONENTS Interpreter Development)

# Scikit-Build does not add your site-packages to the search path automatically,
# so we need to add it _or_ the pybind11 specific directory here.
execute_process(
        COMMAND
        "${Python_EXECUTABLE}" -c
        "import pybind11; print(pybind11.get_cmake_dir())"
        OUTPUT_VARIABLE _tmp_dir
        OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ECHO STDOUT)
list(APPEND CMAKE_PREFIX_PATH "${_tmp_dir}")

# Now we can find pybind11
find_package(pybind11 CONFIG REQUIRED)

# Get our source files
set(VIS_GRAPH_IMPL_DIR ${CMAKE_SOURCE_DIR}/capi/src/implementation/visibility_graphs)
set(VIS_GRAPH_INTERFACES_DIR ${CMAKE_SOURCE_DIR}/capi/src/interfaces/visibility_graphs)

file(GLOB_RECURSE VIS_GRAPH_IMPLEMENTATION_FILES "${VIS_GRAPH_IMPL_DIR}/code/*.cpp" "${VIS_GRAPH_IMPL_DIR}/code/*.hpp")
file(GLOB_RECURSE VIS_GRAPH_INTERFACE_FILES "${VIS_GRAPH_INTERFACES_DIR}/code/*.hpp")
include_directories(${VIS_GRAPH_IMPL_DIR}/code ${VIS_GRAPH_INTERFACES_DIR})

pybind11_add_module(_vis_graph MODULE ${VIS_GRAPH_IMPLEMENTATION_FILES} ${VIS_GRAPH_INTERFACE_FILES} ${VIS_GRAPH_IMPL_DIR}/bindings.cpp)
target_compile_definitions(_vis_graph PRIVATE VERSION_INFO=${PROJECT_VERSION})
install(TARGETS _vis_graph DESTINATION .)

# Setup unit tests
set(TEST_DIR "capi/test")
file(GLOB_RECURSE TEST_FILES "${TEST_DIR}/*.cpp" "${TEST_DIR}/*.hpp")

# Generate a test executable
set(CPP_LIBS_DIR ${CMAKE_SOURCE_DIR}/capi/cpp_libs)
include_directories(${CPP_LIBS_DIR}/catch2)
add_executable("${PROJECT_NAME}_test" ${TEST_FILES} ${VIS_GRAPH_IMPLEMENTATION_FILES} ${VIS_GRAPH_INTERFACE_FILES} ${CPP_LIBS_DIR}/catch2/catch.cpp)
