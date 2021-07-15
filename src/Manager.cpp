#include "../include/Manager.h"

namespace ecs
{

Manager::Manager(const uint64 max_entity_count)
:
m_flagCount(uint8{0}),
m_componentCount(uint8{0}),
m_maxEntityCount(max_entity_count),
m_entityCount(uint64{0}),
m_disposedEntityCount(uint64{0})
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

}  // namespace ecs
