#pragma once

#include "Root.h"

namespace ecs
{

class Interface
{
public:
	Interface(const uint64 &id, const uint64 &index, uint64 &flag_bitset, const uint64 &component_bitset);
	const uint64 &id() const;
	const uint64 &index() const;
	uint64 &flags();
	const uint64 &components() const;

private:
	const uint64 &m_id;
	const uint64 &m_index;
	uint64 &m_flagBitset;
	const uint64 &m_compBitset;
};

}  // namespace ecs
