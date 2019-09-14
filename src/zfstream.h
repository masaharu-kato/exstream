#pragma once
#include "zstream.h"
#include "bfilebuf.h"

namespace exs {

//	zip file output stream
	class ozfstream : public ozstream {
	public:
		explicit ozfstream(const path_t& path);
	};

//	zip file input stream
	class izfstream : public izstream {
	public:
		explicit izfstream(const path_t& path);
	};

}