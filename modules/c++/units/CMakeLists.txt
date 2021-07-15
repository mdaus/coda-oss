set(MODULE_NAME units)

coda_add_module(
    ${MODULE_NAME}
    VERSION 0.1
    DEPS except-c++ str-c++ sys-c++ types-c++ math-c++)

coda_add_tests(
    MODULE_NAME ${MODULE_NAME}
    DIRECTORY "unittests"
    UNITTEST)
