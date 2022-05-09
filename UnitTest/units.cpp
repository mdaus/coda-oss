#include "pch.h"
#include "CppUnitTest.h"

#include <units/Angles.h>
#include <units/Lengths.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define TEST_CASE(X) TEST_METHOD(X)

namespace units
{
    TEST_CLASS(test_units){ public:
    #include "units/unittests/test_units.cpp"
    };
}
