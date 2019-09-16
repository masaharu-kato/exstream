#pragma once
#include "binary.h"

namespace exs {

	class BinaryEntity : public Binary {
	public:
		BinaryEntity(size_t size) noexcept;
		BinaryEntity(const BinaryEntity&) = delete;
		~BinaryEntity() noexcept;

	};

}
