#pragma once
#include "rented_ptr.h"

namespace exs {

	template <class T>
	class rentable_ptr : public rented_ptr<T> {
		friend const_rented_ptr<T>;
		friend rented_ptr<T>;
		using base_type = rented_ptr<T>;
	protected:
		using base_type::previous_owner;

	public:
		using base_type::base_type;
		using base_uptr = typename base_type::base_uptr;

	//	for gcc/clang only, msvc doesn't need this
		rentable_ptr(base_uptr&& ptr) : 
			base_type(std::move(ptr))
		{}

		rentable_ptr(const rentable_ptr&) = delete;
		rentable_ptr(rentable_ptr&& ptr);

		~rentable_ptr() noexcept;

	//	give this pointer to other (temporarily)
		void give(rentable_ptr& ptr) noexcept;

	//	give this pointer to other (completely)
		void give(rentable_ptr&& ptr) noexcept;

	//	create new pointer (takes this pointer)
		rented_ptr<T> rent_ptr() noexcept(false);
		
	//	return pointer to previous owner
		bool return_ptr() noexcept;

	//	move pointer to current owner (completely)
		bool move_ptr() noexcept;

	//	link previous owner and current owner
		bool skip_ptr() noexcept;

		bool is_taken() const noexcept;
		bool is_origin() const noexcept;

		void reset() noexcept;


	protected:
	//	using rented_ptr<T>::previous_owner;
		const_rented_ptr<T>* current_owner  = nullptr;
	};

}