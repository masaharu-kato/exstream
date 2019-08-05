#pragma once
#include <fstream>

namespace exs {

	//	file buffer (basic file buffer)
	class filebuf : public std::basic_filebuf<char> {
	public:
		using _basetype = std::basic_filebuf<char>;

		filebuf(const char* filename, std::ios_base::openmode mode);
		filebuf(std::string filename, std::ios_base::openmode mode);
	};

	//	input file buffer
	class ifilebuf : public filebuf {
	public:
		ifilebuf(const char* filename);
		ifilebuf(std::string filename);
	};

	//	output file buffer
	class ofilebuf : public filebuf {
	public:
		ofilebuf(const char* filename);
		ofilebuf(std::string filename);
	};


}