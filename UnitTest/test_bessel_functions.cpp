#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


TEST_CLASS(test_bessel_functions){
    public:

#define TEST_CASE(X) TEST_METHOD(X)
#include "math/unittests/test_bessel_functions.cpp"

};

