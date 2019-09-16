#include "binary_entity.h"
using namespace exs;

binary_entity::binary_entity(size_t size) noexcept : 
	binary(new byte[size], size)
{}

binary_entity::~binary_entity() noexcept
{
	delete[] data;
}
