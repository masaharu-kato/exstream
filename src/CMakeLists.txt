cmake_minimum_required (VERSION 3.8)

add_library(exstream STATIC
	"filebuf.cpp"
	"bfilebuf.cpp"
	"stream.cpp"
	"bstream.cpp"
	"fstream.cpp"
	"bfstream.cpp"
	"vector_streambuf.cpp"
	"miniz.cpp"
	"zstream_i.cpp"
	"zstream_o.cpp"
	"zfstream.cpp"
	"zip_file_header.cpp"
	"zip_streambuf_compress.cpp"
	"zip_streambuf_decompress.cpp"
)