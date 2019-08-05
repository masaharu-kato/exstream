#include "bfilebuf.h"
using namespace exs;

bfilebuf::bfilebuf(const char* filename, std::ios_base::openmode mode)
	: filebuf(filename, mode | std::ios::binary)
{}

bfilebuf::bfilebuf(std::string filename, std::ios_base::openmode mode)
	: bfilebuf(filename.c_str(), mode)
{}

ibfilebuf::ibfilebuf(const char* filename)
	: bfilebuf(filename, std::ios_base::in)
{}

ibfilebuf::ibfilebuf(std::string filename)
	: ibfilebuf(filename.c_str())
{}

obfilebuf::obfilebuf(const char* filename)
	: bfilebuf(filename, std::ios_base::out)
{}

obfilebuf::obfilebuf(std::string filename)
	: obfilebuf(filename.c_str())
{}
