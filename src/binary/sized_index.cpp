#include "sized_index.h"

using namespace exs;

SizedIndex::SizedIndex(size_t _index, size_t _size) noexcept
	: _index(_index), _size(_size)
{}

size_t SizedIndex::index() const noexcept {
	return _index;
}

size_t SizedIndex::size() const noexcept {
	return _size;
}
