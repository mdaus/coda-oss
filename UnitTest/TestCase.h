#pragma once

#include <stdint.h>
#include <string>

#include "CppUnitTest.h"
#include "include/TestCase.h"

#undef TEST_CHECK
#undef TEST_ASSERT
#undef TEST_ASSERT_NULL 
#undef TEST_ASSERT_TRUE
#undef TEST_ASSERT_FALSE
#undef TEST_MAIN
#undef TEST_CASE
#define TEST_CHECK(X) 
#define TEST_MAIN(X)
#define TEST_ASSERT_NULL(X) Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNull((X))
#define TEST_ASSERT_TRUE(X) Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue((X))
#define TEST_ASSERT_FALSE(X) Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsFalse((X))
#define TEST_ASSERT(X) TEST_ASSERT_TRUE(X)
#define TEST_CASE(X) TEST_METHOD(X)

template<>
inline std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString(const uint16_t& q)
{
	return std::to_wstring(q);
}

namespace test
{
    struct Assert final
    {
        static void FailOnCondition(bool condition, const unsigned short* message, const Microsoft::VisualStudio::CppUnitTestFramework::__LineInfo* pLineInfo);    
        static std::wstring GetAssertMessage(bool equality, const std::string& expected, const std::string& actual, const wchar_t *message);
    };
#undef EQUALS_MESSAGE
#define EQUALS_MESSAGE(expected, actual, message) reinterpret_cast<const unsigned short*>(test::Assert::GetAssertMessage(true, str::toString(expected),  str::toString(actual), message).c_str())

template <typename TX1, typename TX2>
inline void assert_eq__(const TX1& X1, const TX2& X2)
{
    if (!(X1 == X2))
    {
        // AreEqual<> wants both "expected" and "actual" to be the same type;
        // that doesn't take into account operator==().
        const void* pX2 = &X2;
        const auto pX2_as_X1 = reinterpret_cast<const TX1*>(pX2); // == already failed; this is just to trigger the assert
        Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(X1, *pX2_as_X1);
    }
}
template <typename TX1, typename TX2>
inline void assert_ne__(const TX1& X1, const TX2& X2)
{
    if (!(X1 != X2))
    {
        // AreEqual<> wants both "expected" and "actual" to be the same type; 
        // that doesn't take into account operator!=().
        const void* pX2 = &X2;
        const auto pX2_as_X1 = reinterpret_cast<const TX1*>(pX2); // != already failed; this is just to trigger the assert
        Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual(X1, *pX2_as_X1);
    }
}

template <typename TX1, typename TX2>
inline void assert_eq_(const TX1& X1, const TX2& X2)
{
    assert_eq__(X1, X2);
    if (X1 != X2)  // X1 == X2 means X1 != X2 can't be true
    {
        assert_ne__(X1, X2); // trigger assert
    }
}

// see Assert::AreEqual<>
template<typename TExpected, typename TActual>
inline void diePrintf_eq(const TExpected& expected, const TActual& actual,
    const wchar_t* message = NULL, const Microsoft::VisualStudio::CppUnitTestFramework::__LineInfo* pLineInfo = NULL)
{
	Assert::FailOnCondition(expected == actual, EQUALS_MESSAGE(expected, actual, message), pLineInfo);
}
}

#undef CODA_OSS_test_diePrintf_eq_
#define CODA_OSS_test_diePrintf_eq_(X1, X2) test::diePrintf_eq(X1, X2)


namespace test
{
template <typename TX1, typename TX2>
inline void assert_ne_(const TX1& X1, const TX2& X2)
{
    assert_ne__(X1, X2);
    if (X1 == X2)  // X1 != X2 means X1 == X2 can't be true
    {
        assert_eq__(X1, X2); // trigger assert
    }
}
template <typename TX1, typename TX2>
inline void assert_ne(const TX1& X1, const TX2& X2)
{
    assert_ne_(X1, X2);
    assert_ne_(X2, X1); // X1 != X2 means X2 != X1
}
}
#undef TEST_ASSERT_NOT_EQ
#define TEST_ASSERT_NOT_EQ(X1, X2) test::assert_ne(X1, X2);

template <typename TX1, typename TX2>
inline void test_assert_greater_(const TX1& X1, const TX2& X2)
{
	Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue(X1 > X2);
}
#undef TEST_ASSERT_GREATER
#define TEST_ASSERT_GREATER(X1, X2) test_assert_greater_(X1, X2)

#undef TEST_ASSERT_ALMOST_EQ_EPS
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

#undef TEST_ASSERT_ALMOST_EQ 
#define TEST_ASSERT_ALMOST_EQ(X1, X2) test::assert_almost_eq(X1, X2)

#undef TEST_ASSERT_EQ_MSG
#define TEST_ASSERT_EQ_MSG(msg, X1, X2) Microsoft::VisualStudio::CppUnitTestFramework::Logger::WriteMessage(msg.c_str()); TEST_ASSERT_EQ(X1, X2)

#undef TEST_EXCEPTION
#undef TEST_THROWS
#define TEST_EXCEPTION(X) try { (X); TEST_ASSERT_FALSE(false); } catch (...) { TEST_ASSERT_TRUE(true); }
#define TEST_THROWS(X) TEST_EXCEPTION(X)


