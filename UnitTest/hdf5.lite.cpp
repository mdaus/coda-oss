#include "pch.h"
#include "CppUnitTest.h"

#include <mem/ComplexView.h>
#include <import/hdf5/lite.h>
#pragma comment(lib, "hdf5-c++.lib")

namespace hdf5_lite
{

TEST_CLASS(test_highfive){ public:
#include "hdf5.lite/unittests/test_highfive.cpp"
};

}