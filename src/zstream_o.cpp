#include "zstream.h"
#include "zexception.h"
#include "miniz.h"
#include "zip_streambuf.h"

using namespace exs;

ozstream::ozstream(std::unique_ptr<streambuf> sb)
    : os(std::make_unique<obstream>(std::move(sb)))
{
    if(!os) throw zexception("bad stream");
}

ozstream::~ozstream()
{
    // Write all file headers
    auto final_position = os->tellp();

    for(const auto& header : file_headers) {
		header.write(*os, true);
	}

    auto central_end = os->tellp();

    // Write end of central
    *os << uint32_t(0x06054b50);					// end of central
    *os << uint16_t(0);								// this disk number
    *os << uint16_t(0);								// this disk number
    *os << uint16_t(file_headers.size());			// one entry in center in this disk
    *os << uint16_t(file_headers.size());			// one entry in center
    *os << uint32_t(central_end - final_position);	// size of header
    *os << uint32_t(final_position);				// offset to header
    *os << uint16_t(0);								//	 zip comment
}

std::unique_ptr<streambuf> ozstream::open(const path_t& path)
{
    zip_file_header header;
    header.path = path;
    file_headers.push_back(header);

    return std::make_unique<zip_streambuf_compress>(&file_headers.back(), *os);
}
