#include <mem/CircularBuffer.h>
#include "TestCase.h"
#include <iostream>

namespace
{
TEST_CASE(tests)
{
    std::vector<int> stub({ 1, 2, 3, 4, 5 });

    std::cerr << "init test" << std::endl;
    mem::CircularBuffer<int> cc(10);
    TEST_ASSERT_EQ(cc.availableForRead(), 0);
    TEST_ASSERT_EQ(cc.availableForWrite(), 10);

    std::cerr << "read throw test" << std::endl;
    TEST_EXCEPTION(cc.read(stub.data(), 1));
    TEST_ASSERT_EQ(stub[0], 1);

    std::cerr << "simple write tests" << std::endl;

    cc.write(stub.data(), stub.size());
    TEST_ASSERT_EQ(cc.availableForRead(), 5);
    TEST_ASSERT_EQ(cc.availableForWrite(), 5);

    cc.write(stub.data(), stub.size());
    TEST_ASSERT_EQ(cc.availableForRead(), 10);
    TEST_ASSERT_EQ(cc.availableForWrite(), 0);
   
    std::cerr << "indexing tests" << std::endl;

    TEST_ASSERT_EQ(cc[0], 1);
    TEST_ASSERT_EQ(cc[5], 1);
    TEST_ASSERT_EQ(cc[7], 3);
    TEST_ASSERT_EQ(cc[10], 1);
    TEST_ASSERT_EQ(cc[15], 1);
    TEST_ASSERT_EQ(cc[17], 3);

    std::cerr << "simple read tests" << std::endl;

    std::vector<int> readStub(10, 0);
    size_t ne = cc.read(readStub.data(), 7);
    TEST_ASSERT_EQ(ne, 7);
    TEST_ASSERT_EQ(readStub[0], 1);
    TEST_ASSERT_EQ(readStub[1], 2);
    TEST_ASSERT_EQ(readStub[5], 1);
    TEST_ASSERT_EQ(cc.availableForRead(), 3);
    TEST_ASSERT_EQ(cc.availableForWrite(), 7);

    std::cerr << "Wrapping write tests" << std::endl;

    stub = std::vector<int>({5, 6, 7, 8, 9});

    cc.write(stub.data(), 2);
    TEST_ASSERT_EQ(cc.availableForRead(), 5);
    TEST_ASSERT_EQ(cc.availableForWrite(), 5);
    
    cc.read(readStub.data(), 5);
    TEST_ASSERT_EQ(cc.availableForRead(), 0);
    TEST_ASSERT_EQ(cc.availableForWrite(), 10);

    cc.write(stub.data(), 5);
    TEST_ASSERT_EQ(cc.availableForRead(), 5);
    TEST_ASSERT_EQ(cc.availableForWrite(), 5);
    cc.write(stub.data(), 5);

    std::cerr << "Wrapping read tests" << std::endl;
    cc.read(readStub.data(), 10);
    TEST_ASSERT_EQ(readStub[0], 5);
    TEST_ASSERT_EQ(readStub[5], 5);
    TEST_ASSERT_EQ(readStub[7], 7);

    std::cerr << "write throw test" << std::endl;
    TEST_EXCEPTION(cc.write(stub.data(), 1000));

    std::cerr << "zero element exception test" << std::endl;
    TEST_EXCEPTION( mem::CircularBuffer<int>(0) );
}
}

int main(int, char**)
{
    TEST_CHECK(tests);
    return 0;
}

