#pragma once
#include "stream.h"
#include <filesystem>

namespace exs {
	
	//	binary output file stream
	class ofstream : public ostream {
	public:
		explicit ofstream(const std::filesystem::path& path);
	};


	//	binary input file stream
	class ifstream : public istream {
	public:
		explicit ifstream(const std::filesystem::path& path);
	};


}