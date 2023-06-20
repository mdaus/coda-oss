#include "pch.h"
#include "CppUnitTest.h"

namespace simd
{

TEST_CLASS(test_Vec){ public:
#include "simd/unittests/test_Vec.cpp"
};

TEST_CLASS(test_simdMath){ public:
#include "simd/unittests/test_simdMath.cpp"
};

}