/***************************  simd_alg.h   *********************************
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
	template<size_t Bytes, typename Abi, typename T, typename U>
	constexpr auto simd_select(const basic_simd_mask<Bytes, Abi>& c, const basic_simd<T, Abi>& a, const basic_simd<U, Abi>& b) noexcept
	{
		return simd_select_impl(c, a, b);
	}
	template<size_t Bytes, typename Abi, typename T, typename U>
	constexpr auto simd_select(const basic_simd_mask<Bytes, Abi>& c, const T& a, const U& b) noexcept
	{
		return simd_select_impl(c, basic_simd<T, Abi>(a), basic_simd<U, Abi>(b));
	}

	template<size_t Bytes, typename Abi>
	constexpr bool any_of(const basic_simd_mask<Bytes, Abi>& m) noexcept
	{
		using Vec_b = basic_simd_mask<Bytes, Abi>::native_type;
		return horizontal_or(static_cast<Vec_b>(m));
	}

	template<size_t Bytes, typename Abi>
	constexpr bool all_of(const basic_simd_mask<Bytes, Abi>& m) noexcept
	{
		using Vec_b = basic_simd_mask<Bytes, Abi>::native_type;
		return horizontal_and(static_cast<Vec_b>(m));
	}
}

#ifdef VCL_NAMESPACE
}
#endif