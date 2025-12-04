include(FetchContent)
if(CODA_BUILD_TESTS) 
    FetchContent_Declare(Catch2 URL "${CMAKE_CURRENT_SOURCE_DIR}/Catch2-3.11.0.tar.gz")
    FetchContent_MakeAvailable(Catch2)
endif()

set(GSL_INSTALL ON)
FetchContent_Declare(GSL URL "${CMAKE_CURRENT_SOURCE_DIR}/GSL-4.2.0.tar.gz")
FetchContent_MakeAvailable(GSL)
