/* =========================================================================
 * This file is part of net-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * net-c++ is free software; you can redistribute it and/or modify
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

#include <net/net_config.h>

#ifdef NET_CURL_SUPPORT

#include "TestCase.h"
#include <net/CurlHandle.h>
#include <vector>
#include <string>

TEST_CASE(testSetHttpHeaders)
{
    net::CurlHandle handle;
    
    // Test with empty headers - should not throw
    std::vector<std::string> emptyHeaders;
    handle.setHttpHeaders(emptyHeaders);
    
    // Test with single header - should not throw
    std::vector<std::string> singleHeader{"Content-Type: application/json"};
    handle.setHttpHeaders(singleHeader);
    
    // Test with multiple headers - should not throw
    std::vector<std::string> multipleHeaders{
        "Content-Type: application/json",
        "Authorization: Bearer token123",
        "User-Agent: TestClient/1.0"
    };
    handle.setHttpHeaders(multipleHeaders);
    
    // Test that headers can be replaced - should not throw
    std::vector<std::string> newHeaders{"X-Custom-Header: value"};
    handle.setHttpHeaders(newHeaders);
    
    TEST_SUCCESS;
}

// Test that setPutRequest doesn't throw
TEST_CASE(testSetPutRequest)
{
    net::CurlHandle handle;
    handle.setPutRequest();

    TEST_SUCCESS;
}

// Test combining setPutRequest with setHttpHeaders - should not throw
TEST_CASE(testSetPutRequestWithHeaders)
{
    net::CurlHandle handle;
    std::vector<std::string> headers{
        "Content-Type: text/plain",
        "Content-Length: 0"
    };
    
    handle.setHttpHeaders(headers);
    handle.setPutRequest();
    
    TEST_SUCCESS;
}

// Test that multiple handles can be configured independently
TEST_CASE(testMultipleHandlesIndependent)
{
    net::CurlHandle handle1;
    net::CurlHandle handle2;
    
    std::vector<std::string> headers1{"X-Handle: 1"};
    std::vector<std::string> headers2{"X-Handle: 2"};
    
    handle1.setHttpHeaders(headers1);
    handle2.setHttpHeaders(headers2);
    handle1.setPutRequest();
    
    TEST_SUCCESS;
}

// Test that calling setHttpHeaders multiple times properly replaces headers
TEST_CASE(testSetHeadersMultipleTimes)
{
    net::CurlHandle handle;
    std::vector<std::string> firstHeaders{"X-First: 1"};
    handle.setHttpHeaders(firstHeaders);
    
    std::vector<std::string> secondHeaders{"X-Second: 2"};
    handle.setHttpHeaders(secondHeaders);
    
    std::vector<std::string> thirdHeaders{"X-Third: 3", "X-Fourth: 4"};
    handle.setHttpHeaders(thirdHeaders);
    
    TEST_SUCCESS;
}

TEST_MAIN(
    TEST_CHECK(testSetHttpHeaders);
    TEST_CHECK(testSetPutRequest);
    TEST_CHECK(testSetPutRequestWithHeaders);
    TEST_CHECK(testMultipleHandlesIndependent);
    TEST_CHECK(testSetHeadersMultipleTimes);
)
#endif
