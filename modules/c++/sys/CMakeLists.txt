set(MODULE_NAME sys)

coda_generate_module_config_header(${MODULE_NAME})
if (CMAKE_CXX_COMPILER_ID MATCHES "CLANG|GNU")
    add_compile_options(-Wno-unused-parameter)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/wd4100)
endif()

coda_add_module(
    ${MODULE_NAME}
    VERSION 1.2
    DEPS config-c++ except-c++ str-c++ gsl-c++ coda_oss-c++)

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "tests"
    FILTER_LIST "MMapReadOnlyTest.cpp" "ProcessTest1.cpp")
coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "unittests"
    DEPS mt-c++ std-c++
    UNITTEST)
