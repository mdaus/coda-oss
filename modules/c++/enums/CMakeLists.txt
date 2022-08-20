set(MODULE_NAME enums)

coda_add_module(
    ${MODULE_NAME}
    VERSION 1.0
    DEPS str-c++ except-c++ coda_oss-c++)

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "unittests"
    DEPS std-c++
    UNITTEST)
