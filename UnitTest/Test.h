#pragma once

#include <stdint.h>
#include <string>

#include "CppUnitTest.h"

namespace Microsoft{ namespace VisualStudio {namespace CppUnitTestFramework
{
template<>
inline std::wstring ToString(const uint16_t& q)
{
	return std::to_wstring(q);
}
}}}

#define TEST_ASSERT(X) Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue(X)

namespace test
{
template <typename TX1, typename TX2>
inline void assert_eq(const TX1& X1, const TX2& X2)
{
    Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(X1, X2);
    Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(X2, X1);
}
}
#define TEST_ASSERT_EQ(X1, X2) test::assert_eq(X1, X2);
#define TEST_ASSERT_EQ_INT(X1, X2) TEST_ASSERT_EQ(X2, X1)
#define TEST_ASSERT_EQ_STR(X1, X2) TEST_ASSERT_EQ(to_string(X1), to_string(X2))
#define TEST_ASSERT_EQ_FLOAT(X1, X2) TEST_ASSERT_EQ(static_cast<float>(X1), static_cast<float>(X2))

#define TEST_ASSERT_NULL(X) Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNull((X))
#define TEST_ASSERT_NOT_NULL(X) Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull((X))
#define TEST_ASSERT_TRUE(X) Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue((X))
#define TEST_ASSERT_FALSE(X) Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsFalse((X))

template<typename TX1, typename TX2>
inline void test_assert_not_eq_(const TX1& X1, const TX2& X2)
{
	Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual(X1, X2);
	Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual(X2, X1);
}
#define TEST_ASSERT_NOT_EQ(X1, X2) test_assert_not_eq_(X1, X2);
template <typename TX1, typename TX2>
inline void test_assert_greater_(const TX1& X1, const TX2& X2)
{
	Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue(X1 > X2);
}
#define TEST_ASSERT_GREATER(X1, X2) test_assert_greater_(X1, X2)

#define TEST_ASSERT_ALMOST_EQ_EPS(X1, X2, EPS) { Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(X1, X2, EPS); Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(X2, X1, EPS); }
namespace test
{
inline void assert_almost_eq(float X1, float X2)
{
    constexpr auto EPS = static_cast<float>(0.0001);
    TEST_ASSERT_ALMOST_EQ_EPS(X1, X2, EPS);
}
inline void assert_almost_eq(double X1, double X2)
{
    constexpr auto EPS = static_cast<double>(0.0001);
    TEST_ASSERT_ALMOST_EQ_EPS(X1, X2, EPS);
}
inline void assert_almost_eq(long double X1, long double X2)
{
    assert_almost_eq(static_cast<double>(X1), static_cast<double>(X2));
}
}

#define TEST_ASSERT_ALMOST_EQ(X1, X2) test::assert_almost_eq(X1, X2)

#define TEST_ASSERT_EQ_MSG(msg, X1, X2) Microsoft::VisualStudio::CppUnitTestFramework::Logger::WriteMessage(msg.c_str()); TEST_ASSERT_EQ(X1, X2)

#define TEST_EXCEPTION(X) try { (X); TEST_ASSERT_FALSE(false); } catch (...) { TEST_ASSERT_TRUE(true); }
#define TEST_THROWS(X) TEST_EXCEPTION(X)

#define TEST_MAIN(X)
