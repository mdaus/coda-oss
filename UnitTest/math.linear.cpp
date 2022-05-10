#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define TEST_CASE(X) TEST_METHOD(X)

namespace Microsoft
{
namespace VisualStudio
{
namespace CppUnitTestFramework
{

template <typename T = double>
inline std::wstring ToString(const math::linear::Vector<T>& q)
{
    return ToString(str::toString(q));
}

template <size_t ND, typename T = double>
inline std::wstring ToString(const math::linear::VectorN<ND, T>& q)
{
    return ToString(str::toString(q));
}

template <typename T = double>
inline std::wstring ToString(const math::linear::Matrix2D<T>& q)
{
    return ToString(str::toString(q));
}

template <size_t MD, size_t ND, typename T = double>
inline std::wstring ToString(const math::linear::MatrixMxN<MD, ND, T>& q)
{
    return ToString(str::toString(q));
}

}
}
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

TEST_CLASS(test_lin){ public:
#include "math.linear/unittests/test_lin.cpp"
};

}