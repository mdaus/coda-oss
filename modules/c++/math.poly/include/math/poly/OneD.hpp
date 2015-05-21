/* =========================================================================
 * This file is part of math.poly-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * math.poly-c++ is free software; you can redistribute it and/or modify
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

#include <cmath>
#include <import/except.h>
#include <import/sys.h>
#include <math/poly/Utils.h>

namespace math
{
namespace poly
{

template<typename _T>
_T 
OneD<_T>::operator () (double at) const
{
   _T ret(0.0);
   double atPwr = 1.0;
   for (size_t i = 0, sz = mCoef.size() ; i < sz; i++)
   {
      ret += mCoef[i]*atPwr;
      atPwr *= at;
   }
   return ret;
}

template<typename _T>
_T 
OneD<_T>::integrate(double start, double end) const
{
   _T ret(0.0);
   double endAtPwr = end;
   double startAtPwr = start;
   double div = 0;
   double newCoef;
   for (size_t i = 0, sz = mCoef.size(); i < sz; i++)
   {
      div = 1.0 / (i + 1);
      newCoef = mCoef[i] * div;
      ret += newCoef * endAtPwr;
      ret -= newCoef * startAtPwr;
      endAtPwr *= end;
      startAtPwr *= start;
   }
   return ret;
}

template<typename _T>
OneD<_T>
OneD<_T>::derivative() const
{
    OneD<_T> ret(0);
    if (order() > 0)
    {
        ret = OneD<_T>(order()-1);
        for (size_t ii = 0, sz = mCoef.size() - 1; ii < sz; ii++)
        {
            ret[ii] = static_cast<_T>(mCoef[ii + 1] * (ii + 1));
        }
    }
    return ret;
}
template<typename _T>
_T
OneD<_T>::velocity(double x) const
{
    _T vel = derivative()(x);
    return vel;
}
template<typename _T>
_T
OneD<_T>::acceleration(double x) const
{
    _T acc = ((derivative()).derivative())(x);
    return acc;
}
template<typename _T>
_T& 
OneD<_T>::operator [] (size_t i)
{
    if (i < mCoef.size())
    {
        return (mCoef[i]);
    }
    else
    {
        std::stringstream str;
        str << "index: " << i << " not within range [0..."
            << mCoef.size() << ")";
        std::string msg(str.str());
        throw except::IndexOutOfRangeException(Ctxt(msg));
    }
}


template<typename _T>
_T 
OneD<_T>::operator [] (size_t i) const
{
   _T ret(0.0);
   if (i < mCoef.size())
   {
      ret = mCoef[i];
   }
   else
   {
      std::stringstream str;
      str << "idx(" << i << ") not within range [0..." << mCoef.size() << ")";
      std::string msg(str.str());
      throw except::IndexOutOfRangeException(Ctxt(msg));
   }
   return ret;
}

template<typename _T>
std::ostream&
operator << (std::ostream& out, const OneD<_T>& p)
{
   for (size_t i = 0 ; i < p.mCoef.size() ; i++)
   {
      out << p[i] << "*y^" << i << " ";
   }
   return out;
}

template<typename _T>
OneD<_T>&
OneD<_T>::operator *= (double cv)
{
    for (size_t i = 0, sz = mCoef.size() ; i < sz; i++)
    {
        mCoef[i] *= cv;
    }
    return *this;
}
    
template<typename _T>
OneD<_T>
OneD<_T>::operator * (double cv) const
{
    OneD<_T> ret(*this);
    ret *= cv;
    return ret;
}

template<typename _T>
OneD<_T>
operator * (double cv, const OneD<_T>& p)
{
    return p*cv;
}

template<typename _T>
OneD<_T>&
OneD<_T>::operator *= (const OneD<_T>& p)
{
   OneD<_T> tmp(order()+p.order());
   for (size_t i = 0, xsz = mCoef.size() ; i < xsz; i++)
   {
       for (unsigned int j = 0, ysz = p.mCoef.size() ; j < ysz; j++)
       {
           tmp.mCoef[i + j] += mCoef[i] * p.mCoef[j];
       }
   }
   *this = tmp;
   return *this;
}
    
template<typename _T>
OneD<_T>
OneD<_T>::operator * (const OneD<_T>& p) const
{
    OneD<_T> ret(*this);
    ret *= p;
    return ret;
}

template<typename _T>
OneD<_T>&
OneD<_T>::operator += (const OneD<_T>& p)
{
    OneD<_T> tmp(std::max<size_t>(order(), p.order()));
    for (size_t i = 0, sz = mCoef.size() ; i < sz; i++)
    {
        tmp.mCoef[i] = mCoef[i];
    }
    for (size_t i = 0, sz = p.mCoef.size() ; i < sz; i++)
    {
        tmp.mCoef[i] += p.mCoef[i];
    }
   *this = tmp;
   return *this;
}

template<typename _T>
OneD<_T>
OneD<_T>::operator + (const OneD<_T>& p) const
{
    OneD<_T> ret(*this);
    ret += p;
    return ret;
}
    
template<typename _T>
OneD<_T>&
OneD<_T>::operator -= (const OneD<_T>& p)
{
   OneD<_T> tmp(std::max<size_t>(order(), p.order()));
   for (unsigned int i = 0, sz = mCoef.size() ; i < sz; i++)
   {
       tmp.mCoef[i] = mCoef[i];
   }
   for (unsigned int i = 0, sz = p.mCoef.size(); i < sz; i++)
   {
       tmp.mCoef[i] -= p.mCoef[i];
   }
   *this = tmp;
   return *this;
}

template<typename _T>
OneD<_T>
OneD<_T>::operator - (const OneD<_T>& p) const
{
   OneD<_T> ret(*this);
   ret -= p;
   return ret;
}

template<typename _T>
OneD<_T>&
OneD<_T>::operator /= (double cv)
{
    double recipCV = 1.0/cv;
    for (unsigned int i = 0, sz = mCoef.size() ; i < sz; i++)
    {
        mCoef[i] *= recipCV;
    }
    return *this;
}

template<typename _T>
OneD<_T>
OneD<_T>::operator / (double cv) const
{
    OneD<_T> ret(*this);
    ret *= (1.0/cv);
    return ret;
}

template<typename _T>
OneD<_T> OneD<_T>::power(size_t toThe) const
{
    // If its 0, we have to give back a 1*x^0 poly, since
    // we want a 2D poly out
    if (toThe == 0)
    {
        OneD<_T> zero(0);
        zero[0] = 1;
        return zero;
    }

    OneD<double> rv = *this;

    // If its 1 give it back now
    if (toThe == 1)
        return rv;

 
    // Otherwise, we have to raise it   
    for (size_t i = 2; i <= toThe; i++)
    {
        rv *= *this;
    }
    return rv;
}

template<typename _T>
OneD<_T> OneD<_T>::scaleVariable(double scale) const
{
    return ::math::poly::scaleVariable<OneD<_T> >(*this, scale);
}

template<typename _T>
OneD<_T> OneD<_T>::truncateTo(size_t order) const
{
    order = std::min(this->order(), order);

    OneD<_T> newP(order);
    for (size_t ii = 0; ii <= order; ++ii)
    {
        newP[ii] = (*this)[ii];
    }
    return newP;
}

template<typename _T>
OneD<_T> OneD<_T>::truncateToNonZeros(double zeroEpsilon) const
{
    zeroEpsilon = std::abs(zeroEpsilon);
    size_t newOrder(0);

    // Find the highest order non-zero coefficient
    for (size_t ii = 0, idx = order(); ii <= order(); ++ii, --idx)
    {
        if (std::abs((*this)[idx]) > zeroEpsilon)
        {
            newOrder = idx;
            break;
        }
    }

    return truncateTo(newOrder);
}

template<typename _T>
OneD<_T> OneD<_T>::transformInput(const OneD<_T>& gx,
                                  double zeroEpsilon) const
{
    OneD<_T> newP(order());

    for (size_t ii = 0; ii <= order(); ++ii)
    {
        newP += gx.power(ii) * (*this)[ii];
    }

    return newP.truncateToNonZeros(zeroEpsilon);
}

template<typename _T>
void OneD<_T>::copyFrom(const OneD<_T>& p)
{
    const size_t numCopy(std::min(size(), p.size()));
    std::copy(p.mCoef.begin(), p.mCoef.begin() + numCopy, mCoef.begin());
}

} // poly
} // math



