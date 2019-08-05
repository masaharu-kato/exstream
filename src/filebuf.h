#pragma once
#include <fstream>

namespace exs {

	class filebuf : public std::basic_filebuf<char> {
	public:
		using _basetype = std::basic_filebuf<char>;

		filebuf(const char* filename, std::ios_base::openmode mode);
		filebuf(std::string filename, std::ios_base::openmode mode);
	};

	class ifilebuf : public filebuf {
	public:
		ifilebuf(const char* filename);
		ifilebuf(std::string filename);
	};

	class ofilebuf : public filebuf {
	public:
		ofilebuf(const char* filename);
		ofilebuf(std::string filename);
	};


}