#pragma once
#include "binary.h"

namespace exs {

//	entity (actural data) of ConstBinary
	class ConstBinaryEntity : public ConstBinary {
	public:
		explicit ConstBinaryEntity(Size size) noexcept;
		ConstBinaryEntity(const BinaryEntity&) = delete;
		~ConstBinaryEntity() noexcept;
	};


//	entity (actual data) of Binary
	class BinaryEntity : public Binary {
	public:
		explicit BinaryEntity(Size size) noexcept;
		BinaryEntity(const BinaryEntity&) = delete;
		~BinaryEntity() noexcept;
	};

}
