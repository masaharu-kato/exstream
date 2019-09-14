#include "bfstream.h"
#include "bfilebuf.h"
using namespace exs;

obfstream::obfstream(const path_t& path)
	: obstream(std::make_unique<obfilebuf>(path)) {}

ibfstream::ibfstream(const path_t& path)
	: ibstream(std::make_unique<ibfilebuf>(path)) {}
