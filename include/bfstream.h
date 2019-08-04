#include "bstream.h"
#include <filesystem>
#include <fstream>

namespace exs {
	
	class obfstream : public obstream {
	public:
		explicit obfstream(const char* filename, std::ios_base::openmode mode = ios_base::out);
		explicit obfstream(std::string filename, std::ios_base::openmode mode = ios_base::out);

	private:
		std::basic_filebuf<char_type, traits_type> fb;
	};


	class ibfstream : public ibstream {
	public:
		explicit ibfstream(const char* filename, std::ios_base::openmode mode = ios_base::in);
		explicit ibfstream(std::string filename, std::ios_base::openmode mode = ios_base::in);

	private:
		std::basic_filebuf<char_type, traits_type> fb;
	};


}