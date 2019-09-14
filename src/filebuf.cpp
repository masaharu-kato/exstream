#include "filebuf.h"
#include "file_exception.h"
using namespace exs;

filebuf::filebuf(const path_t& path, openmode_t mode) {
	if(!base_type::open(path, mode)) {
		throw file_exception("Failed to open file.");
	}
};

ifilebuf::ifilebuf(const path_t& path, openmode_t mode)
	: filebuf(path, mode | std::ios_base::in)
{}

ofilebuf::ofilebuf(const path_t& path, openmode_t mode)
	: filebuf(path, mode | std::ios_base::out)
{}
