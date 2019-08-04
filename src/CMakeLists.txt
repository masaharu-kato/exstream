cmake_minimum_required (VERSION 3.8)

add_library(zstream STATIC
	"miniz.cpp"
	"vector_streambuf.cpp"
	"zstream.cpp"
	"bstream.cpp"
	"bfstream.cpp"
)