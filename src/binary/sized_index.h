#pragma once
#include <vector>
#include <stdexcept>

namespace exs {

	class Index;

	class Size {
	private:
		size_t size;
	public:
		Size(size_t size) noexcept : size(size) {}
		Size(Index) = delete;

		operator size_t() const noexcept { return size; }
		operator size_t&() noexcept { return size; }
	};

	
	class Index {
	private:
		size_t index;
	public:
		Index(size_t index) noexcept : index(index) {}
		Index(Size) = delete;
		
		Index operator +(const Size& i) const noexcept;
		Size operator -(const Index& i) const noexcept;
		Index operator -(const Size& i) const noexcept;

		auto& operator +=(const Size& i) const noexcept;
		auto& operator -=(const Index& i) const noexcept;
		auto& operator -=(const Size& i) const noexcept;


		auto operator *(const Size& s) const noexcept {
			return Index(index * (size_t)s);
		}

		operator size_t() const noexcept { return index; }
		operator size_t&() noexcept { return index; }
	};


	class SizedIndex {
	public:
		SizedIndex(Index _index, Size _size) noexcept;
		SizedIndex(Size _size, Index _index) noexcept;

		Index index() const noexcept;
		Size size() const noexcept;

	private:
		Index _index;
		Size _size;
	};


	class SizedIndexTable : public std::vector<SizedIndex> {
	public:
		SizedIndexTable(const std::vector<Index>&, Size whole_size);
		SizedIndexTable(const std::vector<Size>&, Index offset = 0);

		class InvalidValues : public std::runtime_error {
		};

		using Iterator = iterator;
	};


}
