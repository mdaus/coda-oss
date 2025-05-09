# - CODA-OSS CMake Build script.
#
# Author:  Scott A. Colcord

cmake_minimum_required(VERSION 3.14)
project(coda-oss)

if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    # build and package with conan
    include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    conan_basic_setup()

    include("${CMAKE_BINARY_DIR}/conan_paths.cmake")
endif()

if (${CMAKE_PROJECT_NAME} STREQUAL coda-oss)
    # this is the top level project

    # Allow other project to set different standard. 
    set(CMAKE_CXX_STANDARD 14)
    set(CXX_STANDARD_REQUIRED true)

    # Always turn on "warnings as errors" to avoid lots of (meaningless?) build output;
    # we'll dial-back warnings as necessary.
    if (MSVC)
       add_compile_options(/WX) # warnings as errors
       add_compile_options(/MP) # multi-processor compile
    elseif (UNIX)
       add_compile_options(-Werror) # warnings as errors
    endif()

    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
    include(CodaBuild)

    coda_initialize_build()

    # install our cmake build modules for downstream use
    install(DIRECTORY "cmake/"
            DESTINATION "${CODA_STD_PROJECT_LIB_DIR}/cmake/"
            FILES_MATCHING PATTERN "*.cmake")
endif()

add_subdirectory("modules")

# generate package config, with the following paths exported
set(JARS_DIR ${CODA_STD_PROJECT_LIB_DIR} CACHE INTERNAL
    "path to installed jars, needed by downstream projects")
set(SWIG_INCLUDE_DIR "${CODA_STD_PROJECT_INCLUDE_DIR}/swig" CACHE INTERNAL
    "path to installed SWIG includes, needed by downstream projects")
coda_generate_package_config(JARS_DIR SWIG_INCLUDE_DIR)
