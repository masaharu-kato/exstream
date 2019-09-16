#pragma once
#include "ref_binary.h"

namespace exs {

	class _padded_utilty {
	protected:
		_padded_utilty(size_t elem_size, size_t unit_size) noexcept;

		void assert_of(size_t index, size_t size) const;
		size_t index_of(size_t index) const noexcept;

	private:
		size_t elem_size;
		size_t unit_size;
	};


	class const_padded_ref_binary : public const_ref_binary, public _padded_utilty {
	public:

	//	get data pointer of specific index
		virtual const byte* data_of(size_t index, size_t size) const override;

	protected:
		const_padded_ref_binary(const const_binary_ref&, size_t index, size_t size, size_t elem_size, size_t unit_size) noexcept; 

	};


	class padded_ref_binary : public ref_binary, public _padded_utilty {
	public:

	//	get data pointer of specific index
		virtual byte* data_of(size_t index, size_t size) const override;

	protected:
		padded_ref_binary(const binary_ref&, size_t index, size_t size, size_t elem_size, size_t unit_size) noexcept; 

	};

}
