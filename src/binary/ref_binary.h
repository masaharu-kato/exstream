#pragma once
#include "binary.h"

namespace exs {

	class ref_binary : public binary {
	public:
		template <class... Args>
		ref_binary* create(Args&&... args) {
			return new ref_binary(args...);
		}

	protected:
		ref_binary(const binary_ref&, size_t index, size_t size) noexcept; 

	private:
		binary_ref source;
	};


	class const_ref_binary : public const_binary {
	public:
		template <class... Args>
		const_ref_binary* create(Args&&... args) {
			return new const_ref_binary(args...);
		}

	protected:
		const_ref_binary(const const_binary_ref&, size_t index, size_t size) noexcept; 

	private:
		const_binary_ref source;
	};

}
