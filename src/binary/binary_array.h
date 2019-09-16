#pragma once
#include "binary.h"

namespace exs {

	class const_binary_vector {
	public:
		class iterator;

		const_binary_vector(const const_binary_ref&, size_t elem_size);

		const_binary_ref operator [](size_t index) const;

		size_t size() const noexcept;

		iterator begin() const noexcept;
		iterator end() const noexcept;

	private:
		const_binary_ref p_binary;
	};

	

	class binary_vector {
	public:
		class iterator;

		binary_vector(const binary_ref&, size_t elem_size);
		
		const_binary_ref operator [](size_t index) const;
		binary_ref operator [](size_t index);

		size_t size() const noexcept;

		iterator begin() noexcept;
		iterator end() noexcept;

	private:
		binary_ref p_binary;
	};

}
