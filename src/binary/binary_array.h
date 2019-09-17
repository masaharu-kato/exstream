#pragma once
#include "binary.h"
#include "index_table.h"
#include <memory>

namespace exs {


	class ConstBinaryVector {
	public:
		class Iterator;

		ConstBinaryVector(ConstBinaryRef, SizedIndexTable);

		ConstBinaryRef operator [](Index index) const;

		Size size() const noexcept;

		Iterator begin() const noexcept;
		Iterator end() const noexcept;

	private:
		ConstBinaryRef const_binary_ref;
		SizedIndexTable sized_index_table;
	};

	

	class BinaryVector {
	public:
		class Iterator;

		BinaryVector(BinaryRef, SizedIndexTable);
		
		ConstBinaryRef operator [](Index index) const;
		BinaryRef operator [](Index index);

		Size size() const noexcept;

		Iterator begin() noexcept;
		Iterator end() noexcept;

	private:
		BinaryRef binary_ref;
		SizedIndexTable sized_index_table;
	};


	class ConstBinaryVector::Iterator : public SizedIndexTable::Iterator {
	public:
		Iterator(ConstBinaryRef, SizedIndexTable::Iterator);

		ConstBinaryRef operator *() const;

	private:
		ConstBinaryRef const_binary_ref;
	};


	class BinaryVector::Iterator : public SizedIndexTable::Iterator {
	public:
		Iterator(BinaryRef, SizedIndexTable::Iterator);

		BinaryRef operator *() const;

	private:
		BinaryRef binary_ref;
	};

}
