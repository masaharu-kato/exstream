#pragma once
#include <stdexcept>
#include "sized_index.h"

namespace exs {

	class ConstBinaryRef;
	class BinaryRef;
	class ConstRefBinary;
	class RefBinary;
	
//	const binary class
	class ConstBinary {
		friend BinaryRef;
	public:
		using Byte = char;

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

		size_t size() const noexcept;
		const Byte* data() const noexcept;

		ConstBinaryRef ref() const noexcept;


		void set_reference() noexcept;
		void unset_reference() noexcept;
		

	protected:
		void assert_of(size_t index, size_t size) const;

		ConstBinary() noexcept;
		ConstBinary(Byte* data, size_t size) noexcept;
		virtual ~ConstBinary() noexcept;


	private:
		size_t _size = 0;
		Byte* _data = nullptr;
		size_t refed_count = 0;


		class out_of_range : public std::out_of_range {
		public:
			using std::out_of_range::out_of_range;
		};

	};

	
//	binary class
	class Binary : public ConstBinary {
	public:
		using ConstBinary::const_binary;
		using ConstBinary::data;
		using ConstBinary::get;
		
	//	get writable data starts from index as type T
		template <class T>
		T& get(size_t index);

	//	get data pointer of specific index
		virtual Byte* data_of(SizedIndex) const;

	//	get particial binary
		BinaryRef part_of(SizedIndex) const;

		Byte* data() noexcept;
		const Byte* c_data() const noexcept;

		BinaryRef ref() noexcept;

	};


	class ConstBinaryRef {
	public:
		ConstBinaryRef(ConstBinary*) noexcept;
		~ConstBinaryRef() noexcept;

		ConstBinary* operator ->() const noexcept;

	private:
		ConstBinary* binary = nullptr;
	};


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