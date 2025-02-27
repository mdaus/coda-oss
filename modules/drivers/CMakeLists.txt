# Declare options
option(ENABLE_XML "enable XML" ON)
option(ENABLE_J2K "enable J2K library" ON)
option(ENABLE_JARS "include jars in the install" ON)
option(ENABLE_JPEG "enable use of libjpeg" ON)
option(ENABLE_PCRE "enable PCRE library" ON)
option(ENABLE_UUID "enable UUID library" ON)
option(ENABLE_ZIP "enable zlib" ON)
option(CODA_ENABLE_HDF5 "enable hdf5" ON)

# Turn off all warnings; this is code we don't control.
if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)

    # By default, there is a /W3 on the command-line from somewhere (?); adding
    # /Wn results in a compiler warning.
    #add_compile_options(/W0)
    string(REGEX REPLACE "/W[0-4]" "/W0" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REGEX REPLACE "/W[0-4]" "/W0" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
elseif (UNIX)
    add_compile_options(-w) # "Inhibit all warning messages"
endif()

#add_subdirectory("curl") # this is handled in coda_find_system_dependencies
#add_subdirectory("numpy") # this is handled in coda_find_system_dependencies
if(NOT CONAN_PACKAGE_NAME)
    set(XML_HOME "" CACHE PATH "path to pre-existing XML library installation")
    if(ENABLE_XML OR XML_HOME)
        add_subdirectory("xml")
    endif()
endif()

#add_subdirectory("gsl") # nothing to do

if(NOT CONAN_PACKAGE_NAME)
    set(J2K_HOME "" CACHE PATH "path to J2K installation")
    if (ENABLE_J2K OR J2K_HOME)
        add_subdirectory("j2k")
    endif()
endif()

if (ENABLE_JARS)
    add_subdirectory("jars")
endif()

if (NOT CONAN_PACKAGE_NAME)
    set(JPEG_HOME "" CACHE PATH "path to libjpeg installation")
    if (ENABLE_JPEG OR JPEG_HOME)
        add_subdirectory("jpeg")
    endif()
endif()

if (NOT CONAN_PACKAGE_NAME)
    set(PCRE_HOME "" CACHE PATH "path to PCRE installation")
    if (ENABLE_PCRE OR PCRE_HOME)
        add_subdirectory("pcre")
    endif()
endif()

set(SQL_LAYER "" CACHE STRING "SQL backend [mysql, psql, oracle]")
set(SQL_HOME "" CACHE PATH "Path to SQL installation")
if (SQL_LAYER OR SQL_HOME)
    add_subdirectory("sql")
endif()

if (ENABLE_UUID OR UUID_HOME)
    add_subdirectory("uuid")
endif()

if (NOT CONAN_PACKAGE_NAME)
    set(ZIP_HOME "" CACHE PATH "path to pre-existing zlib installation, if not provided zlib will be built")
    if (ENABLE_ZIP OR ZIP_HOME)
        add_subdirectory("zlib")
    endif()
endif()

set(CODA_HDF5_HOME "" CACHE PATH "path to pre-existing HDF5 installation, if not provided HDF5 will be built")
if (CODA_ENABLE_HDF5 OR CODA_HDF5_HOME)
    add_subdirectory("hdf5")
    add_subdirectory("highfive")
endif()
