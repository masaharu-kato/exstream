#pragma once
#include "stream.h"

namespace exs {

	class obstream : public ostream {
	public:
		using ostream::ostream;

		template <class T> obstream& put(T value) noexcept;
		template <class T> obstream& operator <<(T value) noexcept { return put(value); }

	};


	class ibstream : public istream {
	public:
		using istream::istream;

		template <class T> ibstream& get(T& value) noexcept;
		template <class T> T get() noexcept;
		template <class T> ibstream& operator >>(T& value) noexcept { return get(value); }
		template <class T> ibstream& skip() noexcept;

	};



	template <class T>
	obstream& obstream::put(T value) noexcept {
		write((char_type*)&value, sizeof(T));
		return *this;
	}
	
	template <class T>
	ibstream& ibstream::get(T& value) noexcept {
		read((char_type*)&value, sizeof(T));
		return *this;
	}
	
	template <class T>
	T ibstream::get() noexcept {
		T value;
		read((char_type*)&value, sizeof(T));
		return value;
	}

	template <class T>
	ibstream& ibstream::skip() noexcept {
		T _dummy;
		read((char_type*)&_dummy, sizeof(T));
		return *this;
	}

}