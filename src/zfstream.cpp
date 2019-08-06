#include "zfstream.h"
using namespace exs;

ozfstream::ozfstream(const std::filesystem::path& path)
	: ozstream(std::make_unique<obfilebuf>(path))  {}

izfstream::izfstream(const std::filesystem::path& path)
	: izstream(std::make_unique<ibfilebuf>(path))  {}
