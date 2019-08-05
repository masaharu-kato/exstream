#include "filebuf.h"
#include <stdexcept>
using namespace exs;

filebuf::filebuf(const char* filename, std::ios_base::openmode mode) {
	if(!_basetype::open(filename, mode)) {
		throw std::runtime_error("Failed to open file.");
	}
};

filebuf::filebuf(std::string filename, std::ios_base::openmode mode)
	: filebuf(filename.c_str(), mode)
{}

ifilebuf::ifilebuf(const char* filename)
	: filebuf(filename, std::ios_base::in)
{}

ifilebuf::ifilebuf(std::string filename)
	: ifilebuf(filename.c_str())
{}

ofilebuf::ofilebuf(const char* filename)
	: filebuf(filename, std::ios_base::out)
{}

ofilebuf::ofilebuf(std::string filename)
	: ofilebuf(filename.c_str())
{}