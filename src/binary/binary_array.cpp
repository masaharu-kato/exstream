#include "binary_array.h"

using namespace exs;

ConstBinaryVector::ConstBinaryVector(ConstBinaryRef const_binary_ref, SizedIndexTable sized_index_table)
	 const_binary_ref(const_binary_ref), sized_index_table(sized_index_table)
{}

ConstBinaryRef ConstBinaryVector::operator [](Index index) const
{
	return const_binary_ref->part_of(sized_index_table[index]);
}

Size ConstBinaryVector::size() const noexcept
{
	return sized_index_table.size();
}

auto ConstBinaryVector::begin() const noexcept -> Iterator
{
	return {const_binary_ref, sized_index_table.begin()};
}

auto ConstBinaryVector::end() const noexcept -> Iterator
{
	return {const_binary_ref, sized_index_table.end()};
}



BinaryVector::BinaryVector(BinaryRef binary_ref, SizedIndexTable sized_index_table)
	 binary_ref(binary_ref), sized_index_table(sized_index_table)
{}

ConstBinaryRef BinaryVector::operator [](Index index) const
{
	return binary_ref->part_of(sized_index_table[index]);
}

BinaryRef BinaryVector::operator [](Index index)
{
	return binary_ref->part_of(sized_index_table[index]);
}

Size BinaryVector::size() const noexcept
{
	return sized_index_table.size();
}

auto BinaryVector::begin() const noexcept -> Iterator
{
	return {binary_ref, sized_index_table.begin()};
}

auto BinaryVector::end() const noexcept -> Iterator
{
	return {binary_ref, sized_index_table.end()};
}

ConstBinaryVector::Iterator::Iterator(ConstBinaryRef const_binary_ref, SizedIndexTable::Iterator itr)
	: const_binary_ref(const_binary_ref), SizedIndexTable::Iterator(itr)
{}

ConstBinaryRef ConstBinaryVector::Iterator::operator *() const {
	return const_binary_ref->part_of(SizedIndexTable::Iterator::operator *());
}

BinaryVector::Iterator::Iterator(BinaryRef binary_ref, SizedIndexTable::Iterator itr)
	: binary_ref(binary_ref), SizedIndexTable::Iterator(itr)
{}

BinaryRef BinaryVector::Iterator::operator *() const {
	return binary_ref->part_of(SizedIndexTable::Iterator::operator *());
}

