#pragma once
#include "_define/const_rented_ptr.h"
#include "_define/rented_ptr.h"
#include "_define/rentable_ptr.h"

namespace exs {

	template <class T>
	const_rented_ptr<T>::const_rented_ptr(const_rented_ptr&& ptr) : 
		rent_ptr_base<T>(std::move(ptr._get_uptr())),
		previous_owner(ptr->previous_owner)
	{
		if(previous_owner) previous_owner -> current_owner = this;
	}

	template <class T>
	const_rented_ptr<T>::const_rented_ptr(rentable_ptr<T>& ptr) :
		rent_ptr_base<T>(std::move(ptr._get_uptr())),
		previous_owner(&ptr)
	{
		ptr.current_owner = this;
	}

	template <class  T>
	bool const_rented_ptr<T>::return_ptr() noexcept {
		if(!previous_owner) return false;
		previous_owner->base_uptr::reset(base_uptr::release());
		previous_owner->current_owner = nullptr;
		previous_owner = nullptr;
		return true;
	}

	template <class  T>
	bool const_rented_ptr<T>::has_ptr() const noexcept {
		return base_uptr::operator bool();
	}

	template <class T>
	bool const_rented_ptr<T>::is_taking() const noexcept {
		return previous_owner;
	}

	template <class T>
	bool const_rented_ptr<T>::is_taken() const noexcept {
		return false;
	}

	template <class T>
	bool const_rented_ptr<T>::is_origin() const noexcept {
		return !previous_owner;
	}

	template <class T>
	const_rented_ptr<T>::~const_rented_ptr() noexcept {
		reset();
	}

	template <class T>
	void const_rented_ptr<T>::reset() noexcept {
		return_ptr();
	}

	template <class T>
	auto const_rented_ptr<T>::_get_uptr() noexcept -> base_uptr {
		return (typename base_type::base_uptr&&)(*this);
	}

}