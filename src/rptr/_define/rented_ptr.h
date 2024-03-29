#pragma once
#include "const_rented_ptr.h"

namespace exs {

	template <class T>
	class rentable_ptr;

	template <class T>
	class rented_ptr : public const_rented_ptr<T> {
		friend const_rented_ptr<T>;
		friend rentable_ptr<T>;
		using base_type = const_rented_ptr<T>;
	protected:
		using base_type::previous_owner;
	public:
		using base_type::base_type;
		using base_type::return_ptr;
		using base_type::has_ptr;
		using base_type::reset;
		using base_uptr = typename base_type::base_uptr;

	//	for gcc/clang only, msvc doesn't need this
		rented_ptr(base_uptr&& ptr) : 
			base_type(std::move(ptr))
		{}
		
	//	explicit rented_ptr(rentable_ptr<T>& ptr);

	//	take other pointer to this (temporarily)
		void take(rented_ptr& ptr) noexcept;

	//	take other pointer to this (completely)
		void take(rented_ptr&& ptr) noexcept;

	//	reset pointer and set new pointer
		void reset(base_uptr&& ptr) noexcept;

	//	replace pointer (keep previous owner and current owner)
		void replace(base_uptr&& ptr) noexcept;

	protected:
	//	using const_rented_ptr<T>::previous_owner;
	};

}
