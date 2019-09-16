#include "Binary.h"
#include "ref_binary.h"
using namespace exs;

ConstBinary::ConstBinary() noexcept = default;

ConstBinary::ConstBinary(Byte* data, size_t size) noexcept : 
	_data(data),
	_size(size)
{}

ConstBinary::~ConstBinary() noexcept
{}

void ConstBinary::assert_of(SizedIndex si) const {
	if(si.index() + si.size() > _size) throw out_of_range();
}

auto ConstBinary::data_of(SizedIndex si) const -> const Byte* {
	assert_of(si.index(), si.size());
	return _data + index;
}

ConstRefBinary ConstBinary::ref() const noexcept {
	return this;
}

ConstRefBinary ConstBinary::part_of(SizedIndex si) const {
	return ConstRefBinary::create(ref(), si);
}

size_t ConstBinary::size() const noexcept {
	return _size;
}

auto ConstBinary::data() const noexcept -> const Byte* {
	return _data;
}

ConstBinary::set_reference() noexcept {
	++refed_count;
}

ConstBinary::unset_reference() noexcept
{
	if(refed_count) --refed_count;
	if(refed_count == 0) delete this;
}



auto Binary::data_of(SizedIndex si) const -> Byte* {
	assert_of(si);
	return _data + si.index();
}

BinaryRef Binary::ref() noexcept {
	return this;
}

BinaryRef Binary::part_of(size_t index, size_t c_size) {
	return ref_binary::create(ref(), index, c_size);
}

auto Binary::data() noexcept -> data_t {
	return _data;
}

auto Binary::c_data() const noexcept -> ConstBinary::data_t {
	return _data;
}



ConstBinaryRef::ConstBinaryRef(ConstBinary* p_binary) noexcept :
	p_binary(p_binary)
{
	p_binary->set_reference();
}

ConstBinaryRef::~ConstBinaryRef() noexcept {
	p_binary->unset_reference();
}

ConstBinary* ConstBinaryRef::operator ->() const noexcept {
	return p_binary;
}



BinaryRef::BinaryRef(Binary* p_binary) noexcept :
	p_binary(p_binary)
{
	p_binary->set_reference();
}

BinaryRef::~BinaryRef() noexcept {
	p_binary->unset_reference();
}

Binary* BinaryRef::operator ->() const noexcept {
	return p_binary;
}
