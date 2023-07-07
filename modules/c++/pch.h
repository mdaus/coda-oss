// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#pragma once
#ifndef CODA_OSS_pch_h_INCLUDED_
#define CODA_OSS_pch_h_INCLUDED_

#include "coda_oss/CPlusPlus.h"
#include "config/disable_compiler_warnings.h"

CODA_OSS_disable_warning_system_header_push

// add headers that you want to pre-compile here
#include "framework.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "import/std.h"

CODA_OSS_disable_warning_pop


#include <net/net_config.h>
#include <zlib.h>
#include <xml/lite/xml_lite_config.h>

#pragma warning(disable: 4626) // '...': assignment operator was implicitly defined as deleted
#pragma warning(disable: 4625) // '...': copy constructor was implicitly defined as deleted
#pragma warning(disable: 4251) // '...': class '...' needs to have dll-interface to be used by clients of class '...'
#pragma warning(disable: 5105) // macro expansion producing '...' has undefined behavior
#pragma warning(disable: 4365) // '...': conversion from '...' to '...', signed/unsigned mismatch
#pragma warning(disable: 5039) // '...': pointer or reference to potentially throwing function passed to '...' function under -EHc. Undefined behavior may occur if this function throws an exception.

#pragma warning(disable: 26432) // If you define or delete any default operation in the type '...', define or delete them all (c.21).
#pragma warning(disable: 26429) // Symbol '...' is never tested for nullness, it can be marked as not_null (f.23).
#pragma warning(disable: 26455) // Default constructor should not throw. Declare it '...' (f.6).
#pragma warning(disable: 26440) // Function '...' can be declared '...' (f.6).
#pragma warning(disable: 26447) // The function is declared '...' but calls function '...' which may throw exceptions (f.6).
#pragma warning(disable: 26446) // Prefer to use gsl::at() instead of unchecked subscript operator (bounds.4).
#pragma warning(disable: 26481) // Don't use pointer arithmetic. Use span instead (bounds.1).
#pragma warning(disable: 26496) // The variable '...' does not change after construction, mark it as const (con.4).
#pragma warning(disable: 26462) // The value pointed to by '...' is assigned only once, mark it as a pointer to const (con.4).
#pragma warning(disable: 26409) // Avoid calling new and delete explicitly, use std::make_unique<T> instead (r.11).
#pragma warning(disable: 26401) // Do not delete a raw pointer that is not an owner<T> (i.11).
#pragma warning(disable: 26482) // Only index into arrays using constant expressions (bounds.2).
#pragma warning(disable: 26472) // Don't use a static_cast for arithmetic conversions. Use brace initialization, gsl::narrow_cast or gsl::narrow (type.1).
#pragma warning(disable: 26485) // Expression '...': No array to pointer decay (bounds.3).
#pragma warning(disable: 26493) // Don't use C-style casts (type.4).
#pragma warning(disable: 26451) // Arithmetic overflow: Using operator '...' on a 4 byte value and then casting the result to a 8 byte value. Cast the value to the wider type before calling operator '...' to avoid overflow (io.2).
#pragma warning(disable: 26494) // Variable '...' is uninitialized. Always initialize an object (type.5).
#pragma warning(disable: 26814) // The const variable '...' can be computed at compile-time. Consider using constexpr (con.5).
#pragma warning(disable: 26826) // Don't use C-style variable arguments (f.55).
#pragma warning(disable: 26426) // Global initializer calls a non-constexpr function '...' (i.22).
#pragma warning(disable: 26435) // Function '...' should specify exactly one of '...', '...', or '...' (c.128).
#pragma warning(disable: 26434) // Function '...' hides a non-virtual function '...'.
#pragma warning(disable: 26400) // Do not assign the result of an allocation or a function call with an owner<T> return value to a raw pointer, use owner<T> instead (i.11).
#pragma warning(disable: 26433) // Function '...' should be marked with '...' (c.128).

// Yes, these are our files ... but they don't change very often, and if they do
// change we want to rebuild everything anyway.
#include "gsl/gsl.h"
#include "config/Exports.h"
#include "except/Throwable.h"
#include "sys/Conf.h"
#include "sys/filesystem.h"
#include "mem/SharedPtr.h"
#include "xml/lite/xerces_.h"
#include "hdf5/lite/H5_.h"

#endif //CODA_OSS_pch_h_INCLUDED_
