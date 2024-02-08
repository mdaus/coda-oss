/***************************  simd_abi.h   *********************************
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

	namespace details
	{
		using size_type = int;

		// See tables 2.1 and 2.2 of https://github.com/vectorclass/manual/raw/master/vcl_manual.pdf
		constexpr size_type detect_vector_size()
		{
			if constexpr (detected_instrset <= instrset::SSE2) return 128;
			if constexpr (detected_instrset <= instrset::AVX2) return 256;
			return 512;
	}
		constexpr auto detected_vector_size_bytes = detect_vector_size() / 8;
		constexpr size_type native_vector_elements(size_t sz)
		{
			return static_cast<size_type>(detected_vector_size_bytes / sz);
		}

		// "a signed integer type T so that sizeof(T) == Bytes."
		template <size_t Bytes> struct integer_from_;
		template<> struct integer_from_<1> { using type = std::int8_t; };
		template<> struct integer_from_<2> { using type = std::int16_t; };
		template<> struct integer_from_<4> { using type = std::int32_t; };
		template<> struct integer_from_<8> { using type = std::int64_t; };
		template <size_t Bytes>
		using integer_from = typename integer_from_<Bytes>::type;

		// VecNt<4, int32_t> == Vec4i
		// https://github.com/vectorclass/manual/raw/master/vcl_manual.pdf
		template <int Elements_per_vector, typename T> struct VecNt;

		// Table 2.1 from https://github.com/vectorclass/manual/raw/master/vcl_manual.pdf
		// 128 Total bits
		template<> struct VecNt<16, int8_t> { using Vector_class = Vec16c; };
		template<> struct VecNt<16, uint8_t> { using Vector_class = Vec16uc; };
		template<> struct VecNt<8, int16_t> { using Vector_class = Vec8s; };
		template<> struct VecNt<8, uint16_t> { using Vector_class = Vec8us; };
		template<> struct VecNt<4, int32_t> { using Vector_class = Vec4i; };
		template<> struct VecNt<4, uint32_t> { using Vector_class = Vec4ui; };
		template<> struct VecNt<2, int64_t> { using Vector_class = Vec2q; };
		template<> struct VecNt<2, uint64_t> { using Vector_class = Vec2uq; };
		// 256 Total bits
		template<> struct VecNt<32, int8_t> { using Vector_class = Vec32c; };
		template<> struct VecNt<32, uint8_t> { using Vector_class = Vec32uc; };
		template<> struct VecNt<16, int16_t> { using Vector_class = Vec16s; };
		template<> struct VecNt<16, uint16_t> { using Vector_class = Vec16us; };
		template<> struct VecNt<8, int32_t> { using Vector_class = Vec8i; };
		template<> struct VecNt<8, uint32_t> { using Vector_class = Vec8ui; };
		template<> struct VecNt<4, int64_t> { using Vector_class = Vec4q; };
		template<> struct VecNt<4, uint64_t> { using Vector_class = Vec4uq; };
		// 512 Total bits
		template<> struct VecNt<64, int8_t> { using Vector_class = Vec64c; };
		template<> struct VecNt<64, uint8_t> { using Vector_class = Vec64uc; };
		template<> struct VecNt<32, int16_t> { using Vector_class = Vec32s; };
		template<> struct VecNt<32, uint16_t> { using Vector_class = Vec32us; };
		template<> struct VecNt<16, int32_t> { using Vector_class = Vec16i; };
		template<> struct VecNt<16, uint32_t> { using Vector_class = Vec16ui; };
		template<> struct VecNt<8, int64_t> { using Vector_class = Vec8q; };
		template<> struct VecNt<8, uint64_t> { using Vector_class = Vec8uq; };

		// Table 2.2 from https://github.com/vectorclass/manual/raw/master/vcl_manual.pdf
		// 128 Total bits
		template<> struct VecNt<4, float> { using Vector_class = Vec4f; };
		template<> struct VecNt<2, double> { using Vector_class = Vec2d; };
		// 256 Total bits
		template<> struct VecNt<8, float> { using Vector_class = Vec8f; };
		template<> struct VecNt<4, double> { using Vector_class = Vec4d; };
		// 512 Total bits
		template<> struct VecNt<16, float> { using Vector_class = Vec16f; };
		template<> struct VecNt<8, double> { using Vector_class = Vec8d; };

		// Vec<4, int32_t> = Vec4i
		template <int Elements_per_vector, typename T>
		using Vec = typename VecNt<Elements_per_vector, T>::Vector_class;


		// See table 2.3 of https://github.com/vectorclass/manual/raw/master/vcl_manual.pdf
		template <typename For_use_with> struct VecNb;
		// 128 Total bits
		template<> struct VecNb<Vec16c> { using Boolean_vector_class = Vec16cb; };
		template<> struct VecNb<Vec16uc> { using Boolean_vector_class = Vec16cb; };
		template<> struct VecNb<Vec8s> { using Boolean_vector_class = Vec8sb; };
		template<> struct VecNb<Vec8us> { using Boolean_vector_class = Vec8sb; };
		template<> struct VecNb<Vec4i> { using Boolean_vector_class = Vec4ib; };
		template<> struct VecNb<Vec4ui> { using Boolean_vector_class = Vec4ib; };
		template<> struct VecNb<Vec2q> { using Boolean_vector_class = Vec2qb; };
		template<> struct VecNb<Vec2uq> { using Boolean_vector_class = Vec2qb; };
		template<> struct VecNb<Vec4f> { using Boolean_vector_class = Vec4fb; };
		template<> struct VecNb<Vec2d> { using Boolean_vector_class = Vec2db; };
		// 256 Total bits
		template<> struct VecNb<Vec32c> { using Boolean_vector_class = Vec32cb; };
		template<> struct VecNb<Vec32uc> { using Boolean_vector_class = Vec32cb; };
		template<> struct VecNb<Vec16s> { using Boolean_vector_class = Vec16sb; };
		template<> struct VecNb<Vec16us> { using Boolean_vector_class = Vec16sb; };
		template<> struct VecNb<Vec8i> { using Boolean_vector_class = Vec8ib; };
		template<> struct VecNb<Vec8ui> { using Boolean_vector_class = Vec8ib; };
		template<> struct VecNb<Vec4q> { using Boolean_vector_class = Vec4qb; };
		template<> struct VecNb<Vec4uq> { using Boolean_vector_class = Vec4qb; };
		template<> struct VecNb<Vec8f> { using Boolean_vector_class = Vec8fb; };
		template<> struct VecNb<Vec4d> { using Boolean_vector_class = Vec4db; };
		// 512 Total bits
		template<> struct VecNb<Vec64c> { using Boolean_vector_class = Vec64cb; };
		template<> struct VecNb<Vec64uc> { using Boolean_vector_class = Vec64cb; };
		template<> struct VecNb<Vec32s> { using Boolean_vector_class = Vec32sb; };
		template<> struct VecNb<Vec32us> { using Boolean_vector_class = Vec32sb; };
		template<> struct VecNb<Vec16i> { using Boolean_vector_class = Vec16ib; };
		template<> struct VecNb<Vec16ui> { using Boolean_vector_class = Vec16ib; };
		template<> struct VecNb<Vec8q> { using Boolean_vector_class = Vec8qb; };
		template<> struct VecNb<Vec8uq> { using Boolean_vector_class = Vec8qb; };
		template<> struct VecNb<Vec16f> { using Boolean_vector_class = Vec16fb; };
		template<> struct VecNb<Vec8d> { using Boolean_vector_class = Vec8db; };

		// Boolean_vector_class<4, i> = Vec4ib
		template <int Elements_per_vector, typename T>
		using Boolean_vector_class = typename VecNb<Vec<Elements_per_vector, T>>::Boolean_vector_class;

		template<size_type N_, typename T = void>
		struct fixed_size
		{
			static constexpr auto N = N_;
			using type = T;
		};
		namespace simd_abi
		{
			template<size_type N, typename T = void>
			using fixed_size = details::fixed_size<N, T>;

			template<typename T, size_type N = native_vector_elements(sizeof(T))>
			using native_abi = fixed_size<N, T>;

			template<typename T, size_type N, typename Abi = native_abi<T, N>>
			struct deduce
			{
				using type = Abi;
			};
			template<typename T, size_type N, typename Abi = native_abi<T, N>>
			using deduce_t = typename deduce<T, N, Abi>::type;
		}
}

}

#ifdef VCL_NAMESPACE
}
#endif