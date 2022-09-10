set(TARGET_NAME hdf5)

# We're mostly about static libs and C++
option(BUILD_SHARED_LIBS OFF)
option(BUILD_STATIC_LIBS ON) # default is ON, repeated here for clarity
option(HDF5_BUILD_CPP_LIB ON) # TODO: might not need for our wrappers?

# We're just going to expose an wrapper API
option(HDF5_BUILD_EXAMPLES OFF)
option(HDF5_BUILD_TOOLS OFF)
option(HDF5_BUILD_HL_TOOLS OFF)
option(HDF5_BUILD_UTILS OFF)

# assume the code as provided works; we'll test our own API
option(BUILD_TESTING OFF)
option(TEST_SHELL_SCRIPTS OFF)

option(HDF5_ENABLE_DEPRECATED_SYMBOLS OFF) # leave these off until we know we need them
option(HDF5_DISABLE_COMPILER_WARNINGS ON) # don't want to change downloaded code

set(HDF5_INSTALL_INCLUDE_DIR "${CODA_STD_PROJECT_INCLUDE_DIR}/${TARGET_NAME}")
add_subdirectory (hdf5-1.12.2)
