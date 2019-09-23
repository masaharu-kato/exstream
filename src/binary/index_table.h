#pragma once
#include <vector>
#include <memory>
#include <stdexcept>
#include "sized_index.h"

namespace exs {

	class IndexTableIterator;

//	index-table base class
	class IndexTableBase {
	public:
		virtual SizedIndex operator [](Index) const = 0;
		virtual Size size() const = 0;
		
		void throw_out_of_range() const;

	//	out-of-range exception
		class OutOfRange : public std::out_of_range {
		};

	protected:
		IndexTableBase() = default;

	};


//	main index-table class (reference of IndexTableBase)
	class IndexTable {
	public:
		IndexTable(IndexTableBase*);

		SizedIndex operator [](size_t) const;
		Size size() const;

		IndexTableIterator begin() const;
		IndexTableIterator end() const;

	private:
		std::shared_ptr<IndexTableBase> ptr;
	};


//	iterator of IndexTable
	class IndexTableIterator : public IndexTable {
	public:
		IndexTableIterator(IndexTable, Index);

		IndexTableIterator& operator ++();
		IndexTableIterator& operator --();
		SizedIndex operator *() const;
		SizedIndex* operator ->() const;

	private:
		Index index = 0;
	};


//	uniform-sized index table derived class
	class UniformIndexTable : public IndexTableBase {
	public:
		virtual SizedIndex operator [](Index) const override;
		virtual Size size() const override;

	protected:
	//	size (length) of each single element
		Size elem_size;

	//	count (number) of elements
		Size elem_count;

		UniformIndexTable(Size elem_size, Size elem_count)
			: elem_size(elem_size), elem_count(elem_count) {}
	};


//	padded, uniform-sized index table derived class
	class PaddedUniformIndexTable : public UniformIndexTable {
	public:
		virtual SizedIndex operator [](Index) const override;

	private:
	//	size (length) between each two elements
		Size unit_size;

		PaddedUniformIndexTable(Size elem_size, Size unit_size, Size elem_count)
			: UniformIndexTable(elem_size, elem_count), unit_size(unit_size) {}
	};


//	uneven(non-uniformed)-sized index table derived class
	class UnevenIndexTable : public IndexTableBase {
	public:
		virtual SizedIndex operator [](Index) const override;
		virtual Size size() const override;

	protected:
		template <class... Args>
		UnevenIndexTable(Args&&... args);

	private:
		SizedIndexTable sized_index_table;
	};


	
	template <class... Args>
	UnevenIndexTable::UnevenIndexTable(Args&&... args)
		: sized_index_table(args...)
	{}


}
