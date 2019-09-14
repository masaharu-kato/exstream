#pragma once
#include "filebuf.h"

namespace exs {

	//	binary file buffer
	class bfilebuf : public filebuf {
	public:
		bfilebuf(const path_t& path, openmode_t mode);
	};

	//	binary input file buffer
	class ibfilebuf : public bfilebuf {
	public:
		explicit ibfilebuf(const path_t& path, openmode_t mode = (openmode_t)0);
	};

	//	binary output file buffer
	class obfilebuf : public bfilebuf {
	public:
		explicit obfilebuf(const path_t& path, openmode_t mode = (openmode_t)0);
	};



}