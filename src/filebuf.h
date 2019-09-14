#pragma once
#include <fstream>
#include <filesystem>
#include "basic_types.h"

namespace exs {

	//	file buffer (basic file buffer)
	class filebuf : public std::basic_filebuf<char> {
	public:
		using _basetype = std::basic_filebuf<char>;

		filebuf(const path_t& path, std::ios_base::openmode mode);
	};

	//	input file buffer
	class ifilebuf : public filebuf {
	public:
		explicit ifilebuf(const path_t& path, std::ios_base::openmode mode = 0);
	};

	//	output file buffer
	class ofilebuf : public filebuf {
	public:
		explicit ofilebuf(const path_t& path, std::ios_base::openmode mode = 0);
	};


}