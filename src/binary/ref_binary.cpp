#include "ref_binary.h"

using namespace exs;

ref_binary::ref_binary(const binary_ref& source, size_t index, size_t size) noexcept :
	binary(source.data() + index, size),
	source(source)
{}


const_ref_binary::const_ref_binary(const const_binary_ref& source, size_t index, size_t size) noexcept :
	binary(source.data() + index, size),
	source(source)
{}
