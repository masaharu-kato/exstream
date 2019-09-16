#include "ref_binary.h"

using namespace exs;

RefBinary::RefBinary(const BinaryRef& source, SizedIndex si) noexcept :
	binary(source.data() + si.index(), si.size()),
	source(source)
{}


ConstRefBinary::ConstRefBinary(const ConstBinaryRef& source, SizedIndex si) noexcept :
	binary(source.data() + si.index(), si.size()),
	source(source)
{}
