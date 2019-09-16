#include "padded_binary.h"

using namespace exs;

void _padded_utilty::assert_of(size_t index, size_t size) const {
	if(size > elem_size) throw const_binary::out_of_range();
}

size_t _padded_utilty::index_of(size_t index) const noexcept {
	return index * unit_size;
}



const_padded_ref_binary::const_padded_ref_binary(const const_binary_ref& bref, size_t index, size_t size, size_t elem_size, size_t unit_size) noexcept
	: const_ref_binary(bref, index, size), _padded_utilty(elem_size, unit_size)
{}


auto const_padded_ref_binary::data_of(size_t index, size_t size) const -> const byte*
{
	_padded_utilty::assert_of(index, size);
	return data_of(_padded_utilty::index_of(index), size);
}


padded_ref_binary::padded_ref_binary(const binary_ref& bref, size_t index, size_t size, size_t elem_size, size_t unit_size) noexcept
	: ref_binary(bref, index, size), _padded_utilty(elem_size, unit_size)
{}


auto padded_ref_binary::data_of(size_t index, size_t size) const -> byte*
{
	_padded_utilty::assert_of(index, size);
	return data_of(_padded_utilty::index_of(index), size);
}
