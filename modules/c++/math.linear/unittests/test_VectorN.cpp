/* =========================================================================
 * This file is part of math.linear-c++ 
 * =========================================================================
 * 
 * © 2012 MDA Information Systems LLC
 *
 * math.linear-c++ is free software; you can redistribute it and/or modify
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

#include <cstdlib>
#include "TestCase.h"
#include "math/linear/VectorN.h"

using namespace math::linear;

TEST_CASE("testDefaultConstructor")
{
    // Solaris doesn't like arrays of zero size unless you use -features=zla,
    // so I've omitted this test.
    //VectorN<0, double> v0;
    //TEST_ASSERT_EQ(v0.size(), 0);

    VectorN<1,double> v1{};
    TEST_ASSERT_EQ(v1.size(), static_cast<size_t>(1));

    VectorN<2,double> v2{};
    TEST_ASSERT_EQ(v2.size(), static_cast<size_t>(2));
}


TEST_CASE("testScalarConstructor")
{
    VectorN<3,double> v(42);
    TEST_ASSERT_EQ(v.size(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(v[0], 42);
    TEST_ASSERT_EQ(v[1], 42);
    TEST_ASSERT_EQ(v[2], 42);
}


TEST_CASE("testRawConstructor")
{
    double raw[] = {1,2,3};
    VectorN<3,double> v(raw);
    TEST_ASSERT_EQ(v.size(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(v[0], 1);
    TEST_ASSERT_EQ(v[1], 2);
    TEST_ASSERT_EQ(v[2], 3);
}


TEST_CASE("testCopyConstructor")
{
    VectorN<3,double> vsrc;
    vsrc[0] = 1;
    vsrc[1] = 2;
    vsrc[2] = 3;
    VectorN<3,double> v(vsrc);
    TEST_ASSERT_EQ(v.size(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(v[0], vsrc[0]);
    TEST_ASSERT_EQ(v[1], vsrc[1]);
    TEST_ASSERT_EQ(v[2], vsrc[2]);

    vsrc[0] = -1;
    TEST_ASSERT_EQ(vsrc[0], -1); // should change
    TEST_ASSERT_EQ(v[0], 1); // shouldn't change
}


TEST_CASE("testStdVectorConstructor")
{
    std::vector<double> stdvec{10, 11, 12};

    VectorN<3,double> v(stdvec);
    TEST_ASSERT_EQ(v.size(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(v[0], stdvec[0]);
    TEST_ASSERT_EQ(v[1], stdvec[1]);
    TEST_ASSERT_EQ(v[2], stdvec[2]);

    stdvec[0] *= -1;
    stdvec[1] *= -1;
    stdvec[2] *= -1;

    TEST_ASSERT_EQ(v[0], 10);
    TEST_ASSERT_EQ(v[1], 11);
    TEST_ASSERT_EQ(v[2], 12);
}


TEST_CASE("testAssignmentOperator")
{
    VectorN<2,double> vsrc;
    vsrc[0] = 42;
    vsrc[1] = 99;
    VectorN<2,double> v;
    v = vsrc;
    TEST_ASSERT_EQ(v.size(), static_cast<size_t>(2));
    TEST_ASSERT_EQ(v[0], 42);
    TEST_ASSERT_EQ(v[1], 99);
}


TEST_CASE("testScalarAssignment")
{
    VectorN<5,double> v(123.456);
    TEST_ASSERT_EQ(v.size(), static_cast<size_t>(5));
    for (int i = 0; i < 5; i++)
        TEST_ASSERT_EQ(v[i], 123.456);

    // UNLIKE Vector, scalar assignment doesn't change the size of the
    // vector - that's fixed.  Instead, it sets every entry in the vector
    // to the given value.
    v = 99;
    TEST_ASSERT_EQ(v.size(), static_cast<size_t>(5));
    for (int i = 0; i < 5; i++)
        TEST_ASSERT_EQ(v[i], 99);
}


TEST_CASE("testStdVectorAssignment")
{
    const std::vector<double> stdvec{10, 11, 12};

    VectorN<3,double> v(-1);
    v = stdvec;
    TEST_ASSERT_EQ(v.size(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(v[0], 10);
    TEST_ASSERT_EQ(v[1], 11);
    TEST_ASSERT_EQ(v[2], 12);
}


TEST_CASE("testDotProduct")
{
    VectorN<5,double> vd1(2);
    VectorN<5,double> vd2(3);
    const double dotprod(vd1.dot(vd2));
    TEST_ASSERT_EQ(dotprod, 2*3*5);
}


TEST_CASE("testNorm")
{
    const std::vector<double> stdvec{10, 11, 12};

    VectorN<3,double> vnorm1(stdvec);
    const double norm(vnorm1.norm());
    const double arg(10.*10 + 11.*11 + 12.*12);
    const double expectedValue(::sqrt(arg));
    TEST_ASSERT_EQ(norm, expectedValue);
}


TEST_CASE("testNormalize")
{
    VectorN<4,double> vnorm(4.0);
    vnorm.normalize();
    TEST_ASSERT_EQ(vnorm[0], 0.5);
    TEST_ASSERT_EQ(vnorm[1], 0.5);
    TEST_ASSERT_EQ(vnorm[2], 0.5);
    TEST_ASSERT_EQ(vnorm[3], 0.5);
}


TEST_CASE("testScale")
{
    VectorN<2,double> vscale(4.0);
    vscale.scale(1.0 / 4.0);
    TEST_ASSERT_EQ(vscale[0], 1.0);
    TEST_ASSERT_EQ(vscale[1], 1.0);
}

TEST_CASE("testUnit")
{
    VectorN<4,double> v(4.0);
    VectorN<4,double> vunit = v.unit();
    TEST_ASSERT_EQ(v[0], 4.0);
    TEST_ASSERT_EQ(v[1], 4.0);
    TEST_ASSERT_EQ(v[2], 4.0);
    TEST_ASSERT_EQ(v[3], 4.0);
    TEST_ASSERT_EQ(vunit[0], 0.5);
    TEST_ASSERT_EQ(vunit[1], 0.5);
    TEST_ASSERT_EQ(vunit[2], 0.5);
    TEST_ASSERT_EQ(vunit[3], 0.5);
}

TEST_CASE("testOperatorPlusEquals")
{
    VectorN<3,double> v1(1);
    VectorN<3,double> v2(-1);

    v2 += v1;

    TEST_ASSERT_EQ(v1.size(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(v2.size(), static_cast<size_t>(3));

    TEST_ASSERT_EQ(v2[0], 0);
    TEST_ASSERT_EQ(v2[1], 0);
    TEST_ASSERT_EQ(v2[2], 0);

    TEST_ASSERT_EQ(v1[0], 1);
    TEST_ASSERT_EQ(v1[1], 1);
    TEST_ASSERT_EQ(v1[2], 1);
}


TEST_CASE("testOperatorPlus")
{
    VectorN<3,double> v1(42);
    VectorN<3,double> v2(11);
    VectorN<3,double> v3;

    v3 = v1 + v2;

    TEST_ASSERT_EQ(v1.size(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(v2.size(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(v3.size(), static_cast<size_t>(3));

    TEST_ASSERT_EQ(v1[0], 42);
    TEST_ASSERT_EQ(v1[1], 42);
    TEST_ASSERT_EQ(v1[2], 42);

    TEST_ASSERT_EQ(v2[0], 11);
    TEST_ASSERT_EQ(v2[1], 11);
    TEST_ASSERT_EQ(v2[2], 11);

    TEST_ASSERT_EQ(v3[0], 53);
    TEST_ASSERT_EQ(v3[1], 53);
    TEST_ASSERT_EQ(v3[2], 53);
}


TEST_CASE("testOperatorMinusEquals")
{
    VectorN<5,double> v1(13);
    VectorN<5,double> v2(-5);

    v2 -= v1;
    TEST_ASSERT_EQ(v1.size(), static_cast<size_t>(5));
    TEST_ASSERT_EQ(v2.size(), static_cast<size_t>(5));
    for (int i = 0; i < 5; i++)
        TEST_ASSERT_EQ(v2[i], -18);
    for (int i = 0; i < 5; i++)
        TEST_ASSERT_EQ(v1[i], 13);
}


TEST_CASE("testNegate")
{
    VectorN<3,double> X;
    VectorN<3,double> Y;
    X[0] = Y[2] =  1.;
    X[1] = Y[1] =  0.;
    X[2] = Y[0] = -1.;
    TEST_ASSERT_EQ(X, -Y);
}


TEST_CASE("testAdd")
{
    VectorN<3,double> v1(2.4);
    VectorN<3,double> v2(1.4);

    VectorN<3,double> v3(v2.add(v1));

    TEST_ASSERT_EQ(v1.size(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(v2.size(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(v3.size(), static_cast<size_t>(3));

    TEST_ASSERT_EQ(v1[0], 2.4);
    TEST_ASSERT_EQ(v1[1], 2.4);
    TEST_ASSERT_EQ(v1[2], 2.4);

    TEST_ASSERT_EQ(v2[0], 1.4);
    TEST_ASSERT_EQ(v2[1], 1.4);
    TEST_ASSERT_EQ(v2[2], 1.4);

    TEST_ASSERT_EQ(v3[0], 3.8);
    TEST_ASSERT_EQ(v3[1], 3.8);
    TEST_ASSERT_EQ(v3[2], 3.8);
}


TEST_CASE("testSubtract")
{
    VectorN<3,double> v1(2.4);
    VectorN<3,double> v2(1.4);

    VectorN<3,double> v3(v2.subtract(v1));

    TEST_ASSERT_EQ(v1.size(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(v2.size(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(v3.size(), static_cast<size_t>(3));

    TEST_ASSERT_EQ(v1[0], 2.4);
    TEST_ASSERT_EQ(v1[1], 2.4);
    TEST_ASSERT_EQ(v1[2], 2.4);

    TEST_ASSERT_EQ(v2[0], 1.4);
    TEST_ASSERT_EQ(v2[1], 1.4);
    TEST_ASSERT_EQ(v2[2], 1.4);

    TEST_ASSERT_EQ(v3[0], -1);
    TEST_ASSERT_EQ(v3[1], -1);
    TEST_ASSERT_EQ(v3[2], -1);
}


TEST_CASE("testOperatorMinus")
{
    VectorN<4,double> v1;
    VectorN<4,double> v2;
    for (int i = 0; i < 4; i++)
    {
        v1[i] = i;
        v2[i] = -i;
    }
    VectorN<4,double> v3(v2 - v1);

    TEST_ASSERT_EQ(v1.size(), static_cast<size_t>(4));
    TEST_ASSERT_EQ(v2.size(), static_cast<size_t>(4));
    TEST_ASSERT_EQ(v3.size(), static_cast<size_t>(4));

    for (int i = 0; i < 4; i++)
    {
        TEST_ASSERT_EQ(v1[i], i);
        TEST_ASSERT_EQ(v2[i], -i);
    }

    for (int i = 0; i < 4; i++)
        TEST_ASSERT_EQ(v3[i], -2 * i);
}


TEST_CASE("testOperatorTimesEquals")
{
    VectorN<4,double> v1;
    for (int i = 0; i < 4; i++)
        v1[i] = i;
    VectorN<4,double> v2;
    for (int i = 0; i < 4; i++)
        v2[i] = -i;

    v2 *= v1;

    for (int i = 0; i < 4; i++)
        TEST_ASSERT_EQ(v1[i], i);
    for (int i = 0; i < 4; i++)
        TEST_ASSERT_EQ(v2[i], -i * i);
}


TEST_CASE("testOperatorTimesEqualsScalar")
{
    VectorN<5,double> v1;
    for (int i = 0; i < 5; i++)
        v1[i] = i;

    v1 *= 3;

    for (int i = 0; i < 5; i++)
        TEST_ASSERT_EQ(v1[i], 3 * i);
}


TEST_CASE("testOperatorTimesScalar")
{
    VectorN<5,double> v1;
    for (int i = 0; i < 5; i++)
        v1[i] = i;

    VectorN<5,double> v2(v1 * 2);

    for (int i = 0; i < 5; i++)
        TEST_ASSERT_EQ(v1[i], i);
    for (int i = 0; i < 5; i++)
        TEST_ASSERT_EQ(v2[i], 2 * i);
}

TEST_CASE("testElementDivision")
{
    VectorN<3,double> numerator({0, 1, 2});
    VectorN<3,double> denominator({1, 2, 3});

    VectorN<3,double> quotient(numerator / denominator);
    TEST_ASSERT_EQ(quotient[0], 0);
    TEST_ASSERT_EQ(quotient[1], 1./2.);
    TEST_ASSERT_EQ(quotient[2], 2./3.);
}

TEST_CASE("testOperatorTimes")
{
    VectorN<3,double> v1, v2;
    for (int i = 0; i < 3; i++)
    {
        v1[i] = i;
        v2[i] = i + 2;
    }

    VectorN<3, double> v3(v1 * v2);
    for (int i = 0; i < 3; i++)
        TEST_ASSERT_EQ(v3[i], i * (i + 2));
}

TEST_CASE("testOperatorDivide")
{
    VectorN<3,double> v1, v2;
    for (int i = 0; i < 3; i++)
    {
        v1[i] = i + 1.;
        v2[i] = i + 2.;
    }

    VectorN<3, double> v3(v1 / v2);
    for (int i = 0; i < 3; i++)
        TEST_ASSERT_EQ(v3[i], (i + 1.) / (i + 2.));
}

TEST_CASE("testMatVecProds")
{
    MatrixMxN<3, 3, double> matrix({1, 0, 1,
                                    0, 2, 0,
                                    3, 0, 3});
    VectorN<3, double> vec({1, 1, 1});

    VectorN<3, double> r_vm({4, 2, 4});
    VectorN<3, double> r_mv({2, 2, 6});

    TEST_ASSERT_EQ(vec * matrix, r_vm);
    TEST_ASSERT_EQ(matrix * vec, r_mv);
}
