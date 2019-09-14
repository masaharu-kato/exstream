#pragma once
#include "stream.h"

namespace exs {
	
	//	binary output file stream
	class ofstream : public ostream {
	public:
		explicit ofstream(const path_t& path);
	};


	//	binary input file stream
	class ifstream : public istream {
	public:
		explicit ifstream(const path_t& path);
	};


}