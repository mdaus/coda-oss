/* =========================================================================
 * This file is part of io-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * io-c++ is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include <span>
#include <cstddef>

#include <import/io.h>
#include <mem/BufferView.h>
#include <sys/Conf.h>
#include <catch2/catch_test_macros.hpp>
#include <string.h>

TEST_CASE("testStringStream")
{
    io::StringStream stream;
    stream.writeln("test");
    stream.writeln("test");
    CHECK(stream.available() == 10);
    stream.seek(1, io::Seekable::START);
    CHECK(stream.available() == 9);

    stream.write("0123456789");

    CHECK(stream.available() == 19);
    CHECK(stream.tell() == 1);

    stream.seek(22, io::Seekable::CURRENT);
    stream.seek(0, io::Seekable::START);

    stream.reset();
    CHECK(stream.available() == 0);
    stream.write("test");
    CHECK(stream.available() == 4);
    sys::byte buf[255];
    stream.read(buf, 4);
    buf[4] = 0;
    CHECK(std::string(buf) == "test");
}

TEST_CASE("testByteStream")
{
    io::ByteStream stream;
    stream.writeln("test");
    stream.writeln("test");

    CHECK(stream.available() == 0);
    stream.seek(0, io::Seekable::START);
    CHECK(stream.available() == 10);

    stream.seek(10, io::Seekable::START);
    stream.write("0123456789");
    CHECK(stream.tell() == 20);
    CHECK(stream.available() == 0);

    stream.seek(22, io::Seekable::CURRENT);
    CHECK(stream.tell() == -1);
    stream.reset();
    CHECK(stream.tell() == 0);
    stream.seek(0, io::Seekable::START);
    CHECK(stream.tell() == 0);

    stream.seek(2, io::Seekable::END);
    CHECK(stream.tell() == 18);
    CHECK(std::ssize(stream) == 20);

    stream.write("abcdef");
    CHECK(std::ssize(stream) == 24);

     const std::string test("test");
    {
        stream.clear();
        CHECK(stream.available() == 0);
        stream.write(test);
        stream.seek(0, io::Seekable::START);
        CHECK(stream.available() == 4);
        sys::byte buf[255];
        stream.read(buf, 4);
        buf[4] = 0;
        CHECK(std::string(buf) == test);
    }
    {
        stream.clear();
        const std::span<const std::string::value_type> test_span(test.data(), test.size());
        stream.write(test_span);
        stream.seek(0, io::Seekable::START);
        CHECK(stream.available() == 4);
        std::byte buf[255];
        stream.read(std::span<std::byte>(buf, 4));
        buf[4] = std::byte(0);
        const void* pBuf = buf;
        auto pStrBuf = static_cast<std::string::const_pointer>(pBuf);
        CHECK(pStrBuf == test);
    }
}

TEST_CASE("testProxyOutputStream")
{
    io::StringStream stream;
    io::ProxyOutputStream proxy(&stream);
    proxy.write("test1");
    sys::byte buf[255];
    stream.read(buf, 5);
    buf[5] = 0;
    CHECK(std::string(buf) == "test1");
}

TEST_CASE("testCountingOutputStream")
{
    io::ByteStream stream;
    io::CountingOutputStream counter(&stream);
    counter.write("test1");
    CHECK(counter.getCount() == 5);
}

TEST_CASE("testBufferViewStream")
{
    {
        mem::BufferView<sys::ubyte> bufferView(nullptr, 0);
        io::BufferViewStream<sys::ubyte> stream(bufferView);
        CHECK(stream.tell() == 0);
        CHECK(stream.available() == 0);
        CHECK(stream.get() == nullptr);
    }
    {
        std::vector<sys::ubyte> data(4);
        data[0] = 2;
        data[1] = 4;
        data[2] = 5;
        data[3] = 9;
        mem::BufferView<sys::ubyte> bufferView(&data[0], data.size());
        io::BufferViewStream<sys::ubyte> stream(bufferView);
        CHECK(stream.tell() == 0);
        CHECK(stream.available() == 4);

        std::vector<sys::ubyte> output(3);
        auto result = stream.read(&output[0], 2);
        CHECK(result == 2);
        CHECK(stream.tell() == 2);
        CHECK(stream.available() == 2);
        stream.seek(1, io::Seekable::CURRENT);
        result = stream.read(&output[2], 1);
        CHECK(result == 1);
        CHECK(output[0] == 2);
        CHECK(output[1] == 4);
        CHECK(output[2] == 9);

        stream.seek(1, io::Seekable::START);
        stream.write(&output[0], output.size());

        CHECK(data[0] == 2);
        CHECK(data[1] == 2);
        CHECK(data[2] == 4);
        CHECK(data[3] == 9);

        CHECK(stream.available() == 0);
        CHECK_THROWS(stream.write(&data[0], data.size()));

        CHECK_THROWS(stream.seek(-1, io::Seekable::START));
        CHECK_THROWS(stream.seek(-1, io::Seekable::END));
    }
}

TEST_CASE("testBufferViewIntStream")
{
    // Test for datatype with size > 1 to make sure copies are done correctly
    std::vector<int> data(4);
    data[0] = 2;
    data[1] = 4;
    data[2] = 5;
    data[3] = 9;
    mem::BufferView<int> bufferView(&data[0], data.size());
    io::BufferViewStream<int> stream(bufferView);
    std::vector<int> output(3);

    auto result = stream.read(&output[0], 2);
    CHECK(result == 2);
    CHECK(stream.tell() == static_cast<sys::Off_T>(2 * sizeof(int)));
    CHECK(stream.available() == static_cast<sys::Off_T>(2 * sizeof(int)));
    stream.seek(1 * sizeof(int), io::Seekable::CURRENT);
    result = stream.read(&output[2], 1);
    CHECK(result == 1);
    CHECK(output[0] == 2);
    CHECK(output[1] == 4);
    CHECK(output[2] == 9);

    stream.seek(1 * sizeof(int), io::Seekable::START);
    stream.write(&output[0], output.size());

    CHECK(data[0] == 2);
    CHECK(data[1] == 2);
    CHECK(data[2] == 4);
    CHECK(data[3] == 9);

    // Truncate properly if we ask for more elements than there are
    ::memset(&output[0], 0, output.size() * sizeof(output[0]));
    stream.seek(3 * sizeof(int), io::Seekable::START);
    result = stream.read(&output[0], 2);
    CHECK(result == 1);
    CHECK(stream.tell() == static_cast<sys::Off_T>(4 * sizeof(int)));
    CHECK(output[0] == 9);
    CHECK(output[1] == 0);
}

void cleanupFiles(std::string base)
{
    // cleanup
    sys::OS os;
    for (size_t i = 0;; ++i)
    {
        std::ostringstream oss;
        oss << base << "." << (i + 1);
        std::string fname(oss.str());
        if (os.isFile(fname))
            os.remove(fname);
        else
            break;
    }
    if (os.isFile(base))
        os.remove(base);
}

TEST_CASE("testRotate")
{
    std::string outFile = "test_rotate.txt";
    size_t maxFiles = 5;

    cleanupFiles( outFile);

    sys::OS os;

    {
        io::RotatingFileOutputStream out(outFile, 10, maxFiles);
        out.write("0123456789");
        CHECK(os.exists(outFile));
        CHECK_FALSE(os.isFile(outFile + ".1"));

        out.write("1");
        CHECK(os.isFile(outFile + ".1"));
        CHECK(out.getCount() == 1);

        for(size_t i = 0; i < maxFiles - 1; ++i)
        {
            std::string fname = outFile + "." + std::to_string(i + 1);
            std::string next = outFile + "." + std::to_string(i + 2);

            CHECK(os.isFile(fname));
            CHECK_FALSE(os.isFile(next));

            out.write("0123456789");
            CHECK(os.isFile(next));
        }
    }

    cleanupFiles( outFile);
}

TEST_CASE("testNeverRotate")
{
    std::string outFile = "test_rotate.txt";
    cleanupFiles( outFile);

    sys::OS os;
    {
        io::RotatingFileOutputStream out(outFile);
        for(size_t i = 0; i < 1024; ++i)
        out.write("0");
        CHECK(os.exists(outFile));
        CHECK_FALSE(os.isFile(outFile + ".1"));
        CHECK(out.getCount() == 1024);
    }
    cleanupFiles( outFile);
}

TEST_CASE("testRotateReset")
{
    std::string outFile = "test_rotate.txt";
    cleanupFiles( outFile);

    sys::OS os;
    io::RotatingFileOutputStream out(outFile, 10);
    out.write("01234567890");
    CHECK(os.exists(outFile));
    CHECK_FALSE(os.isFile(outFile + ".1"));
    CHECK(out.getCount() == 11);

    out.write("0");
    CHECK(os.exists(outFile));
    CHECK_FALSE(os.isFile(outFile + ".1"));
    CHECK(out.getCount() == 1);

    out.close();
    CHECK_THROWS(out.write("0"));

    cleanupFiles( outFile);
}
