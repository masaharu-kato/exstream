#include "binary_array.h"

using namespace exs;

ConstBinaryVector::ConstBinaryVector(
	ConstBinaryRef binary_ref,
	std::shared_ptr<IndexTable> p_index_table
) :
	binary_ref(binary_ref),
	p_index_table(p_index_table)
{}


ConstBinaryRef ConstBinaryVector::operator [](size_t index) const
{
	return binary_ref->data_of(
		p_index_table->index_of(index),
		p_index_table->size_of(index)
	);
}

size_t ConstBinaryVector::size() const noexcept
{
	return p_index_table->size();
}

auto ConstBinaryVector::begin() const noexcept -> Iterator
{
	return p_index_table;
}

auto ConstBinaryVector::end() const noexcept -> Iterator
{

}
