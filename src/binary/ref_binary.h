#pragma once
#include "binary.h"

namespace exs {

	class RefBinary : public Binary {
	public:
		template <class... Args>
		RefBinary* create(Args&&... args) {
			return new RefBinary(args...);
		}

	protected:
		RefBinary(BinaryRef, SizedIndex) noexcept; 

	private:
		BinaryRef source;
	};


	class ConstRefBinary : public ConstBinary {
	public:
		template <class... Args>
		ConstRefBinary* create(Args&&... args) {
			return new ConstRefBinary(args...);
		}

	protected:
		ConstRefBinary(ConstBinaryRef, SizedIndex) noexcept; 

	private:
		ConstBinaryRef source;
	};

}
