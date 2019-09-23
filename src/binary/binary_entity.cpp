#include "binary_entity.h"
using namespace exs;

ConstBinaryEntity::ConstBinaryEntity(Size size) noexcept
	: Binary(new Byte[size], size)
{}

ConstBinaryEntity::~ConstBinaryEntity() noexcept {
	delete[] data;
}


BinaryEntity::BinaryEntity(Size size) noexcept
	: Binary(new Byte[size], size)
{}

BinaryEntity::~BinaryEntity() noexcept {
	delete[] data;
}
