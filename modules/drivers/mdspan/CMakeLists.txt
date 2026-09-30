set(DRIVER_NAME kokkos-mdspan)
FetchContent_Declare(${DRIVER_NAME}
URL "${CMAKE_CURRENT_SOURCE_DIR}/mdspan-mdspan-0.6.0.tar.gz"
URL_HASH "SHA256=79f94d7f692cbabfbaff6cd0d3434704435c853ee5087b182965fa929a48a592"
SOURCE_SUBDIR "IGNORE"  # This makes it so "add_subdirectory" isn't called
)
FetchContent_MakeAvailable(${DRIVER_NAME})

set(TARGET_LANGUAGE c++)
coda_add_module(mdspan)
target_include_directories(mdspan-c++ INTERFACE "$<BUILD_INTERFACE:${${DRIVER_NAME}_SOURCE_DIR}/include>")