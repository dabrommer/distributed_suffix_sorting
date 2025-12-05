cmake_minimum_required(VERSION 3.13)

project(DDCX_lib)
set(NAMESPACE_NAME "DDCX_lib")

option(BUILD_SHARED_LIBS "Build the shared library" ON)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(GenerateExportHeader)
include(FetchContent)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_NINJA_PARALLEL_PROCESSING ON)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)

list(APPEND
        DSSS_WARNING_FLAGS
        # "-Werror"
        "-Wall"
        "-Wextra"
        "-Wundef"
        "-Wunreachable-code"
        "-Wno-unused-parameter"
        # "-Wpedantic"
        "-Wnull-dereference"
        "-Wimplicit-fallthrough"
        "-Wno-vla"
        "-Wno-pessimizing-move"
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  list(APPEND DSSS_WARNING_FLAGS "-Wno-gnu-zero-variadic-macro-arguments")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -DDEBUG -march=native -ggdb")

# compile options
option(OPTIMIZE_DATA_TYPES "use smallest data type possible for ranks in pdcx" OFF)
option(INCLUDE_ALL_SORTERS "include all axtmann sorters" OFF)
message(STATUS "---> OPTIMIZE_DATA_TYPES: ${OPTIMIZE_DATA_TYPES}")
message(STATUS "---> INCLUDE_ALL_SORTERS: ${INCLUDE_ALL_SORTERS}")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

set(CPP_LIBRARY_VERSION_MAJOR 0)
set(CPP_LIBRARY_VERSION_MINOR 1)
set(CPP_LIBRARY_VERSION_PATCH 0)
set(CPP_LIBRARY_VERSION_STRING ${CPP_LIBRARY_VERSION_MAJOR}.${CPP_LIBRARY_VERSION_MINOR}.${CPP_LIBRARY_VERSION_PATCH})

FetchContent_Declare(
        kaval
        GIT_REPOSITORY https://github.com/niklas-uhl/kaval.git
        GIT_TAG 1600bda
        SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/kaval)

FetchContent_Declare(
        kamping
        GIT_REPOSITORY https://github.com/kamping-site/kamping.git
        GIT_TAG 1effe84
        SYSTEM)

FetchContent_Declare(
        cli11
        GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
        GIT_TAG v2.3.2
        SYSTEM)

FetchContent_Declare(magic-enum
        GIT_REPOSITORY  https://github.com/Neargye/magic_enum.git
        GIT_TAG         v0.9.7
        SYSTEM
)

FetchContent_Declare(tlx
        GIT_REPOSITORY  https://github.com/tlx/tlx.git
        GIT_TAG         cf83363
        SYSTEM
)
FetchContent_MakeAvailable(tlx)

set(IPS4O_DISABLE_PARALLEL OFF)
FetchContent_Declare(ips4o
        GIT_REPOSITORY  https://github.com/mschimek/ips4o.git
        GIT_TAG         eb30352
        SYSTEM
)
FetchContent_MakeAvailable(ips4o)

FetchContent_Declare(kadis
        GIT_REPOSITORY  https://github.com/mschimek/KaDiS.git
        GIT_TAG         908a9aa
        SYSTEM
)

FetchContent_MakeAvailable(kaval kamping cli11 magic-enum kadis)


add_library(${PROJECT_NAME})
add_library(${NAMESPACE_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

find_package(MPI REQUIRED)
add_subdirectory(src)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hash
        ${CMAKE_CURRENT_SOURCE_DIR}/src/mpi
        ${CMAKE_CURRENT_SOURCE_DIR}/src/pdcx
        ${CMAKE_CURRENT_SOURCE_DIR}/src/sorters
        ${CMAKE_CURRENT_SOURCE_DIR}/src/strings
        ${CMAKE_CURRENT_SOURCE_DIR}/src/util
)
target_link_libraries(${PROJECT_NAME} PRIVATE kamping::kamping MPI::MPI_CXX tlx ips4o kadis magic_enum::magic_enum CLI11::CLI11)

generate_export_header(${PROJECT_NAME}
        EXPORT_FILE_NAME ${PROJECT_NAME}/${PROJECT_NAME}_export.h
)
set(public_header_files
        src/include/dss_interface.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}_export.h
)

target_compile_options(${PROJECT_NAME} PRIVATE ${DSSS_WARNING_FLAGS})
set_target_properties(${PROJECT_NAME} PROPERTIES
        LANGUAGES CXX
        VERSION ${CPP_LIBRARY_VERSION_STRING}
        SOVERSION ${CPP_LIBRARY_VERSION_MAJOR}
        PUBLIC_HEADER "${public_header_files}"
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN 1
)
target_sources(${PROJECT_NAME}
        PRIVATE
        ${public_header_files}
        src/executables/cli.cpp
)

# Define headers for this library. PUBLIC headers are used for
# compiling the library, and will be added to consumers' build
# paths.
target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# adjust this path depending where cmake searches for the target files
set(ConfigPackageLocation "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

# 'make install' to the correct locations (provided by GNUInstallDirs).
install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Library
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Library
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}  COMPONENT Library # This is for Windows
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME} COMPONENT Development
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT ${PROJECT_NAME}Targets
        FILE
        ${PROJECT_NAME}Targets.cmake
        NAMESPACE
        ${NAMESPACE_NAME}::
        DESTINATION
        ${ConfigPackageLocation}
        COMPONENT
        Development
)

configure_package_config_file(
        ${PROJECT_NAME}Config.cmake.in
        ${PROJECT_NAME}Config.cmake
        INSTALL_DESTINATION "${ConfigPackageLocation}"
        PATH_VARS CMAKE_INSTALL_PREFIX
)

write_basic_package_version_file(
        ${PROJECT_NAME}ConfigVersion.cmake
        VERSION ${CPP_LIBRARY_VERSION_STRING}
        COMPATIBILITY AnyNewerVersion
)

install(
        FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION
        ${ConfigPackageLocation}
        COMPONENT
        Development
)

# Install dss_interface.hpp
install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/include/dss_interface.hpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)

# Install public dependency directories
install(
    DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}/src/util/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/util
    FILES_MATCHING PATTERN "*.hpp"
)

# Install generated export header
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/DDCX_lib
)


