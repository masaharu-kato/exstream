#pragma once
#include "filebuf.h"

namespace exs {

	class bfilebuf : public filebuf {
	public:
		bfilebuf(const char* filename, std::ios_base::openmode mode);
		bfilebuf(std::string filename, std::ios_base::openmode mode);
	};

	class ibfilebuf : public bfilebuf {
	public:
		ibfilebuf(const char* filename);
		ibfilebuf(std::string filename);
	};

	class obfilebuf : public bfilebuf {
	public:
		obfilebuf(const char* filename);
		obfilebuf(std::string filename);
	};



}