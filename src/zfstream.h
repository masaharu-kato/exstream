#pragma once
#include "zstream.h"
#include "bfilebuf.h"

namespace exs {

//	zip file output stream
	class ozfstream : public ozstream {
	public:
		ozfstream(const char* filename);
		ozfstream(std::string filename);
	};

//	zip file input stream
	class izfstream : public izstream {
	public:
		izfstream(const char* filename);
		izfstream(std::string filename);
	};

}