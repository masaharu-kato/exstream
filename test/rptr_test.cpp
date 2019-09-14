#include "rptr/rentable_ptr.h"
#include <iostream>
#include <sstream>

using namespace exs;

class string_stream : public std::stringstream {
public:
	string_stream(std::string source)
		: std::stringstream(source)
	{
		std::cerr << "MyClass constructed.\n";
	}

	~string_stream() {
		std::cerr << "MyClass deconstructed.\n";
	}

	template <class T>
	T get() {
		T value;
		*this >> value;
		return value;
	}

};

int main(void) {

	auto fuck = std::make_unique<string_stream>("hogehoge fugafuga piyopiyo");

	rentable_ptr<string_stream> source(std::move(fuck));

	std::cout << source->get<std::string>();

	auto rented = source.rent_ptr();
	
	std::cout << rented->get<std::string>();
	

	auto rented_2 = source.rent_ptr();
	
	std::cout << rented_2->get<std::string>();

	return 0;
}