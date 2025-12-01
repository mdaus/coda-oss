# - CODA-OSS CMake Build script.
#
# Author:  Scott A. Colcord

cmake_minimum_required(VERSION 3.26)
project(coda-oss VERSION 1.0.0 LANGUAGES C CXX)

if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    # build and package with conan
    include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    conan_basic_setup()

    include("${CMAKE_BINARY_DIR}/conan_paths.cmake")
endif()

if (${CMAKE_PROJECT_NAME} STREQUAL coda-oss)
    # this is the top level project

    # Only set std if not used as library
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
    install(DIRECTORY "cmake/"
            DESTINATION "${CODA_STD_PROJECT_LIB_DIR}/cmake/"
            FILES_MATCHING PATTERN "*.cmake")

    # If the package version is defined, write a version file for find_package
    set(version_file "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}ConfigVersion.cmake")
    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(${version_file} VERSION ${CMAKE_PROJECT_VERSION} COMPATIBILITY SameMajorVersion)
    install(FILES ${version_file} DESTINATION "lib/cmake")

    if(CODA_BUILD_TESTS)
        # Catch2 for unittests
        find_package(Catch2 3 QUIET)
        if(NOT catch2_FOUND)
            message(STATUS "Couldn't find Catch2 installed. Pulling from Github")
            include(FetchContent)

            FetchContent_Declare(
                Catch2
                GIT_REPOSITORY https://github.com/catchorg/Catch2.git
                GIT_TAG v3.11.0
            )
            FetchContent_MakeAvailable(Catch2)
        endif()
    endif()
    
endif()

add_subdirectory("modules")

# generate package config, with the following paths exported
set(JARS_DIR ${CODA_STD_PROJECT_LIB_DIR} CACHE INTERNAL
    "path to installed jars, needed by downstream projects")
coda_generate_package_config(JARS_DIR)
