#pragma once
#include <vector>
#include <string>
#include "bstream.h"

namespace exs {

	//	zip file header
	//	A structure representing the header that occurs before each compressed file in a ZIP archive 
	//	and again at the end of the file with more information.
	class zip_file_header {
	public:
		zip_file_header() = default;
		explicit zip_file_header(ibstream& is, bool f_global);

		void write(obstream& os, bool f_global) const;

		uint16_t version = 20;
		uint16_t flags = 0;
		uint16_t compression_type = 8;
		uint16_t stamp_date = 0;
		uint16_t stamp_time = 0;
		uint32_t crc = 0;
		uint32_t compressed_size = 0;
		uint32_t uncompressed_size = 0;
		path_t path;
		std::string comment;
		std::vector<uint8_t> extra;
		uint32_t header_offset = 0;
	};

}