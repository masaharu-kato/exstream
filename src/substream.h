#pragma once
#include "stream.h"
#include "rptr/rented_ptr.h"
#include <memory>

namespace exs {

	class substream_entity {
	public:
		substream(rented_ptr<stream>&&) noexcept;

		char get();
		bool eof() const;

	protected:
		virtual bool is_eof_char(char ch) = 0;

	private:
		rented_ptr<stream> _stream;
	};

	
	class substream : public std::unique_ptr<substream_entity> {
	public:
		char get();
		bool eof() const;
	};


}
