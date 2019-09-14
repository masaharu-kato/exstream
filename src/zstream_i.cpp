#include "zstream.h"
#include "zexception.h"
#include "miniz.h"
#include "vector_streambuf.h"
#include "zip_streambuf.h"

using namespace exs;

izstream::izstream(std::unique_ptr<streambuf> sb)
    : is(std::make_unique<ibstream>(std::move(sb)))
{
    if(!*is) throw zexception("Invalid file handle");
    read_central_header();
}

izstream::~izstream() {}

bool izstream::read_central_header()
{
    // Find the header
    // NOTE: this assumes the zip file header is the last thing written to file...
    is->seekg(0, std::ios_base::end);
    auto end_position = size_t(is->tellg());

    auto max_comment_size = uint32_t(0xffff); // max size of header
    auto read_size_before_comment = uint32_t(22);

    uint32_t read_start = max_comment_size + read_size_before_comment;

    if(read_start > end_position) read_start = uint32_t(end_position);

    is->seekg(end_position - read_start);
    std::vector<uint8_t> buf(size_t(read_start), '\0');

    if(read_start <= 0) throw zexception("file is empty");

    is->read(reinterpret_cast<char *>(buf.data()), read_start);

    if(    buf[0] == 0xd0 && buf[1] == 0xcf && buf[2] == 0x11 && buf[3] == 0xe0
		&& buf[4] == 0xa1 && buf[5] == 0xb1 && buf[6] == 0x1a && buf[7] == 0xe1){
		throw zexception("encrypted zip file.");
	}

    bool found_header = false;
    size_t header_index = 0;

    for(size_t i = 0; i < size_t(read_start - 3); ++i) {
        if (buf[i] == 0x50 && buf[i + 1] == 0x4b && buf[i + 2] == 0x05 && buf[i + 3] == 0x06) {
            found_header = true;
            header_index = i;
            break;
        }
    }

    if(!found_header) throw zexception("failed to find zip header");

    // seek to end of central header and read
    is->seekg(end_position - (read_start - std::ptrdiff_t(header_index)));

	is->skip<uint32_t>();	//	word
    auto disk_number1 = is->get<uint16_t>();
    auto disk_number2 = is->get<uint16_t>();

    if(disk_number1 != disk_number2 || disk_number1 != 0) throw zexception("multiple disk zip files are not supported");

    auto num_files = is->get<uint16_t>(); // one entry in center in this disk
    auto num_files_this_disk = is->get<uint16_t>(); // one entry in center

    if(num_files != num_files_this_disk) throw zexception("multi disk zip files are not supported");

    is->skip<uint32_t>(); // size of header
    auto header_offset = is->get<uint32_t>(); // offset to header

    // go to header and read all file headers
    is->seekg(header_offset);

    for(uint16_t i = 0; i < num_files; ++i) {
        zip_file_header header(*is, true);
        file_headers[header.path] = header;
    }

    return true;
}

std::unique_ptr<streambuf> izstream::open(const path_t& path)
{
    if(!has_file(path)) throw std::runtime_error("file not found");
	if(!is) throw std::runtime_error("stream is in use.");

    auto header = file_headers.at(path);
    is->seekg(header.header_offset);

    return std::make_unique<zip_streambuf_decompress>(*is, header);
}

std::string izstream::read(const path_t& path)
{
    auto buffer = open(path);
    std::istream stream(buffer.get());
    auto bytes = to_vector(stream);

    return std::string(bytes.begin(), bytes.end());
}

auto izstream::files() const -> std::vector<path_t>
{
    std::vector<path_t> paths;
    std::transform(file_headers.begin(), file_headers.end(), std::back_inserter(paths),
        [](const std::pair<path_t, zip_file_header> &h) { return h.first; });

    return paths;
}

bool izstream::has_file(const path_t &path) const
{
    return file_headers.count(path) != 0;
}
