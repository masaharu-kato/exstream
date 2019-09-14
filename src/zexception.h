#include <stdexcept>

namespace exs {

	class zexception : public std::runtime_error {
	public:
		using std::runtime_error::runtime_error;
	};

}