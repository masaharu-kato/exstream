#pragma once
#include "filebuf.h"

namespace exs {

	//	binary file buffer
	class bfilebuf : public filebuf {
	public:
		bfilebuf(const char* filename, std::ios_base::openmode mode);
		bfilebuf(std::string filename, std::ios_base::openmode mode);
	};

	//	binary input file buffer
	class ibfilebuf : public bfilebuf {
	public:
		ibfilebuf(const char* filename);
		ibfilebuf(std::string filename);
	};

	//	binary output file buffer
	class obfilebuf : public bfilebuf {
	public:
		obfilebuf(const char* filename);
		obfilebuf(std::string filename);
	};



}