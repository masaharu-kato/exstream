#pragma once
#include <memory>

namespace exs {

	template <class T>
	class const_rent_ptr;

	template <class T>
	class rented_ptr;

	template <class T>
	class rentable_ptr;

	template <class T>
	class rent_ptr_base : public std::unique_ptr<T> {
	public:
		using base_uptr = std::unique_ptr<T>;

	protected:
		using base_uptr::unique_ptr;

	};

}