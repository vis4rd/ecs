#include "../include/Interface.h"

namespace ecs
{

Interface::Interface(const uint64 &id, const uint64 &index, uint64 &flag_bitset, const uint64 &component_bitset)
:
m_id(id),
m_index(index),
m_flagBitset(flag_bitset),
m_compBitset(component_bitset)
{ }

const uint64 &Interface::id() const
{
	return m_id;
}

const uint64 &Interface::index() const
{
	return m_index;
}

uint64 &Interface::flags()
{
	return m_flagBitset;
}

const uint64 &Interface::components() const
{
	return m_compBitset;
}

}  // namespace ecs
