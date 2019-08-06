#include "filebuf.h"
#include "file_exception.h"
using namespace exs;

filebuf::filebuf(const std::filesystem::path& path, std::ios_base::openmode mode) {
	if(!_basetype::open(path, mode)) {
		throw file_exception("Failed to open file.");
	}
};

ifilebuf::ifilebuf(const std::filesystem::path& path, std::ios_base::openmode mode)
	: filebuf(path, mode | std::ios_base::in)
{}

ofilebuf::ofilebuf(const std::filesystem::path& path, std::ios_base::openmode mode)
	: filebuf(path, mode | std::ios_base::out)
{}
