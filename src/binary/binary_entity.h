#pragma once
#include "binary.h"

namespace exs {

	class binary_entity : public binary {
	public:
		binary_entity(size_t size) noexcept;
		binary_entity(const binary_entity&) = delete;
		~binary_entity() noexcept;

	};

}
