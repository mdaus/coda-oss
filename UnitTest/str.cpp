#include "pch.h"
#include "CppUnitTest.h"

#include <string>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define TEST_CASE(X) TEST_METHOD(X)

//TEST_CLASS(test_base_convert){ public:
//    const std::string testName;
//#include "str/unittests/test_base_convert.cpp"
//};

TEST_CLASS(test_str){ public:
#include "str/unittests/test_str.cpp"
};
