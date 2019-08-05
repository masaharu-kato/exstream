#pragma once
#include "bstream.h"
#include "bfilebuf.h"

namespace exs {
	
	class obfstream : public obstream {
	public:
		explicit obfstream(const char* filename);
		explicit obfstream(std::string filename);
	};


	class ibfstream : public ibstream {
	public:
		explicit ibfstream(const char* filename);
		explicit ibfstream(std::string filename);
	};


}