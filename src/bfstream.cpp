#include "bfstream.h"
using namespace exs;

obfstream::obfstream(const char* filename, std::ios_base::openmode mode)
	: obstream(&fb) 
{
	if(!fb.open(filename, mode | ios_base::binary | ios_base::out)) {
		throw std::runtime_error("Failed to open file.");
	}
}

obfstream::obfstream(std::string filename, std::ios_base::openmode mode)
	: obfstream(filename.c_str(), mode)
{}


ibfstream::ibfstream(const char* filename, std::ios_base::openmode mode)
	: ibstream(&fb) 
{
	if(!fb.open(filename, mode | ios_base::binary | ios_base::in)) {
		throw std::runtime_error("Failed to open file.");
	}
}

ibfstream::ibfstream(std::string filename, std::ios_base::openmode mode)
	: ibfstream(filename.c_str(), mode)
{}