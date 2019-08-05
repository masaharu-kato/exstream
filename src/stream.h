#pragma once
#include <iostream>
#include <memory>

namespace exs {

	//	stream buffer (basic stream buffer)
	using streambuf = std::basic_streambuf<char>;

	//	stream (basic stream)
	class stream {
	protected:
		stream(std::unique_ptr<streambuf> sb);

		streambuf* get_streambuf() noexcept;

	private:
		//	stream buffer
		std::unique_ptr<streambuf> sb;
	};


	//	output stream
	class ostream : public stream, public std::ostream {
	public:
		ostream(std::unique_ptr<streambuf> sb);

	};

	
	//	input stream
	class istream : public stream, public std::istream {
	public:
		istream(std::unique_ptr<streambuf> sb);

	};


}