add_subdirectory("drivers")

if (NOT CODA_OSS_BUILD_ONLY_DRIVERS)
    add_subdirectory("c++")
    if (BUILD_PYTHON_MODULES)
        add_subdirectory("python")
    endif()
endif()
