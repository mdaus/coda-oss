#include "pch.h"
#include "CppUnitTest.h"

#include <string>

#include "import/str.h"
#include "str/EncodedString.h"

namespace Microsoft
{
namespace VisualStudio
{
namespace CppUnitTestFramework
{
inline std::wstring ToString(const str::EncodedStringView& q)
{
    return q.wstring();
}

inline std::wstring ToString(const str::EncodedString& q)
{
    return ToString(q.view());
}

inline std::wstring ToString(const std::u8string& q)
{
    return ToString(str::EncodedStringView(q));
}

}
}
}

#include "TestCase.h"
namespace test
{
inline void assert_eq(const str::EncodedString& X1, const str::EncodedStringView& X2)
{
    TEST_ASSERT_EQ(X1.native(), X2.native());
}
inline void assert_eq(const str::EncodedStringView& X1, const str::EncodedString& X2)
{
    assert_eq(X2, X1);
}
}

namespace str
{
    TEST_CLASS(test_base_convert){ public:
    #include "str/unittests/test_base_convert.cpp"
    };

    TEST_CLASS(test_str){ public:
    #include "str/unittests/test_str.cpp"
    };
}
