#pragma once
#include <vector>
#include <stdexcept>

namespace exs {

	class Index;
	class Size;

//	size value class
	class Size {
	private:
		size_t size;
	public:
		Size(size_t size) noexcept : size(size) {}
		Size(Index) = delete;

		operator size_t() const noexcept { return size; }
		operator size_t&() noexcept { return size; }
	};

	
//	index value class
	class Index {
	private:
		size_t index;
	public:
		Index(size_t index) noexcept : index(index) {}
		Index(Size) = delete;
		
		Index operator +(const Size&) const noexcept;
		Size operator -(const Index&) const noexcept;
		Index operator -(const Size&) const noexcept;

		Index& operator +=(const Size&) const noexcept;
		Index& operator -=(const Index&) const noexcept;
		Index& operator -=(const Size&) const noexcept;

		auto operator *(const Size& s) const noexcept {
			return Index(index * (size_t)s);
		}


		operator size_t() const noexcept { return index; }
		operator size_t&() noexcept { return index; }
	};








//	pair of size value and index value
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


//	table (variable length array) of SizedIndex
	class SizedIndexTable : public std::vector<SizedIndex> {
	public:
		SizedIndexTable(const std::vector<SizedIndex>&) noexcept;
		SizedIndexTable(const std::vector<Index>&, Size whole_size);
		SizedIndexTable(const std::vector<Size>&, Index offset = 0);

	//	invalid values exception
		class InvalidValues : public std::runtime_error {
		};

	//	iterator class
		using Iterator = iterator;
	};


}
