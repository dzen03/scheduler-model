cmake_minimum_required(VERSION 3.27)
project(yql-model)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# option(BUILD_TESTING "Enable tests" ON)

option(USE_CLANG_TIDY "Enable clang tidy" ON)
set(CLANG_TIDY_NAME "clang-tidy" CACHE STRING "clang-tidy name/verion")
if(USE_CLANG_TIDY)
    find_program(CLANG_TIDY "${CLANG_TIDY_NAME}")
endif()

# find_package(opentelemetry-cpp CONFIG REQUIRED)
# if(NOT DEFINED prometheus-cpp_VERSION)
#   message(FATAL_ERROR "prometheus-cpp_VERSION is not defined")
# endif()
# message("TEST: " ${prometheus-cpp_INCLUDE_DIR})
# include_directories(${OPENTELEMETRY_CPP_INCLUDE_DIRS})

if(CLANG_TIDY)
    message(STATUS "Clang-tidy: ${CLANG_TIDY}")

    set_property(
            DIRECTORY
            APPEND
            PROPERTY CMAKE_CONFIGURE_DEPENDS clang-tidy-checks.txt
    )

    file(STRINGS clang-tidy-checks.txt clang_tidy_checks)
    list(JOIN clang_tidy_checks "," clang_tidy_checks_str)
    set(CMAKE_C_CLANG_TIDY
            ${CLANG_TIDY}
            -header-filter=${CMAKE_SOURCE_DIR}
            -checks=${clang_tidy_checks_str}
            -warnings-as-errors=*
    )
    set(CMAKE_CXX_CLANG_TIDY
            ${CLANG_TIDY}
            -header-filter=${CMAKE_SOURCE_DIR}
            -checks=${clang_tidy_checks_str}
            -warnings-as-errors=*
    )
endif()

add_subdirectory(server)
include_directories(server)

add_subdirectory(src)

add_executable(model main.cpp)
target_link_libraries(model PRIVATE model-sources simple-http-server-sources)
target_compile_options(model PRIVATE -Wall -Wextra -Wpedantic -Werror)
