#pragma once
#include "index_table.h"
#include "../stream.h"
#include <unordered_map>

namespace exs {

	using Name = std::string;
	using NameIndexMap = std::unordered_map<Name, Index>;

	class NamedIndexTable : public IndexTable {
	public:
		NamedIndexTable(const IndexTable&, const NameIndexMap&);
		NamedIndexTable(const std::unordered_map<Name, SizedIndex>&);

		SizedIndex operator [](Name) const noexcept;

	private:
		NameIndexMap name_index_map;
	};

}
