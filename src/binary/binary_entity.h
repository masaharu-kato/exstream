#pragma once
#include "binary.h"

namespace exs {

	class ConstBinaryEntity : public ConstBinary {
	public:
		explicit ConstBinaryEntity(size_t size) noexcept;
		ConstBinaryEntity(const BinaryEntity&) = delete;
		~ConstBinaryEntity() noexcept;
	};

	class BinaryEntity : public Binary {
	public:
		explicit BinaryEntity(size_t size) noexcept;
		BinaryEntity(const BinaryEntity&) = delete;
		~BinaryEntity() noexcept;
	};

}
