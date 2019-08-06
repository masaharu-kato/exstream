#pragma once
#include "filebuf.h"

namespace exs {

	//	binary file buffer
	class bfilebuf : public filebuf {
	public:
		bfilebuf(const std::filesystem::path& path, std::ios_base::openmode mode);
	};

	//	binary input file buffer
	class ibfilebuf : public bfilebuf {
	public:
		explicit ibfilebuf(const std::filesystem::path& path, std::ios_base::openmode mode = 0);
	};

	//	binary output file buffer
	class obfilebuf : public bfilebuf {
	public:
		explicit obfilebuf(const std::filesystem::path& path, std::ios_base::openmode mode = 0);
	};



}