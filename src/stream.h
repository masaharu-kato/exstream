#pragma once
#include <iostream>
#include <memory>

namespace exs {

	using streambuf = std::basic_streambuf<char>;

	class stream {
	public:
		stream(std::unique_ptr<streambuf> sb);

	protected:
		streambuf* get_streambuf() noexcept;

	private:
		std::unique_ptr<streambuf> sb;
	};


	class ostream : public stream, public std::ostream {
	public:
		ostream(std::unique_ptr<streambuf> sb);

	};


	class istream : public stream, public std::istream {
	public:
		istream(std::unique_ptr<streambuf> sb);

	};


}