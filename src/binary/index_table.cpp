#include "index_table.h"

using namespace exs;

void IndexTableBase::throw_out_of_range() const {
	throw OutOfRange();
}

SizedIndex IndexTable::operator[](size_t i) const {
	return ptr->operator[](i);
}

Size IndexTable::size() const {
	return ptr->size();
}

IndexTable::IndexTable(std::shared_ptr<IndexTableBase> ptr)
	: ptr(ptr)
{}

IndexTableIterator IndexTable::begin() const {
	return {ptr, 0};
}

IndexTableIterator IndexTable::end() const {
	return {ptr, ptr->size() - 1};
}

IndexTableIterator::IndexTableIterator(std::shared_ptr<IndexTableBase> ptr, Index index)
	: ptr(ptr), index(index)
{}

IndexTableIterator IndexTableIterator::operator++() {
	++index;
	return *this;
}

IndexTableIterator IndexTableIterator::operator--() {
	--index;
	return *this;
}

IndexTable& IndexTableIterator::operator*() const {
	return *this;
}


SizedIndex UniformIndexTable::operator[](Index i) const {
	if(i >= elem_count) throw_out_of_range();
	return {i * elem_size, elem_size};
}

SizedIndex UniformIndexTable::size() const {
	return elem_count;
}

SizedIndex PaddedUniformIndexTable::operator[](Index i) const {
	if(i >= elem_count) throw_out_of_range();
	return {i * unit_size, elem_size};
}

Size UnevenIndexTable::size() const {
	return sized_index_of.size();
}

UnevenIndexTable::UnevenIndexTable(const std::vector<Index>& indexes) noexcept
	: sized_index_table(sizes)
{}

UnevenIndexTable::UnevenIndexTable(const std::vector<Size>& sizes) noexcept
	: sized_index_table(sizes)
{}

SizedIndex UnevenIndexTable::operator[](Index i) const {
	if(i >= sized_index_table.size()) throw_out_of_range();
	return sized_index_table[i];
}
