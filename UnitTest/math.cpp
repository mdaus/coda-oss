#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define TEST_CASE(X) TEST_METHOD(X)

TEST_CLASS(test_bessel_functions){ public:
#include "math/unittests/test_bessel_functions.cpp"
};

TEST_CLASS(test_convex_hull){ public:
#include "math/unittests/test_convex_hull.cpp"
};

