#include "fstream.h"
#include "filebuf.h"
using namespace exs;

ofstream::ofstream(const path_t& path)
	: ostream(std::make_unique<ofilebuf>(path)) {}

ifstream::ifstream(const path_t& path)
	: istream(std::make_unique<ifilebuf>(path)) {}
