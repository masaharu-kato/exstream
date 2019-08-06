#pragma once
#include "zstream.h"
#include "bfilebuf.h"

namespace exs {

//	zip file output stream
	class ozfstream : public ozstream {
	public:
		explicit ozfstream(const std::filesystem::path& path);
	};

//	zip file input stream
	class izfstream : public izstream {
	public:
		explicit izfstream(const std::filesystem::path& path);
	};

}