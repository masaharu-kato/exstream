#pragma once
#include <vector>
#include <memory>
#include <stdexcept>
#include "sized_index.h"

namespace exs {

	class IndexTableIterator;

	class IndexTableBase {
	public:
		virtual SizedIndex operator [](Index) const = 0;
		virtual Size size() const = 0;
		
		void throw_out_of_range() const;

		class OutOfRange : public std::out_of_range {
		};
	};

	class IndexTable {
	public:
		IndexTable(std::shared_ptr<IndexTableBase>);

		SizedIndex operator [](size_t) const;
		Size size() const;

		IndexTableIterator begin() const;
		IndexTableIterator end() const;

	private:
		std::shared_ptr<IndexTableBase> ptr;
	};

	class IndexTableIterator : public IndexTable {
	public:
		IndexTableIterator(std::shared_ptr<IndexTableBase>, Index);

		IndexTableIterator& operator ++();
		IndexTableIterator& operator --();
		IndexTable& operator *() const;

	private:
		Index index = 0;
	};


	class UniformIndexTable : public IndexTableBase {
	public:
		virtual SizedIndex operator [](Index) const override;
		virtual Size size() const override;

	protected:
		Size elem_size;
		Size elem_count;
	};

	class PaddedUniformIndexTable : public UniformIndexTable {
	public:
		virtual SizedIndex operator [](Index) const override;

	private:
		Size unit_size;
	};


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
