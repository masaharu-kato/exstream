#pragma once
#include "const_rented_ptr.h"
#include "_define/rented_ptr.h"
#include "_define/rentable_ptr.h"

namespace exs {

	//template <class T>
	//rented_ptr<T>::rented_ptr(rented_ptr&& ptr) : 
	//	const_rented_ptr<T>(std::move(ptr._get_uptr())),
	//	previous_owner(ptr->previous_owner)
	//{
	//	if(previous_owner) previous_owner -> current_owner = this;
	//}

	template <class T>
	void rented_ptr<T>::take(rented_ptr& ptr) noexcept {
		return_ptr();
		base_uptr::reset(std::move(ptr));
		previous_owner = &ptr;
		ptr->current_owner = this;
	}
		
	template <class T>
	void rented_ptr<T>::take(rented_ptr&& ptr) noexcept {
		return_ptr();
		base_uptr::reset(std::move(ptr));
	}

	template <class T>
	void rented_ptr<T>::reset(base_uptr&& ptr) noexcept {
		reset();
		base_uptr::reset(std::move(ptr));
	}

	template <class T>
	void rented_ptr<T>::replace(base_uptr&& ptr) noexcept {
		if(has_ptr()) base_uptr::reset(std::move(ptr));
	}

}