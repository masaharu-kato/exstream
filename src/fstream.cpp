#include "fstream.h"
#include "filebuf.h"
using namespace exs;

ofstream::ofstream(const std::filesystem::path& path)
	: ostream(std::make_unique<ofilebuf>(path)) {}

ifstream::ifstream(const std::filesystem::path& path)
	: istream(std::make_unique<ifilebuf>(path)) {}
