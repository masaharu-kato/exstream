#include "bfilebuf.h"
using namespace exs;

bfilebuf::bfilebuf(const std::filesystem::path& path, std::ios_base::openmode mode)
	: filebuf(path, mode | std::ios::binary)
{}

ibfilebuf::ibfilebuf(const std::filesystem::path& path, std::ios_base::openmode mode)
	: bfilebuf(path, mode | std::ios_base::in)
{}

obfilebuf::obfilebuf(const std::filesystem::path& path, std::ios_base::openmode mode)
	: bfilebuf(path, mode | std::ios_base::out)
{}
