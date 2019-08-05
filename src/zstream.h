/*
PARTIO SOFTWARE
Copyright 2010 Disney Enterprises, Inc. All rights reserved

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in
the documentation and/or other materials provided with the
distribution.

* The names "Disney", "Walt Disney Pictures", "Walt Disney Animation
Studios" or the names of its contributors may NOT be used to
endorse or promote products derived from this software without
specific prior written permission from Walt Disney Pictures.

Disclaimer: THIS SOFTWARE IS PROVIDED BY WALT DISNEY PICTURES AND
CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE, NONINFRINGEMENT AND TITLE ARE DISCLAIMED.
IN NO EVENT SHALL WALT DISNEY PICTURES, THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND BASED ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
*/

#pragma once
#include <iostream>
#include <memory>
#include <unordered_map>
#include "zip_file_header.h"
#include "bstream.h"

namespace exs {

	using Path = std::string;

	// Writes a series of uncompressed binary file data as ostreams into another ostream
	// according to the ZIP format.
	class ozstream {
	public:
		ozstream(std::unique_ptr<streambuf> sb);
		virtual ~ozstream();

		// Returns a pointer to a streambuf which compresses the data it receives.
		std::unique_ptr<std::streambuf> open(const Path& file);

	private:
		std::vector<zip_file_header> file_headers;
		obstream os;
	};

	//	Reads an archive containing a number of files from an istream 
	//	and allows them to be decompressed into an istream.
	class izstream {
	public:
		izstream(std::unique_ptr<streambuf> sb);
		virtual ~izstream();

		std::unique_ptr<std::streambuf> open(const Path& file);
		std::string read(const Path& file);
		std::vector<Path> files() const;
		bool has_file(const Path& filename) const;

	private:
		bool read_central_header();

		std::unordered_map<std::string, zip_file_header> file_headers;
		ibstream is;
	};

}