#pragma once
#include "rent_ptr_base.h"

namespace exs {

	template <class T>
	class const_rented_ptr : public rent_ptr_base<T> {
		friend rented_ptr<T>;
		friend rentable_ptr<T>;
	public:
		using rent_ptr_base<T>::rent_ptr_base;

		explicit const_rented_ptr(rentable_ptr<T>& ptr);
		const_rented_ptr(const const_rented_ptr&) = delete;
		const_rented_ptr(const_rented_ptr&& ptr);

		~const_rented_ptr() noexcept;

		bool has_ptr() const noexcept;
		bool is_taking() const noexcept;
		bool is_taken() const noexcept;
		bool is_origin() const noexcept;

	protected:
	//	return pointer to previous owner
		bool return_ptr() noexcept;
		
	//	reset pointer
		void reset() noexcept;

		base_uptr _get_uptr() noexcept;

		rentable_ptr<T>* previous_owner = nullptr;
	};

}