set(MODULE_NAME xml.dom)

if(ENABLE_XML)
    set(MODULE_DEPS xml.lite-c++ io-c++ mem-c++ mt-c++ logging-c++ str-c++ coda_oss-c++)
    if (USE_XercesC)
        list(APPEND MODULE_DEPS xerces-c)
    endif()

    coda_add_module(
        ${MODULE_NAME}
        VERSION 1.0
        DEPS ${MODULE_DEPS})

    coda_add_tests(
        MODULE_NAME ${MODULE_NAME}
        DIRECTORY "unittests"
        UNITTEST)
else()
    message("${MODULE_NAME} will not be built since XML is not enabled")
endif()
