#include "zip_streambuf.h"
using namespace exs;

zip_streambuf_decompress::zip_streambuf_decompress(ibstream &is, const zip_file_header& central_header)
	: is(is), header(central_header), total_read(0), total_uncompressed(0), valid(true)
{
	in.fill(0);
	out.fill(0);

	strm.zalloc = nullptr;
	strm.zfree = nullptr;
	strm.opaque = nullptr;
	strm.avail_in = 0;
	strm.next_in = nullptr;

	setg(in.data(), in.data(), in.data());
	setp(nullptr, nullptr);

	// skip the header
	zip_file_header(is, false);

	if (header.compression_type == DEFLATE) {
		compressed_data = true;
	}
	else if (header.compression_type == UNCOMPRESSED) {
		compressed_data = false;
	}
	else {
		compressed_data = false;
		throw std::runtime_error("unsupported compression type, should be DEFLATE or uncompressed");
	}

	// initialize the inflate
	if (compressed_data && valid)
	{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
		int result = inflateInit2(&strm, -MAX_WBITS);
#pragma clang diagnostic pop

		if (result != Z_OK) throw std::runtime_error("couldn't inflate ZIP, possibly corrupted");
	}

	header = central_header;
}

zip_streambuf_decompress::~zip_streambuf_decompress()
{
	if (compressed_data && valid) inflateEnd(&strm);
}

int zip_streambuf_decompress::process()
{
	if (!valid) return -1;

	if(compressed_data) {
		strm.avail_out = buffer_size - 4;
		strm.next_out = reinterpret_cast<Bytef *>(out.data() + 4);

		while(strm.avail_out != 0) {
			if(strm.avail_in == 0) {
				// buffer empty, read some more from file
				is.read(in.data(),
					static_cast<std::streamsize>(std::min(buffer_size, header.compressed_size - total_read)));
				strm.avail_in = static_cast<unsigned int>(is.gcount());
				total_read += strm.avail_in;
				strm.next_in = reinterpret_cast<Bytef *>(in.data());
			}

			const auto ret = inflate(&strm, Z_NO_FLUSH); // decompress

			if(ret == Z_STREAM_ERROR || ret == Z_NEED_DICT || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR) {
				throw std::runtime_error("couldn't inflate ZIP, possibly corrupted");
			}

			if(ret == Z_STREAM_END) break;
		}

		auto unzip_count = buffer_size - strm.avail_out - 4;
		total_uncompressed += unzip_count;
		return static_cast<int>(unzip_count);
	}

	// uncompressed, so just read
	is.read(out.data() + 4,
		static_cast<std::streamsize>(std::min(buffer_size - 4, header.uncompressed_size - total_read)));
	auto count = is.gcount();
	total_read += static_cast<std::size_t>(count);
	return static_cast<int>(count);
}

int zip_streambuf_decompress::underflow() {
	if(gptr() && (gptr() < egptr())) {
		return traits_type::to_int_type(*gptr()); // if we already have data just use it
	}
	auto put_back_count = gptr() - eback();
	if (put_back_count > 4) put_back_count = 4;
	std::memmove(out.data() + (4 - put_back_count), gptr() - put_back_count, static_cast<std::size_t>(put_back_count));
	int num = process();
	setg(out.data() + 4 - put_back_count, out.data() + 4, out.data() + 4 + num);
	if (num <= 0) return EOF;
	return traits_type::to_int_type(*gptr());
}

int zip_streambuf_decompress::overflow(int) {
	throw std::runtime_error("writing to read-only buffer");
}