#pragma once
#include "rented_ptr.h"
#include "_define/rentable_ptr.h"

namespace exs {

	template <class T>
	rentable_ptr<T>::rentable_ptr(rentable_ptr&& ptr) : 
		rented_ptr<T>(ptr),
		current_owner(ptr->current_owner)
	{
		if(current_owner) current_owner->previous_owner = this;
	}

	//template <class T>
	//rentable_ptr<T>::rentable_ptr(rentable_ptr& ptr) :
	//	const_rented_ptr<T>(std::move(ptr._get_uptr())),
	//	current_owner(this)
	//{
	//	ptr->current_owner = this;
	//}

	template <class T>
	void rentable_ptr<T>::give(rentable_ptr& ptr) noexcept {
		ptr.take(*this);
	}

	template <class T>
	void rentable_ptr<T>::give(rentable_ptr&& ptr) noexcept {
		ptr.take(std::move(*this));
	}

	template <class T>
	rented_ptr<T> rentable_ptr<T>::rent_ptr() noexcept(false) {
		return rented_ptr<T>(*this);
	}

	template <class  T>
	bool rentable_ptr<T>::return_ptr() noexcept {
		if(current_owner) return false;
		return rented_ptr<T>::return_ptr();
	}

	template <class T>
	bool rentable_ptr<T>::move_ptr() noexcept {
		if(!current_owner) return false;
		if(previous_owner) return skip_ptr();
		current_owner->previous_owner = nullptr;
		current_owner = nullptr;
		return true;
	}

	template <class T>
	bool rentable_ptr<T>::skip_ptr() noexcept {
		if(!previous_owner || !current_owner) return false;
		previous_owner->current_owner  = current_owner;
		current_owner ->previous_owner = previous_owner;
		previous_owner = nullptr;
		current_owner  = nullptr;
		return true;
	}

	template <class T>
	bool rentable_ptr<T>::is_taken() const noexcept {
		return current_owner;
	}

	template <class T>
	bool rentable_ptr<T>::is_origin() const noexcept {
		return !previous_owner && !current_owner;
	}

	template <class T>
	rentable_ptr<T>::~rentable_ptr() noexcept {
		reset();
	}

	template <class T>
	void rentable_ptr<T>::reset() noexcept {
		move_ptr();
		return_ptr();
	}

}