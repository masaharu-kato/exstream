#pragma once
#include <vector>
#include <string>
#include "bstream.h"

namespace exs {

	//	A structure representing the header that occurs before each compressed file in a ZIP archive 
	//	and again at the end of the file with more information.
	class zip_file_header {
	public:
		zip_file_header() = default;
		explicit zip_file_header(ibstream& is, bool f_global);

		void write(obstream& os, bool f_global) const;

		std::uint16_t version = 20;
		std::uint16_t flags = 0;
		std::uint16_t compression_type = 8;
		std::uint16_t stamp_date = 0;
		std::uint16_t stamp_time = 0;
		std::uint32_t crc = 0;
		std::uint32_t compressed_size = 0;
		std::uint32_t uncompressed_size = 0;
		std::string filename;
		std::string comment;
		std::vector<std::uint8_t> extra;
		std::uint32_t header_offset = 0;
	};

}