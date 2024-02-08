/***************************  simd_math.h   *********************************
* Author:        Agner Fog
* Date created:  2024-02-02
* Last modified: 2024-02-02
* Version:       2.02.00
* Project:       Extension to vector class library
* Description:
* Allow client code using the vector class library to look more like it was using `std::simd::simd`.
* https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p1928r8.pdf
*
* © Copyright 2012-2024. Apache License version 2.0 or later
******************************************************************************/
#pragma once

#include "simd.h"

#ifdef VCL_NAMESPACE
namespace VCL_NAMESPACE {
#endif

namespace simd
{
	template<typename T, typename Abi>
	inline basic_simd<T, Abi> round(const basic_simd<T, Abi>& v)
	{
		using Vec = typename basic_simd<T, Abi>::native_type;
		return round(static_cast<Vec>(v));
	}

	template<typename T, typename Abi>
	inline basic_simd<int, Abi> lround(const basic_simd<T, Abi>& v)
	{
		using Vec = typename basic_simd<T, Abi>::native_type;
		return roundi(static_cast<Vec>(v));
	}

	template<typename T, typename Abi>
	inline basic_simd<T, Abi> atan2(const basic_simd<T, Abi>& y, const basic_simd<T, Abi>& x)
	{
		using Vec = typename basic_simd<T, Abi>::native_type;
		return atan2(static_cast<Vec>(y), static_cast<Vec>(x));
	}
}

#ifdef VCL_NAMESPACE
}
#endif