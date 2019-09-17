#include "binary_entity.h"
using namespace exs;

ConstBinaryEntity::ConstBinaryEntity(size_t size) noexcept
	: Binary(new Byte[size], size)
{}

ConstBinaryEntity::~ConstBinaryEntity() noexcept
{
	delete[] data;
}


BinaryEntity::BinaryEntity(size_t size) noexcept
	: Binary(new Byte[size], size)
{}

BinaryEntity::~BinaryEntity() noexcept
{
	delete[] data;
}
