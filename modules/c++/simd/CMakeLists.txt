set(MODULE_NAME simd)

coda_add_module(${MODULE_NAME}
    VERSION 1.0
    DEPS vectorclass-c++ coda_oss-c++ config-c++ gsl-c++ sys-c++)

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "unittests"
    UNITTEST)
