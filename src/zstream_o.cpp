#include "zstream.h"
#include <stdexcept>
#include "miniz.h"
#include "zip_streambuf.h"

using namespace exs;

ozstream::ozstream(std::unique_ptr<streambuf> sb)
    : os(std::move(sb))
{
    if (!os) throw std::runtime_error("Bad stream.");
}

ozstream::~ozstream()
{
    // Write all file headers
    auto final_position = os.tellp();

    for(const auto &header : file_headers) header.write(os, true);

    auto central_end = os.tellp();

    // Write end of central
    os << std::uint32_t(0x06054b50);					// end of central
    os << std::uint16_t(0);								// this disk number
    os << std::uint16_t(0);								// this disk number
    os << std::uint16_t(file_headers.size());			// one entry in center in this disk
    os << std::uint16_t(file_headers.size());			// one entry in center
    os << std::uint32_t(central_end - final_position);	// size of header
    os << std::uint32_t(final_position);				// offset to header
    os << std::uint16_t(0);								//	 zip comment
}

std::unique_ptr<std::streambuf> ozstream::open(const Path &filename)
{
    zip_file_header header;
    header.filename = filename;
    file_headers.push_back(header);
    auto buffer = new zip_streambuf_compress(&file_headers.back(), os);

    return std::unique_ptr<zip_streambuf_compress>(buffer);
}
