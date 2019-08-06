#include "zip_streambuf.h"
using namespace exs;

zip_streambuf_compress::zip_streambuf_compress(zip_file_header* central_header, obstream &os)
	: os(os), header(central_header), valid(true)
{
	strm.zalloc = nullptr;
	strm.zfree = nullptr;
	strm.opaque = nullptr;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
	int ret = deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, -MAX_WBITS, 8, Z_DEFAULT_STRATEGY);
#pragma clang diagnostic pop

	if (ret != Z_OK)
	{
		std::cerr << "libz: failed to deflateInit" << std::endl;
		valid = false;
		return;
	}

	setg(nullptr, nullptr, nullptr);
	setp(in.data(), in.data() + buffer_size - 4); // we want to be 4 aligned

	// Write appropriate header
	if(header) {
		header->header_offset = static_cast<std::uint32_t>(os.tellp());
		header->write(os, false);
	}

	uncompressed_size = crc = 0;
}

zip_streambuf_compress::~zip_streambuf_compress()
{
	if(valid) {
		process(true);
		deflateEnd(&strm);
		if(header) {
			auto final_position = os.tellp();
			header->uncompressed_size = uncompressed_size;
			header->crc = crc;
			os.seekp(header->header_offset);
			header->write(os, false);
			os.seekp(final_position);
		}else {
			os << crc;
			os << uncompressed_size;
		}
	}
	if(!header) delete &os;
}

int zip_streambuf_compress::process(bool flush)
{
	if (!valid) return -1;

	strm.next_in = reinterpret_cast<Bytef *>(pbase());
	strm.avail_in = static_cast<unsigned int>(pptr() - pbase());

	while(strm.avail_in != 0 || flush) {
		strm.avail_out = buffer_size;
		strm.next_out = reinterpret_cast<Bytef *>(out.data());

		int ret = deflate(&strm, flush ? Z_FINISH : Z_NO_FLUSH);

		if (!(ret != Z_BUF_ERROR && ret != Z_STREAM_ERROR)){
			valid = false;
			std::cerr << "gzip: gzip error " << strm.msg << std::endl;
			return -1;
		}

		auto generated_output = static_cast<int>(strm.next_out - reinterpret_cast<std::uint8_t *>(out.data()));
		os.write(out.data(), generated_output);
		if (header) header->compressed_size += static_cast<std::uint32_t>(generated_output);
		if (ret == Z_STREAM_END) break;
	}

	// update counts, crc's and buffers
	auto consumed_input = static_cast<std::uint32_t>(pptr() - pbase());
	uncompressed_size += consumed_input;
	crc = static_cast<std::uint32_t>(crc32(crc, reinterpret_cast<Bytef *>(in.data()), consumed_input));
	setp(pbase(), pbase() + buffer_size - 4);

	return 1;
}

int zip_streambuf_compress::overflow(int c)
{
	if(c != EOF) {
		*pptr() = static_cast<char>(c);
		pbump(1);
	}
	if(process(false) == EOF) return EOF;
	return c;
}

int zip_streambuf_compress::sync()
{
	if (pptr() && pptr() > pbase()) return process(false);
	return 0;
}

int zip_streambuf_compress::underflow()
{
	throw std::logic_error("Attempt to read write only ostream");
}