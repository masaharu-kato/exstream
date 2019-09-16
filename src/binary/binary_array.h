#pragma once
#include "binary.h"
#include "index_table.h"
#include <memory>

namespace exs {


	class ConstBinaryVector {
	public:
		class Iterator;

		ConstBinaryVector(ConstBinaryRef, std::shared_ptr<IndexTable>);

		ConstBinaryRef operator [](size_t index) const;

		size_t size() const noexcept;

		Iterator begin() const noexcept;
		Iterator end() const noexcept;

	private:
		ConstBinaryRef binary_ref;
		std::shared_ptr<IndexTable> p_index_table;
	};

	

	class BinaryVector {
	public:
		class Iterator;

		BinaryVector(BinaryRef, std::shared_ptr<IndexTable>);
		
		ConstBinaryRef operator [](size_t index) const;
		BinaryRef operator [](size_t index);

		size_t size() const noexcept;

		Iterator begin() noexcept;
		Iterator end() noexcept;

	private:
		BinaryRef binary_ref;
		std::shared_ptr<IndexTable> p_index_table;
	};

}
