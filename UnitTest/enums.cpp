#include "pch.h"
#include "CppUnitTest.h"

#include <import/enums.h>
#include <enums/Convert.h>
#include <enums/Test.h>

namespace enums
{
    TEST_CLASS(test_enums_details){ public:
    #include "enums/unittests/test_enums_details.cpp"
    };

    TEST_CLASS(test_enums_convert){ public:
    #include "enums/unittests/test_enums_convert.cpp"
    };

    TEST_CLASS(test_enums_macros){ public:
    #include "enums/unittests/test_enums_macros.cpp"
    };

    TEST_CLASS(test_enums_Enum){ public:
    #include "enums/unittests/test_enums_Enum.cpp"
    };
}
