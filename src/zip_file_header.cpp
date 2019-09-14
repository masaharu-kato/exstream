#include "zip_file_header.h"
#include "zexception.h"
using namespace exs;

zip_file_header::zip_file_header(ibstream& is, bool f_global)
{
	auto sig = is.get<uint32_t>();

	// read and check for local/global magic
	if(f_global) {
		if(sig != 0x02014b50) throw zexception("Invalid global header signature.");
		is >> version;
	}
	else if(sig != 0x04034b50) {
		throw zexception("Invalid local header signature.");
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

	auto filename_length = is.get<uint16_t>();
	auto extra_length = is.get<uint16_t>();

	uint16_t comment_length = 0;

	if(f_global) {
		is >> comment_length;
		is.skip<uint16_t>();	//	disk_number_start
		is.skip<uint16_t>();	//	int_file_attrib
		is.skip<uint32_t>();	//	ext_file_attrib
		is >> header_offset;
	}

	std::string filename(filename_length, '\0');
	is.read(&filename[0], filename_length);
	path = filename;

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
		os << uint32_t(0x02014b50);	// header sig
		os << uint16_t(20);			// version made by
	}
	else {
		os << uint32_t(0x04034b50);
	}

	os << version;
	os << flags;
	os << compression_type;
	os << stamp_date;
	os << stamp_time;
	os << crc;
	os << compressed_size;
	os << uncompressed_size;
	os << uint16_t(path.length());
	os << uint16_t(0); // extra lengthx

	if(f_global) {
		os << uint16_t(0); // filecomment
		os << uint16_t(0); // disk# start
		os << uint16_t(0); // internal file
		os << uint32_t(0); // ext final
		os << uint32_t(header_offset); // rel offset
	}

	for(auto c : path) os << c;
}
