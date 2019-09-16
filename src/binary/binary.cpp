#include "binary.h"
#include "ref_binary.h"
using namespace exs;

const_binary::const_binary() noexcept = default;

const_binary::const_binary(byte* data, size_t size) noexcept : 
	_data(data),
	_size(size)
{}

const_binary::~const_binary() noexcept
{}

void const_binary::assert_of(size_t index, size_t c_size) const {
	if(index + c_size > _size) throw out_of_range();
}

auto const_binary::data_of(size_t index, size_t c_size) const -> const byte* {
	assert_of(index, c_size);
	return _data + index;
}

const_binary_ref const_binary::ref() const noexcept {
	return this;
}

const_binary_ref const_binary::part_of(size_t index, size_t c_size) const {
	return const_ref_binary::create(ref(), index, c_size);
}

size_t const_binary::size() const noexcept {
	return _size;
}

auto const_binary::data() const noexcept -> const byte* {
	return _data;
}

const_binary::set_reference() noexcept {
	++refed_count;
}

const_binary::unset_reference() noexcept
{
	if(refed_count) --refed_count;
	if(refed_count == 0) delete this;
}



auto binary::data_of(size_t index, size_t c_size) const -> byte* {
	assert_of(index, c_size);
	return _data + index;
}

binary_ref binary::ref() noexcept {
	return this;
}

binary_ref binary::part_of(size_t index, size_t c_size) {
	return ref_binary::create(ref(), index, c_size);
}

auto binary::data() noexcept -> data_t {
	return _data;
}

auto binary::c_data() const noexcept -> const_binary::data_t {
	return _data;
}



const_binary_ref::const_binary_ref(const_binary* p_binary) noexcept :
	p_binary(p_binary)
{
	p_binary->set_reference();
}

const_binary_ref::~const_binary_ref() noexcept {
	p_binary->unset_reference();
}

const_binary* const_binary_ref::operator ->() const noexcept {
	return p_binary;
}



binary_ref::binary_ref(binary* p_binary) noexcept :
	p_binary(p_binary)
{
	p_binary->set_reference();
}

binary_ref::~binary_ref() noexcept {
	p_binary->unset_reference();
}

binary* binary_ref::operator ->() const noexcept {
	return p_binary;
}
