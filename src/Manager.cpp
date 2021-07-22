#include "../include/Manager.h"

namespace ecs
{

template <typename TypeListT>
Manager<TypeListT>::Manager(const uint64 max_entity_count)
:
m_flagCount(uint8{0}),
m_componentCount(meta::TypeListSize<TypeListT>),
m_maxEntityCount(max_entity_count),
m_entityCount(uint64{0})
// m_disposedEntityCount(uint64{0})
{
	if(m_entityBuffer.capacity() < m_maxEntityCount)
	{
		m_entityBuffer.reserve(m_maxEntityCount);
	}
	if(m_entityFlags.capacity() < m_maxEntityCount)
	{
		m_entityFlags.reserve(m_maxEntityCount);
	}
}

template <typename TypeListT>
void Manager<TypeListT>::initComponentBuffer()
{
	m_componentBuffer();
}

template <typename TypeListT>
void Manager<TypeListT>::addEntity(const uint64 components, const uint64 flags)
{
	if(m_entityCount < m_maxEntityCount)
	{
		m_entityCount++;
		m_entityBuffer.emplace_back(Entity());

		// parsing components
		uint16 bit_pos = 0;
		for(uint64 c = uint64{1} << 63; c >= uint64{1 << 0}, bit_pos < 64; c == (c >> 1), bit_pos++)
		{
			if(bit_pos < m_componentCount)
			{
				m_componentBuffer.addComponentByIndex<bit_pos>(m_entityBuffer.back().getID());
			}
			else
			{
				std::cout << "[WARNING] Component bit position has exceeded " <<
					"known component count (position: " << bit_pos << ", component count: " <<
					m_componentCount << ") - breaking operation..." << std::endl;
				break;
			}
		}

		// adding flags
		m_entityFlags.push_back(flags);

		// TBD : use disposed entities instead of assigning new ones everytime
	}
	else
	{
		std::cout << "[WARNING] Number of allocated entities has reached the cap," <<
			" ignoring the new ones..." << std::endl;
	}
}

}  // namespace ecs
