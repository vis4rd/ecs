#pragma once

#include "Root.h"
#include "Entity.h"
#include "Component.h"
#include "ComponentBuffer.h"

namespace ecs
{

template <typename TypeListT>
class Manager final
{
public:
	Manager(const uint64 max_entity_count = uint64{1000});
	void initComponentBuffer();

	// uint64 contains bitset of components.
	// Manager assumes that the user knows the order of components in the pool.
	// Most-important bit represents the first component in the pool.
	void addEntity(const uint64 components, const uint64 flags);
	void addComponent(const uint16 comp_dec_index, const uint64 entity_id);
	void deleteEntity(const uint64 entity_id);

private:
	std::vector<Entity> m_entityBuffer;  // stores all entities
	std::vector<uint64> m_entityFlags;  // stores flags of all entities
	ComponentBuffer<TypeListT> m_componentBuffer;  // stores all components

	uint8 m_flagCount;  // number of existing entity flags
	uint8 m_componentCount;  // number of components
	uint64 m_maxEntityCount;  // max number of entities
	uint64 m_entityCount;  // number of currently existing entities
	// uint64 m_disposedEntityCount;  // number of destroyed entities in m_disposedEntityPool
};

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
	m_componentBuffer = ComponentBuffer<TypeListT>{};
}

template <typename TypeListT>
void Manager<TypeListT>::addEntity(const uint64 components, const uint64 flags)
{
	if(m_entityCount < m_maxEntityCount)
	{
		m_entityCount++;
		m_entityBuffer.emplace_back(Entity());

		// parsing components
		uint16 bitpos = 0;
		for(uint64 c = uint64{1} << 63; c >= uint64{1 << 0}, bitpos < 64; c = (c >> 1), bitpos++)
		{
			if(bitpos < m_componentCount)
			{
				m_componentBuffer.template addComponentByIndex(bitpos, m_entityBuffer.back().getID());
			}
			else
			{
				std::cout << "[WARNING] Component bit position has exceeded " <<
					"known component count (position: " << bitpos << ", component count: " <<
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

template <typename TypeListT>
void Manager<TypeListT>::addComponent(const uint16 comp_dec_index, const uint64 entity_id)
{
	if(m_componentBuffer.getComponentByIndex<comp_dec_index>(entity_id))
	{
		std::cout << "[WARNING] Given component already exists under " << 
			"passed Entity ID - " <<std::endl <<"ignoring void Manager<TypeListT>::addComponent" <<
			"(const uint16 comp_dec_index, const uint64 entity_id)" << std::endl;
		return;
	}
	else
	{
		m_componentBuffer.addComponentByIndex<comp_dec_index>(entity_id);
	}
}

template <typename TypeListT>
void Manager<TypeListT>::deleteEntity(const uint64 entity_id)
{
	auto e = m_entityBuffer.begin();
	for(; e < m_entityBuffer.end(); e++)
	{
		if(e->getID() == entity_id)
		{
			break;
		}
	}
	m_componentBuffer.removeComponents(e->getID());
	auto pos = e - m_entityBuffer.begin();
	std::swap(*e, m_entityBuffer.back());
	m_entityBuffer.pop_back();
	std::swap(m_entityFlags[pos], m_entityFlags.back());
	m_entityFlags.pop_back();
}

}  // namespace ecs
