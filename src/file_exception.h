#include <stdexcept>

namespace exs {

	class file_exception : public std::runtime_error {
	public:
		using std::runtime_error::runtime_error;
	};

}