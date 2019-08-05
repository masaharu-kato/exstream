#include "stream.h"
using namespace exs;

stream::stream(std::unique_ptr<streambuf> sb)
	: sb(std::move(sb)) {}

streambuf* stream::get_streambuf() noexcept {
	return sb.get();
}

ostream::ostream(std::unique_ptr<streambuf> sb)
	: stream(std::move(sb)), std::ostream(get_streambuf())
{}

istream::istream(std::unique_ptr<streambuf> sb)
	: stream(std::move(sb)), std::istream(get_streambuf())
{}
