#pragma once
#include <fstream>
#include "basic_types.h"

namespace exs {

	//	file buffer (basic file buffer)
	class filebuf : public std::basic_filebuf<char> {
	public:
		using base_type = std::basic_filebuf<char>;
		using openmode_t = std::ios_base::openmode;

		filebuf(const path_t& path, openmode_t mode);
	};

	//	input file buffer
	class ifilebuf : public filebuf {
	public:
		explicit ifilebuf(const path_t& path, openmode_t mode = (openmode_t)0);
	};

	//	output file buffer
	class ofilebuf : public filebuf {
	public:
		explicit ofilebuf(const path_t& path, openmode_t mode = (openmode_t)0);
	};


}