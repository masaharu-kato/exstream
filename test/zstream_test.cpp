#include <iostream>
#include <fstream>
#include <ctime>
#include "zfstream.h"

using namespace exs;

int main(int argc, char* argv[]) {

	if(argc < 2) { std::cerr << "Input file not specified.\n"; return -1; }

	exs::izfstream izs(argv[1]);

	for(auto path : izs.files()) {
		std::cout << "[" << path << "]\n";
		auto fsp = izs.open(path);
		if(fsp) {
			std::istream is(fsp.get());
			std::string content(std::istreambuf_iterator<char>(is), {});
			std::cout << content << "\n";
		}
	}


	if(argc < 3) { std::cerr << "Output file not specified.\n"; return -2; }

	exs::ozfstream ozs(argv[2]);

	for(auto path : izs.files()) {
		std::cout << "[" << path << "]\n";
		auto ifsp = izs.open(path);
		if(ifsp) {
			auto ofsp = ozs.open(path);
			if(ofsp) {
				std::istream is(ifsp.get());
				std::ostream os(ofsp.get());
				std::string content(std::istreambuf_iterator<char>(is), {});
				os << content;
			}
		}
	}

	{
		auto fsp = ozs.open("new_file.txt");
		if(fsp) {
			std::ostream os(fsp.get());
			os << "This file is created by extream.\n";
		}
	}

	return 0;
}
