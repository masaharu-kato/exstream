#pragma once
#include <vector>
#include <memory>

namespace exs {


	class IndexTableBase {
	public:
		virtual size_t index_of(size_t) const = 0;
		virtual size_t size_of(size_t) const = 0;
		virtual size_t size() const = 0;

		class Iterator;
	};

	class IndexTable {
	public:
		std::shared_ptr<IndexTableBase> ptr;
	};

	class IndexTableIterator : public IndexTable {
	public:
		IndexTableIterator operator ++();
		IndexTableIterator operator --();
		 operator *() const;


	private:
		size_t index = 0;
	};


	class UniformIndexTable : public IndexTableBase {
	public:
		virtual size_t index_of(size_t) const override;
		virtual size_t size_of(size_t) const override;
		virtual size_t size() const override;

	private:
		size_t elem_size;
	};

	class PaddedUniformIndexTable : public IndexTableBase {
	public:
		virtual size_t index_of(size_t) const override;

	private:
		size_t unit_size;
	};


	class UnevenIndexTable : public IndexTableBase {
	public:
		virtual size_t index_of(size_t) const override;
		virtual size_t size_of(size_t) const override;
		virtual size_t size() const override;

	private:
		std::vector<size_t> _index_of;
	};


}
