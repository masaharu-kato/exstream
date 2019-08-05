#include "zstream.h"
#include <stdexcept>
#include "miniz.h"
#include "vector_streambuf.h"
#include "zip_streambuf.h"

using namespace exs;

izstream::izstream(std::unique_ptr<streambuf> sb)
    : is(std::move(sb))
{
    if(!is) throw std::runtime_error("Invalid file handle");
    read_central_header();
}

izstream::~izstream() {}

bool izstream::read_central_header()
{
    // Find the header
    // NOTE: this assumes the zip file header is the last thing written to file...
    is.seekg(0, std::ios_base::end);
    auto end_position = static_cast<std::size_t>(is.tellg());

    auto max_comment_size = std::uint32_t(0xffff); // max size of header
    auto read_size_before_comment = std::uint32_t(22);

    std::uint32_t read_start = max_comment_size + read_size_before_comment;

    if(read_start > end_position) read_start = (std::uint32_t)end_position;

    is.seekg(end_position - read_start);
    std::vector<std::uint8_t> buf(static_cast<std::size_t>(read_start), '\0');

    if(read_start <= 0) throw std::runtime_error("file is empty");

    is.read(reinterpret_cast<char *>(buf.data()), read_start);

    if(buf[0] == 0xd0 && buf[1] == 0xcf && buf[2] == 0x11 && buf[3] == 0xe0 && buf[4] == 0xa1 && buf[5] == 0xb1 && buf[6] == 0x1a && buf[7] == 0xe1) throw std::runtime_error("encrypted xlsx, password required");

    auto found_header = false;
    std::size_t header_index = 0;

    for (std::size_t i = 0; i < static_cast<std::size_t>(read_start - 3); ++i) {
        if (buf[i] == 0x50 && buf[i + 1] == 0x4b && buf[i + 2] == 0x05 && buf[i + 3] == 0x06) {
            found_header = true;
            header_index = i;
            break;
        }
    }

    if (!found_header) throw std::runtime_error("failed to find zip header");

    // seek to end of central header and read
    is.seekg(end_position - (read_start - static_cast<std::ptrdiff_t>(header_index)));

	is.skip<std::uint32_t>();	//	word
    auto disk_number1 = is.get<std::uint16_t>();
    auto disk_number2 = is.get<std::uint16_t>();

    if (disk_number1 != disk_number2 || disk_number1 != 0) throw std::runtime_error("multiple disk zip files are not supported");

    auto num_files = is.get<std::uint16_t>(); // one entry in center in this disk
    auto num_files_this_disk = is.get<std::uint16_t>(); // one entry in center

    if (num_files != num_files_this_disk) throw std::runtime_error("multi disk zip files are not supported");

    is.skip<std::uint32_t>(); // size of header
    auto header_offset = is.get<std::uint32_t>(); // offset to header

    // go to header and read all file headers
    is.seekg(header_offset);

    for (std::uint16_t i = 0; i < num_files; ++i)
    {
        zip_file_header header(is, true);
        file_headers[header.filename] = header;
    }

    return true;
}

std::unique_ptr<std::streambuf> izstream::open(const Path &filename)
{
    if (!has_file(filename)) throw std::runtime_error("file not found");

    auto header = file_headers.at(filename);
    is.seekg(header.header_offset);
    auto buffer = new zip_streambuf_decompress(is, header);

    return std::unique_ptr<zip_streambuf_decompress>(buffer);
}

std::string izstream::read(const Path &filename)
{
    auto buffer = open(filename);
    std::istream stream(buffer.get());
    auto bytes = to_vector(stream);

    return std::string(bytes.begin(), bytes.end());
}

std::vector<Path> izstream::files() const
{
    std::vector<Path> filenames;
    std::transform(file_headers.begin(), file_headers.end(), std::back_inserter(filenames),
        [](const std::pair<std::string, zip_file_header> &h) { return Path(h.first); });

    return filenames;
}

bool izstream::has_file(const Path &filename) const
{
    return file_headers.count(filename) != 0;
}
