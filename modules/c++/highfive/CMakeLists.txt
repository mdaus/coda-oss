set(MODULE_NAME highfive)

if(CODA_ENABLE_HDF5)
    set(MODULE_DEPS config-c++)
    list(APPEND MODULE_DEPS hdf5-c++)

    coda_add_module(
        ${MODULE_NAME}
        VERSION 2.6.2
        DEPS ${MODULE_DEPS})
else()
    message("${MODULE_NAME} will not be built since HDF5 is not enabled")
endif()
