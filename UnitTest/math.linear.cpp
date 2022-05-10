#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define TEST_CASE(X) TEST_METHOD(X)

template <>
inline std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString(const math::linear::Matrix2D<double>& q)
{
    return ToString(str::toString(q));
}

namespace math_linear
{
TEST_CLASS(test_eigenvalue){ public:
#include "math.linear/unittests/test_eigenvalue.cpp"
};

TEST_CLASS(test_inf_equality){ public:
#include "math.linear/unittests/test_inf_equality.cpp"
};

TEST_CLASS(test_inverse){ public:
#include "math.linear/unittests/test_inverse.cpp"
};

//TEST_CLASS(test_lin){ public:
//#include "math.linear/unittests/test_lin.cpp"
//};

}