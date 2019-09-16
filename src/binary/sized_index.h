#pragma once

namespace exs {

	class SizedIndex {
	public:
		SizedIndex(size_t _index, size_t _size) noexcept;

		size_t index() const noexcept;
		size_t size() const noexcept;

	private:
		size_t _index;
		size_t _size;
	};

}
