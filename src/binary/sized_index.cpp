#include "sized_index.h"

using namespace exs;

SizedIndex::SizedIndex(Index _index, Size _size) noexcept
	: _index(_index), _size(_size)
{}

SizedIndex::SizedIndex(Size _size, Index _index) noexcept
	: _index(_index), _size(_size)
{}

Index SizedIndex::index() const noexcept {
	return _index;
}

Size SizedIndex::size() const noexcept {
	return _size;
}

SizedIndexTable::SizedIndexTable(const std::vector<Index>& indexes, Size whole_size)
{
	size_t count = indexes.size();

	for(size_t i = 0; i < count; ++i) {
		Index i_beg = indexes[i];
		Index i_end = (i < count - 1) ? indexes[i+1] : whole_size;
		if(i_beg > i_end) throw InvalidValues();
		emplace_back(i_beg, i_end - i_beg);
	}

}

SizedIndexTable::SizedIndexTable(const std::vector<Size>& sizes, Index offset = 0)
{
	size_t count = sizes.count();

	Index c_index = offset;
	for(size_t i = 0; i < count; ++i) {
		Size c_size = sizes[i];
		emplace_back(c_index, c_size);
		c_index += c_size;
	}
}

