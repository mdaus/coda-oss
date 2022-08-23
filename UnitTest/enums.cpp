#include "pch.h"
#include "CppUnitTest.h"

#include <import/enums.h>
#include <enums/Convert.h>
#include <enums/Test.h>

namespace enums
{
    TEST_CLASS(test_enums_convert){ public:
    #include "enums/unittests/test_enums_convert.cpp"
    };
}
