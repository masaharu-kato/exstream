#pragma once
#include "zstream.h"
#include "bfilebuf.h"

namespace exs {

	class ozfstream : public ozstream {
	public:
		ozfstream(const char* filename);
		ozfstream(std::string filename);
	};

	class izfstream : public izstream {
	public:
		izfstream(const char* filename);
		izfstream(std::string filename);
	};

}