#pragma once
#include "bstream.h"

namespace exs {
	
	//	binary output file stream
	class obfstream : public obstream {
	public:
		explicit obfstream(const path_t& path);
	};


	//	binary input file stream
	class ibfstream : public ibstream {
	public:
		explicit ibfstream(const path_t& path);
	};


}