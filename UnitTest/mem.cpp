#include "pch.h"
#include "CppUnitTest.h"

#include <import/mem.h>

namespace mem
{

TEST_CLASS(test_scoped_cloneable_ptr){ public:
#include "mem/unittests/test_scoped_cloneable_ptr.cpp"
};

}