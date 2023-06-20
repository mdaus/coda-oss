set(MODULE_NAME vectorclass)
set(TARGET_LANGUAGE c++)

set(MODULE_DEPS config-c++)

coda_add_module(
    ${MODULE_NAME}
    VERSION 1.0.0
    DEPS ${MODULE_DEPS})


