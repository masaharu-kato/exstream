#include "filebuf.h"
#include "file_exception.h"
using namespace exs;

filebuf::filebuf(const path_t& path, std::ios_base::openmode mode) {
	if(!_basetype::open(path, mode)) {
		throw file_exception("Failed to open file.");
	}
};

ifilebuf::ifilebuf(const path_t& path, std::ios_base::openmode mode)
	: filebuf(path, mode | std::ios_base::in)
{}

ofilebuf::ofilebuf(const path_t& path, std::ios_base::openmode mode)
	: filebuf(path, mode | std::ios_base::out)
{}
