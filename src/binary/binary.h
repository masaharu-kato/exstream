#pragma once
#include <stdexcept>
#include "sized_index.h"

namespace exs {

	class ConstBinaryRef;
	class BinaryRef;
	class ConstRefBinary;
	class RefBinary;
	
	using Byte = char;
	
//	const binary class
	class ConstBinary {
		friend BinaryRef;
	public:

	//	get readonly data starts from index as type T
		template <class T>
		const T& get(size_t index) const;

	//	get readonly data starts from index as type T to variable
		template <class T>
		void get_to(size_t index, T& out) const;

	//	get particial binary
		ConstBinaryRef part_of(SizedIndex) const;

	//	get data pointer of specific index
		virtual const Byte* data_of(SizedIndex) const;

	//	get size (length) of data
		Size size() const noexcept;

	//	get first pointer of data
		const Byte* data() const noexcept;

	//	get own reference
		ConstBinaryRef ref() const noexcept;

	//	set reference
	//	(increment reference counter)
		void set_reference() noexcept;

	//	unset reference
	//	(decrement reference counter, and delete this if counter is zero)
		void unset_reference() noexcept;
		

	protected:
	//	check if given sized-index is valid
		void assert_of(SizedIndex) const;

	//	ConstBinary() noexcept;
		ConstBinary(Byte* data, Size size) noexcept;
		virtual ~ConstBinary() noexcept;


	private:
	//	size (length) of data
		Size _size = 0;

	//	first pointer of data
		Byte* _data = nullptr;

	//	reference counter
		size_t refed_count = 0;


	//	out-of-range exception class
		class OutOfRange : public std::out_of_range {
		public:
			using std::out_of_range::out_of_range;
		};

	};

	
//	binary class
	class Binary : public ConstBinary {
	public:
		using ConstBinary::ConstBinary;
		using ConstBinary::data;
		using ConstBinary::get;
		using ConstBinary::ref;
		
	//	get writable data starts from index as type T
		template <class T>
		T& get(Size index);

	//	get data pointer of specific index
		virtual Byte* data_of(SizedIndex) const;

	//	get particial binary
		BinaryRef part_of(SizedIndex);

	//	get first pointer of data
		Byte* data() noexcept;

	//	get readonly first pointer of data
		const Byte* c_data() const noexcept;

	//	get own reference
		BinaryRef ref() noexcept;

	//	get readonly own reference
		ConstBinaryRef c_ref() noexcept;

	};


//	reference of ConstBinary
	class ConstBinaryRef {
	public:
		ConstBinaryRef(ConstBinary*) noexcept;
		~ConstBinaryRef() noexcept;

		ConstBinary* operator ->() const noexcept;

	private:
		ConstBinary* binary = nullptr;
	};


//	reference of Binary
	class BinaryRef {
	public:
		BinaryRef(Binary*) noexcept;
		~BinaryRef() noexcept;
		
		Binary* operator ->() const noexcept;

	private:
		Binary* binary = nullptr;
	};




	template <class T>
	const T& ConstBinary::get(size_t index) const
	{
		return *(T*)data_of(index, sizeof(T));
	}


	template <class T>
	void ConstBinary::get_to(size_t index, T& out) const
	{
		out = *(T*)data_of(index, sizeof(T));
	}


	template <class T>
	T& Binary::get(size_t index)
	{
		return *(T*)data_of(index, sizeof(T));
	}


}