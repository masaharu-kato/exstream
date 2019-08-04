#include <iostream>

namespace exs {

	class obstream : public std::ostream {
	public:
		using std::ostream::basic_ostream;

		template <class T>
		obstream& operator <<(T value) noexcept;

	};


	class ibstream : public std::istream {
	public:
		using std::istream::basic_istream;

		template <class T>
		ibstream& operator >>(T& value) noexcept;

	};



	template <class T>
	obstream& obstream::operator <<(T value) noexcept {
		write((char_type*)&value, sizeof(T));
	}
	
	
	template <class T>
	ibstream& ibstream::operator >>(T& value) noexcept {
		read((char_type*)&value, sizeof(T));
	}

}