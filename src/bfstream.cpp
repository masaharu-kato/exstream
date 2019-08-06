#include "bfstream.h"
#include "bfilebuf.h"
using namespace exs;

obfstream::obfstream(const std::filesystem::path& path)
	: obstream(std::make_unique<obfilebuf>(path)) {}

ibfstream::ibfstream(const std::filesystem::path& path)
	: ibstream(std::make_unique<ibfilebuf>(path)) {}
