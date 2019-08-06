#pragma once
#include <memory>

namespace exs {

	template <class T>
	class rentable_ptr : public std::unique_ptr<T> {
	public:
		using std::unique_ptr<T>::unique_ptr;
		explicit rentable_ptr(const rentable_ptr& ptr);
		rentable_ptr(rentable_ptr&& ptr);

	//	take other pointer to this (temporarily)
		void take(rentable_ptr& ptr);

	//	give this pointer to other (temporarily)
		void give(rentable_ptr& ptr);

	//	take other pointer to this (completely)
		void take(rentable_ptr&& ptr);

	//	give this pointer to other (completely)
		void give(rentable_ptr&& ptr);
		
		bool return_ptr();

		bool has_ptr() const noexcept;
		bool is_taking() const noexcept;
		bool is_taken() const noexcept;
		bool is_origin() const noexcept;

		~rentable_ptr();

	private:
		rentable_ptr* previous_owner = nullptr;
		rentable_ptr* current_owner  = nullptr;
	};


	template <class T>
	void rentable_ptr<T>::take(rentable_ptr& ptr) {
		return_ptr();
		reset(std::move(ptr));
		previous_owner = &ptr;
		ptr->current_owner = this;
	}

	template <class T>
	void rentable_ptr<T>::give(rentable_ptr& ptr) {
		ptr.take(*this);
	}

	template <class T>
	void rentable_ptr<T>::take(rentable_ptr&& ptr) {
		return_ptr();
		reset(std::move(ptr));
	}

	template <class T>
	void rentable_ptr<T>::give(rentable_ptr&& ptr) {
		ptr.take(std::move(*this));
	}


	template <class  T>
	bool rentable_ptr<T>::return_ptr() {
		if(!has_ptr()) return;
		previous_owner->reset(release());
		previous_owner->current_owner = nullptr;
		previous_owner = nullptr;
	}

	template <class  T>
	bool rentable_ptr<T>::has_ptr() const noexcept {
		return operator bool();
	}

	template <class T>
	bool rentable_ptr<T>::is_taking() const noexcept {
		return previous_owner;
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
	rentable_ptr<T>::~rentable_ptr() {
		return_ptr();
	}




}