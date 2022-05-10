#include "pch.h"
#include "TestCase.h"

#include "str/EncodedStringView.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// C++ hack to call private methods
// https://stackoverflow.com/questions/6873138/calling-private-method-in-c?msclkid=dd8b1f8bd09711ec8610b4501a04de94

// declare method's type
using FailOnCondition_t = void(bool condition, const unsigned short* message, const __LineInfo* pLineInfo);
using GetAssertMessage_t = std::wstring(bool equality, const std::wstring& expected, const std::wstring& actual, const wchar_t *message);

// helper structure to inject call() code
template <FailOnCondition_t fFailOnCondition, GetAssertMessage_t fGetAssertMessage>
struct caller final
{
    friend void FailOnCondition(bool condition, const unsigned short* message, const __LineInfo* pLineInfo)
    {
        fFailOnCondition(condition, message, pLineInfo);
    }

    friend std::wstring GetAssertMessage(bool equality, const std::wstring& expected, const std::wstring& actual, const wchar_t *message)
    {
        return fGetAssertMessage(equality, expected, actual, message);
    }
};

// even instantiation of the helper
template struct caller<&Assert::FailOnCondition, &Assert::GetAssertMessage>;

void FailOnCondition(bool condition, const unsigned short* message, const __LineInfo* pLineInfo);  // declare caller
void test::Assert::FailOnCondition(bool condition, const unsigned short* message, const __LineInfo* pLineInfo)
{
    ::FailOnCondition(condition, message, pLineInfo);  // and call!
}

std::wstring GetAssertMessage(bool equality, const std::wstring& expected, const std::wstring& actual, const wchar_t *message); // declare caller
std::wstring test::Assert::GetAssertMessage(bool equality, const std::string& expected, const std::string& actual, const wchar_t *message)
{
    const str::EncodedStringView vExpected(expected);
    const str::EncodedStringView vActual(actual);
    return ::GetAssertMessage(equality, vExpected.wstring(), vActual.wstring(), message); // and call!
}
