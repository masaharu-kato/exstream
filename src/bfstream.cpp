#include "bfstream.h"
using namespace exs;

obfstream::obfstream(const char* filename)
	: obstream(std::make_unique<obfilebuf>(filename)) {}

obfstream::obfstream(std::string filename)
	: obstream(std::make_unique<obfilebuf>(filename)) {}


ibfstream::ibfstream(const char* filename)
	: ibstream(std::make_unique<ibfilebuf>(filename)) {}

ibfstream::ibfstream(std::string filename)
	: ibstream(std::make_unique<ibfilebuf>(filename)) {}
