/***************************  simd_builtin.h   *********************************
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
	constexpr basic_simd<T, Abi>& basic_simd<T, Abi>::operator++() noexcept
	{
		++(this->v_);
		return *this;
	}
	template<typename T, typename Abi>
	constexpr basic_simd<T, Abi> basic_simd<T, Abi>::operator++(int) noexcept
	{
		auto old = *this; // copy old value
		operator++();  // prefix increment
		return old;    // return old value
	}
	template<typename T, typename Abi>
	constexpr basic_simd<T, Abi>& basic_simd<T, Abi>::operator--() noexcept
	{
		--(this->v_);
		return *this;
	}
	template<typename T, typename Abi>
	constexpr basic_simd<T, Abi> basic_simd<T, Abi>::operator--(int) noexcept
	{
		auto old = *this; // copy old value
		operator--();  // prefix decrement
		return old;    // return old value
	}
	template<typename T, typename Abi>
	constexpr basic_simd<T, Abi>::mask_type basic_simd<T, Abi>::operator!() const noexcept
	{
		return !(this->v_);
	}
	template<typename T, typename Abi>
	constexpr basic_simd<T, Abi> basic_simd<T, Abi>::operator~() const noexcept
	{
		return ~(this->v_);
	}
	template<typename T, typename Abi>
	constexpr basic_simd<T, Abi> basic_simd<T, Abi>::operator+() const noexcept
	{
		return +(this->v_);
	}
	template<typename T, typename Abi>
	constexpr basic_simd<T, Abi> basic_simd<T, Abi>::operator-() const noexcept
	{
		return -(this->v_);
	}

	#define VECTORCLASS_basic_simd_binary(OPERATOR_) \
		template<typename T, typename Abi> constexpr basic_simd<T, Abi> operator OPERATOR_(const basic_simd<T, Abi>& lhs, const basic_simd<T, Abi>& rhs) noexcept { \
			using Vec = basic_simd<T, Abi>::native_type; return static_cast<Vec>(lhs) OPERATOR_ static_cast<Vec>(rhs); } \
		template<typename T, typename Abi, typename U> constexpr basic_simd<T, Abi> operator OPERATOR_(const basic_simd<T, Abi>& lhs, const U& rhs) noexcept { \
			using Vec = basic_simd<T, Abi>::native_type; return static_cast<Vec>(lhs) OPERATOR_ rhs; }
	VECTORCLASS_basic_simd_binary(+);
	VECTORCLASS_basic_simd_binary(-);
	VECTORCLASS_basic_simd_binary(*);
	VECTORCLASS_basic_simd_binary(/);
	VECTORCLASS_basic_simd_binary(%);
	VECTORCLASS_basic_simd_binary(&);
	VECTORCLASS_basic_simd_binary(|);
	VECTORCLASS_basic_simd_binary(^);
	VECTORCLASS_basic_simd_binary(<<);
	VECTORCLASS_basic_simd_binary(>>);
	template<typename T, typename Abi>
	constexpr basic_simd<T, Abi> operator<<(const basic_simd<T, Abi>& lhs, details::size_type rhs) noexcept
	{
		using Vec = basic_simd<T, Abi>::Vec;
		return static_cast<Vec>(lhs) << rhs;
	}
	template<typename T, typename Abi>
	constexpr basic_simd<T, Abi> operator>>(const basic_simd<T, Abi>& lhs, details::size_type rhs) noexcept
	{
		using Vec = basic_simd<T, Abi>::Vec;
		return static_cast<Vec>(lhs) >> rhs;
	}
	#undef VECTORCLASS_basic_simd_binary

	#define VECTORCLASS_basic_simd_cassign(OPERATOR_) \
		template<typename T, typename Abi> constexpr basic_simd<T, Abi>& operator OPERATOR_(basic_simd<T, Abi>& lhs, const basic_simd<T, Abi>& rhs) noexcept { \
			lhs.v_ OPERATOR_ rhs.v_; return lhs; } \
		template<typename T, typename Abi, typename U> constexpr basic_simd<T, Abi>& operator OPERATOR_(basic_simd<T, Abi>& lhs, const U& rhs) noexcept { \
			lhs.v_ OPERATOR_ rhs; return lhs; }
	VECTORCLASS_basic_simd_cassign(+=);
	VECTORCLASS_basic_simd_cassign(-=);
	VECTORCLASS_basic_simd_cassign(*=);
	VECTORCLASS_basic_simd_cassign(/=);
	VECTORCLASS_basic_simd_cassign(%=);
	VECTORCLASS_basic_simd_cassign(&= );
	VECTORCLASS_basic_simd_cassign(|=);
	VECTORCLASS_basic_simd_cassign(^= );
	VECTORCLASS_basic_simd_cassign(<<= );
	VECTORCLASS_basic_simd_cassign(>>= );
	#undef VECTORCLASS_basic_simd_cassign
	template<typename T, typename Abi, typename U>
	constexpr basic_simd<T, Abi>& operator<<=(basic_simd<T, Abi>& lhs, details::size_type rhs) noexcept {
			lhs.v_ <<= rhs;
			return lhs;
	}
	template<typename T, typename Abi, typename U>
	constexpr basic_simd<T, Abi>& operator>>=(basic_simd<T, Abi>& lhs, details::size_type rhs) noexcept {
		lhs.v_ >>= rhs;
		return lhs;
	}

	#define VECTORCLASS_basic_simd_comparison(OPERATOR_) \
		template<typename T, typename Abi> constexpr basic_simd<T, Abi>::mask_type operator OPERATOR_(const basic_simd<T, Abi>& lhs, const basic_simd<T, Abi>& rhs) noexcept { \
			using Vec = basic_simd<T, Abi>::native_type; \
			return static_cast<Vec>(lhs) OPERATOR_ static_cast<Vec>(rhs); } \
		template<typename T, typename Abi, typename U> constexpr basic_simd<T, Abi>::mask_type operator OPERATOR_(const basic_simd<T, Abi>& lhs, const U& rhs) noexcept { \
			using Vec = basic_simd<T, Abi>::native_type; \
			return static_cast<Vec>(lhs) OPERATOR_ rhs; }
	VECTORCLASS_basic_simd_comparison(==);
	VECTORCLASS_basic_simd_comparison(!=);
	VECTORCLASS_basic_simd_comparison(>=);
	VECTORCLASS_basic_simd_comparison(<=);
	VECTORCLASS_basic_simd_comparison(>);
	VECTORCLASS_basic_simd_comparison(<);
	#undef VECTORCLASS_basic_simd_comparison


	#define VECTORCLASS_basic_simd_mask_binary(OPERATOR_) \
		template<size_t Bytes, typename Abi> constexpr basic_simd_mask<Bytes, Abi> \
		operator OPERATOR_(const basic_simd_mask<Bytes, Abi>& lhs, const basic_simd_mask<Bytes, Abi>& rhs) noexcept { \
			using Vec = basic_simd_mask<Bytes, Abi>::native_type; return static_cast<Vec>(lhs) OPERATOR_ static_cast<Vec>(rhs); }
	VECTORCLASS_basic_simd_mask_binary(&&);
	VECTORCLASS_basic_simd_mask_binary(||);
	VECTORCLASS_basic_simd_mask_binary(&);
	VECTORCLASS_basic_simd_mask_binary(|);
	VECTORCLASS_basic_simd_mask_binary(^);
	#undef VECTORCLASS_basic_simd_mask_binary

}

#ifdef VCL_NAMESPACE
}
#endif