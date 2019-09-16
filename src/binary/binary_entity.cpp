#include "binary_entity.h"
using namespace exs;

BinaryEntity::BinaryEntity(size_t size) noexcept : 
	Binary(new byte[size], size)
{}

BinaryEntity::~BinaryEntity() noexcept
{
	delete[] data;
}
