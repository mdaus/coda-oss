set(MODULE_NAME vectorclass)
set(TARGET_LANGUAGE c++)

coda_add_module(
	${MODULE_NAME}
        VERSION 2.7.1
        DEPS ${MODULE_DEPS})
