#include "zip_file_header.h"
#include "miniz.h"
#include <array>

namespace exs {
	


	class zip_streambuf_compress : public std::streambuf {
	public:
		zip_streambuf_compress(zip_file_header* central_header, obstream &os);
		virtual ~zip_streambuf_compress();

	protected:
		int process(bool flush);
		virtual int sync();
		virtual int underflow();
		virtual int overflow(int c = EOF);

	private:
		static constexpr std::size_t buffer_size = 512;

		obstream& os; // owned when header==0 (when not part of zip file)

		z_stream strm;
		std::array<char, buffer_size> in;
		std::array<char, buffer_size> out;

		zip_file_header* header;
		std::uint32_t uncompressed_size;
		std::uint32_t crc;

		bool valid;
	};


	class zip_streambuf_decompress : public std::streambuf {
	public:
		zip_streambuf_decompress(ibstream &is, const zip_file_header& central_header);
		virtual ~zip_streambuf_decompress();
		int process();
		virtual int underflow();
		virtual int overflow(int c = EOF);

	private:
		static constexpr std::size_t buffer_size = 512;

		ibstream& is;

		z_stream strm;
		std::array<char, buffer_size> in;
		std::array<char, buffer_size> out;
		zip_file_header header;
		std::size_t total_read;
		std::size_t total_uncompressed;
		bool valid;
		bool compressed_data;

		static const unsigned short DEFLATE = 8;
		static const unsigned short UNCOMPRESSED = 0;

	};



}