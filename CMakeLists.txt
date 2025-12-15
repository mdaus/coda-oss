# - CODA-OSS CMake Build script.
#
# Author:  Scott A. Colcord

cmake_minimum_required(VERSION 3.26)
project(coda-oss VERSION 2.0.0 LANGUAGES C CXX)

if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    # build and package with conan
    include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    conan_basic_setup()

    include("${CMAKE_BINARY_DIR}/conan_paths.cmake")
endif()

if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 14)
    message(FATAL_ERROR "CODA-OSS require at least gcc-14")
endif()

if (${CMAKE_PROJECT_NAME} STREQUAL coda-oss)
    # this is the top level project
    set(CMAKE_CXX_STANDARD 23)
    set(CXX_STANDARD_REQUIRED true)

    # Always turn on "warnings as errors" to avoid lots of (meaningless?) build output;
    # we'll dial-back warnings as necessary.
    if (MSVC)
        # No warnings as errors for MSVC
        add_compile_options(/MP) # multi-processor compile
    elseif (UNIX)
        add_compile_options(-Werror) # warnings as errors
    endif()

    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
    include(CodaBuild)

    coda_initialize_build()

    # install our cmake build modules for downstream use
    install(DIRECTORY "cmake" DESTINATION "lib" FILES_MATCHING PATTERN "*.cmake")

endif()

# As opposed to drivers, these are "required" third-party packages
add_subdirectory("externals")
add_subdirectory("modules")

# generate package config, with the following paths exported
set(JARS_DIR lib CACHE INTERNAL "path to installed jars, needed by downstream projects")
coda_generate_package_config(JARS_DIR)
