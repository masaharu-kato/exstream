#pragma once
#include <stdexcept>

namespace exs {

	class const_binary_ref;
	class binary_ref;
	class const_ref_binary;
	class ref_binary;
	
//	const binary class
	class const_binary {
		friend binary_ref;
	public:
		using byte = char;

	//	get readonly data starts from index as type T
		template <class T>
		const T& get(size_t index) const;

	//	get readonly data starts from index as type T to variable
		template <class T>
		void get_to(size_t index, T& out) const;

	//	get particial binary
		const_binary_ref part_of(size_t index, size_t c_size) const;

	//	get data pointer of specific index
		virtual const byte* data_of(size_t index, size_t c_size) const;

		size_t size() const noexcept;
		const byte* data() const noexcept;

		const_binary_ref ref() const noexcept;


		void set_reference() noexcept;
		void unset_reference() noexcept;
		

	protected:
		void assert_of(size_t index, size_t size) const;

		const_binary() noexcept;
		const_binary(byte* data, size_t size) noexcept;
		virtual ~const_binary() noexcept;


	private:
		size_t _size = 0;
		byte* _data = nullptr;
		size_t refed_count = 0;


		class out_of_range : public std::out_of_range {
		public:
			using std::out_of_range::out_of_range;
		};

	};

	
//	binary class
	class binary : public const_binary {
	public:
		using data_t = byte*;
		
		using const_binary::const_binary;
		using const_binary::data;
		using const_binary::get;
		
	//	get writable data starts from index as type T
		template <class T>
		T& get(size_t index);

	//	get data pointer of specific index
		virtual byte* data_of(size_t index, size_t size) const;

	//	get particial binary
		binary_ref part_of(size_t index, size_t size) const;

		data_t data() noexcept;
		const byte* c_data() const noexcept;

		binary_ref ref() noexcept;

	};


	class const_binary_ref {
	public:
		const_binary_ref(const_binary*) noexcept;
		~const_binary_ref() noexcept;

		const_binary* operator ->() const noexcept;

	private:
		const_binary* p_binary = nullptr;
	};


	class binary_ref {
	public:
		binary_ref(binary*) noexcept;
		~binary_ref() noexcept;
		
		binary* operator ->() const noexcept;

	private:
		binary* p_binary = nullptr;
	};




	template <class T>
	const T& const_binary::get(size_t index) const
	{
		return *(T*)data_of(index, sizeof(T));
	}


	template <class T>
	void const_binary::get_to(size_t index, T& out) const
	{
		out = *(T*)data_of(index, sizeof(T));
	}


	template <class T>
	T& binary::get(size_t index)
	{
		return *(T*)data_of(index, sizeof(T));
	}


}