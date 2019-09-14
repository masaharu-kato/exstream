#include "bfilebuf.h"
using namespace exs;

bfilebuf::bfilebuf(const path_t& path, openmode_t mode)
	: filebuf(path, mode | std::ios::binary)
{}

ibfilebuf::ibfilebuf(const path_t& path, openmode_t mode)
	: bfilebuf(path, mode | std::ios_base::in)
{}

obfilebuf::obfilebuf(const path_t& path, openmode_t mode)
	: bfilebuf(path, mode | std::ios_base::out)
{}
