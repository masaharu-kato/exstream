#include "zip_file_header.h"
using namespace exs;

zip_file_header::zip_file_header(ibstream& is, bool f_global)
{
	auto sig = is.get<uint32_t>();

	// read and check for local/global magic
	if(f_global) {
		if(sig != 0x02014b50) throw std::runtime_error("Invalid global header signature.");
		is >> version;
	}
	else if(sig != 0x04034b50) {
		throw std::runtime_error("Invalid local header signature.");
	}

	// Read rest of header
	is >> version;
	is >> flags;
	is >> compression_type;
	is >> stamp_date;
	is >> stamp_time;
	is >> crc;
	is >> compressed_size;
	is >> uncompressed_size;

	auto filename_length = is.get<std::uint16_t>();
	auto extra_length = is.get<std::uint16_t>();

	std::uint16_t comment_length = 0;

	if(f_global) {
		is >> comment_length;
		is.skip<std::uint16_t>();	//	disk_number_start
		is.skip<std::uint16_t>();	//	int_file_attrib
		is.skip<std::uint32_t>();	//	ext_file_attrib
		is >> header_offset;
	}

	filename.resize(filename_length, '\0');
	is.read(&filename[0], filename_length);

	extra.resize(extra_length, 0);
	is.read(reinterpret_cast<char *>(extra.data()), extra_length);

	if(f_global) {
		comment.resize(comment_length, '\0');
		is.read(&comment[0], comment_length);
	}
}

void zip_file_header::write(obstream& os, bool f_global) const
{
    if(f_global) {
        os << std::uint32_t(0x02014b50);	// header sig
        os << std::uint16_t(20);			// version made by
    }
    else {
        os << std::uint32_t(0x04034b50);
    }

    os << version;
    os << flags;
    os << compression_type;
    os << stamp_date;
    os << stamp_time;
    os << crc;
    os << compressed_size;
    os << uncompressed_size;
    os << std::uint16_t(filename.length());
    os << std::uint16_t(0); // extra lengthx

    if(f_global) {
        os << std::uint16_t(0); // filecomment
        os << std::uint16_t(0); // disk# start
        os << std::uint16_t(0); // internal file
        os << std::uint32_t(0); // ext final
        os << std::uint32_t(header_offset); // rel offset
    }

    for(auto c : filename) os << c;
}