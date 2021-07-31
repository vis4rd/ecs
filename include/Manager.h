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
	void deleteAllEntities();
	const uint16 bufferSize() const;

	ComponentBuffer<TypeListT> m_componentBuffer;  // stores all components
private:
	std::vector<Entity> m_entityBuffer;  // stores all entities
	std::vector<uint64> m_entityFlags;  // stores flags of all entities

	uint8 m_flagCount;  // number of existing entity flags (types)
	uint8 m_componentCount;  // number of components in the TypeList
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
		auto &id = m_entityBuffer.back().getID();

		// parsing components
		uint16 index = uint16{0};
		for(uint64 iter = (uint64{1} << 63); iter >= uint64{1}; iter >>= 1, index++)
		{
			if((iter & components) == iter)  // this bit is 1, so component should be added
			{
				m_componentBuffer.addComponentByIndex(index, id);
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
	m_componentBuffer.template addComponentByIndex(comp_dec_index, entity_id);
}

template <typename TypeListT>
void Manager<TypeListT>::deleteEntity(const uint64 entity_id)
{
	auto e = m_entityBuffer.begin();
	for(; e < m_entityBuffer.end(); e++)  // searching for position of entity in the buffer
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

template <typename TypeListT>
void Manager<TypeListT>::deleteAllEntities()
{
	m_componentBuffer.removeAllComponents();
	m_entityBuffer.clear();
	m_entityFlags.clear();
	m_entityCount = uint64{0};
}

template <typename TypeListT>
const uint16 Manager<TypeListT>::bufferSize() const
{
	return m_componentBuffer.size();
}

}  // namespace ecs
