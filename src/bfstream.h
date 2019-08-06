#pragma once
#include "bstream.h"
#include <filesystem>

namespace exs {
	
	//	binary output file stream
	class obfstream : public obstream {
	public:
		explicit obfstream(const std::filesystem::path& path);
	};


	//	binary input file stream
	class ibfstream : public ibstream {
	public:
		explicit ibfstream(const std::filesystem::path& path);
	};


}