#include "zfstream.h"
using namespace exs;

ozfstream::ozfstream(const path_t& path)
	: ozstream(std::make_unique<obfilebuf>(path))  {}

izfstream::izfstream(const path_t& path)
	: izstream(std::make_unique<ibfilebuf>(path))  {}
