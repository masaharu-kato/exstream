#include "zfstream.h"
using namespace exs;

ozfstream::ozfstream(const char* filename)
	: ozstream(std::make_unique<obfilebuf>(filename))  {}

ozfstream::ozfstream(std::string filename)
	: ozstream(std::make_unique<obfilebuf>(filename))  {}


izfstream::izfstream(const char* filename)
	: izstream(std::make_unique<ibfilebuf>(filename))  {}

izfstream::izfstream(std::string filename)
	: izstream(std::make_unique<ibfilebuf>(filename))  {}