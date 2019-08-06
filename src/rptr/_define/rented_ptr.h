#pragma once
#include "const_rented_ptr.h"

namespace exs {

	template <class T>
	class rentable_ptr;

	template <class T>
	class rented_ptr : public const_rented_ptr<T> {
		friend const_rented_ptr<T>;
		friend rentable_ptr<T>;
	public:
		using const_rented_ptr<T>::const_rented_ptr;
		using const_rented_ptr<T>::return_ptr;
		using const_rented_ptr<T>::reset;
		
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