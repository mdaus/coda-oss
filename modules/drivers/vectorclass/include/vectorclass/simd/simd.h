/***************************  simd.h   *********************************
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

#ifndef VECTORCLASS_H
#include "vectorclass.h"
#endif

#include "simd_abi.h"

#ifdef VCL_NAMESPACE
namespace VCL_NAMESPACE {
#endif

namespace simd
{
	template<typename T, typename Abi = details::simd_abi::native_abi<T>> struct basic_simd;

	template<size_t Bytes, typename Abi = details::simd_abi::fixed_size<details::native_vector_elements(Bytes)>> struct basic_simd_mask
	{
		using value_type = bool;
		using abi_type = Abi;
		static constexpr auto N = abi_type::N;
		using VecNt = typename abi_type::type; // e.g., Vec4i
		using native_type = details::Boolean_vector_class<N, VecNt>; // e.g., Vec4ib

	        static constexpr auto size = basic_simd<details::integer_from<Bytes>, Abi>::size();
		static_assert(size == native_type::size());

		constexpr basic_simd_mask() noexcept = default;

		// [simd.ctor]
		template<typename U> constexpr basic_simd_mask(U&& value) noexcept : v_(value) {}
		template<size_t UBytes, typename UAbi>
		constexpr explicit basic_simd_mask(const basic_simd_mask<UBytes, UAbi>& other) noexcept : v_(other.v_) {}
		template<typename G> constexpr explicit basic_simd_mask(G&& gen, std::nullptr_t /*TODO: remove*/) noexcept;
		// "Implementations should enable explicit conversion from and to implementation-defined types."
		constexpr explicit operator native_type() const { return v_; }
		constexpr explicit basic_simd_mask(const native_type& init) : v_(init) {}

		// [simd.subscr]
		// §2.5 of https://github.com/vectorclass/manual/raw/master/vcl_manual.pdf
		// "Note that you can read a vector element with the[] operator, but not write an element."
		constexpr value_type& operator[](details::size_type) & = delete;
		constexpr value_type operator[](details::size_type i) const& { return v_[i]; }

	private:
		native_type v_;
	};

	template<typename T, details::size_type N = basic_simd_mask<sizeof(T)>::size()>
	using simd_mask = basic_simd_mask<sizeof(T), details::simd_abi::deduce_t<T, N>>;

	template<typename T, typename Abi> struct basic_simd
	{
		using value_type = T;
		using mask_type = basic_simd_mask<sizeof(T), Abi>;
		using abi_type = Abi;

		using native_type = typename details::VecNt<abi_type::N, T>::Vector_class; // e.g., Vec4i

	    //static constexpr std::integral_constant<details::size_type, native_type::size()> size;
	    static constexpr details::size_type size() { return native_type::size(); }
		//static_assert(size() == native_type::size());

		constexpr basic_simd() noexcept = default;

		// [simd.ctor]
		template<typename U> constexpr basic_simd(U&& value) noexcept : v_(value) {}
		template<typename U, typename UAbi>
		constexpr explicit basic_simd(const basic_simd<U, UAbi>& other) noexcept : v_(other.v_) {}
		template<typename G> constexpr explicit basic_simd(G&& gen, std::nullptr_t /*TODO: remove*/) noexcept
		{
			for (int i = 0; i < v_.size(); i++)
			{
				v_.insert(i, gen(i));
			}
		}
		template<typename It, typename... Flags>
		constexpr basic_simd(It first, Flags...)
		{
			// TODO: look at simd_flags
			// However, §2.4 states "There is hardly any difference in efficiency
			// between `load` and `load_a` on newer microprocessors."
			copy_from(first);
		}
		//template<class It, class... Flags>
		//constexpr basic_simd(It first, const mask_type& mask, simd_flags<Flags...> = {});

		// "Implementations should enable explicit conversion from and to implementation-defined types."
		constexpr explicit operator native_type() const { return v_; }
		constexpr explicit basic_simd(const native_type& init) : v_(init) {}

		// [simd.copy]
		template<typename It>
		constexpr void copy_from(It first)
		{
			// TODO: look at simd_flags
			// However, §2.4 states "There is hardly any difference in efficiency
			// between `load` and `load_a` on newer microprocessors."

			// "`It` satisfies contiguous_iterator."
			const auto mem = &(*first);
			v_.load(mem);
		}
		//template<typename It, typename... Flags>
		//constexpr void copy_from(It first, const mask_type& mask, simd_flags<Flags...> f = {});
		template<typename Out>
		constexpr void copy_to(Out first) const
		{
			// TODO: look at simd_flags
			// However, §2.5 states "There is hardly any difference in efficiency
			// between `store` and `store_a` on newer microprocessors."

			// "`It` satisfies contiguous_iterator."
			auto mem = &(*first);
			v_.store(mem);
		}
		//template<typename Out, typename... Flags>
		//constexpr void copy_to(Out first, const mask_type& mask, simd_flags<Flags...> f = {}) const;

		// [simd.subscr]
		// §2.5 of https://github.com/vectorclass/manual/raw/master/vcl_manual.pdf
		// "Note that you can read a vector element with the[] operator, but not write an element."
		constexpr value_type& operator[](details::size_type) & = delete;
		constexpr value_type operator[](details::size_type i) const& { return v_[i]; }

		// [simd.unary]
		constexpr basic_simd& operator++() noexcept;
		constexpr basic_simd operator++(int) noexcept;
		constexpr basic_simd& operator--() noexcept;
		constexpr basic_simd operator--(int) noexcept;
		constexpr mask_type operator!() const noexcept;
		constexpr basic_simd operator~() const noexcept;
		constexpr basic_simd operator+() const noexcept;
		constexpr basic_simd operator-() const noexcept;

		// [simd.cond]
		friend constexpr basic_simd simd_select_impl(
			const mask_type& c, const basic_simd& a, const basic_simd& b) noexcept
		{
			using Vec_b = typename mask_type::native_type;
			return select(static_cast<Vec_b>(c), static_cast<native_type>(a), static_cast<native_type>(b));
		}

	//private:
		native_type v_;
	};

	template<typename T, details::size_type N = basic_simd<T>::size()>
	using simd = basic_simd<T, details::simd_abi::deduce_t<T, N>>;
}


#ifdef VCL_NAMESPACE
}
#endif
