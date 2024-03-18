/***************************  simd_traits.h   *********************************
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
	template<typename T, typename V> class rebind_simd
	{
		// "`V` is ... `basic_simd<U, Abi0>` ... where `U`, ..., and `Abi0` are deduced from `V` ..."
		using U = typename V::value_type;
		using Abi0 = typename V::abi_type;
		// "Let `Abi1` denote the type `deduce_t<T, basic_simd<U, Abi0>::size, Abi0>`"
		using Abi1 = typename details::simd_abi::deduce_t<T, basic_simd<U, Abi0>::size(), Abi0>;
	public:
		// "... the member typedef `type` names `basic_simd<T, Abi1>` if `V` is `basic_simd<U, Abi0>` ..."
		using type = basic_simd<T, Abi1>;
	};
	template<typename T, typename V>
	using rebind_simd_t = typename rebind_simd<T, V>::type;

	template<details::size_type N, typename V> class resize_simd
	{
		// "`V` is ... `basic_simd<U, Abi0>` ... where `U`, ..., and `Abi0` are deduced from `V` ..."
		using T = typename V::value_type;
		using Abi0 = typename V::abi_type;
		// "Let `Abi1` denote the type `deduce_t<T, N, Abi0>`"
		using Abi1 = typename details::simd_abi::deduce_t<T, N, Abi0>;
	public:
		// "... the member typedef `type` names `basic_simd<T, Abi1>` if `V` is `basic_simd<U, Abi0>` ..."
		using type = basic_simd<T, Abi1>;
	};
	template<details::size_type N, typename V>
	using resize_simd_t = typename resize_simd<N, V>::type;


}

#ifdef VCL_NAMESPACE
}
#endif
