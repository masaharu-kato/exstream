#include "Binary.h"
#include "ref_binary.h"
using namespace exs;

//	ConstBinary::ConstBinary() noexcept = default;

ConstBinary::ConstBinary(Byte* data, Size size) noexcept : 
	_data(data),
	_size(size)
{}

ConstBinary::~ConstBinary() noexcept
{}

void ConstBinary::assert_of(SizedIndex si) const {
	if(si.index() + si.size() > (Index)_size) throw OutOfRange();
}

const Byte* ConstBinary::data_of(SizedIndex si) const {
	assert_of(si.index(), si.size());
	return _data + index;
}

ConstRefBinary ConstBinary::ref() const noexcept {
	return this;
}

ConstRefBinary ConstBinary::part_of(SizedIndex si) const {
	return ConstRefBinary::create(ref(), si);
}

Size ConstBinary::size() const noexcept {
	return _size;
}

const Byte* ConstBinary::data() const noexcept {
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



Byte* Binary::data_of(SizedIndex si) const {
	assert_of(si);
	return _data + si.index();
}

BinaryRef Binary::ref() noexcept {
	return this;
}

ConstBinaryRef Binary::c_ref() noexcept {
	return ConstBinary::ref();
}

BinaryRef Binary::part_of(SizedIndex si) {
	return RefBinary::create(ref(), si);
}

Byte* Binary::data() noexcept {
	return _data;
}

const Byte* Binary::c_data() const noexcept {
	return ConstBinary::data();
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
