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
	const Entity &getEntity(const uint64 entity_id) const;
	const std::any &getComponent(const uint16 comp_dec_index, const uint64 entity_id) const;
	std::any &getComponent(const uint16 comp_dec_index, const uint64 entity_id);
	std::vector<std::any> &getComponentBucket(const uint16 comp_dec_index);
	void printComponentBuffer() const;

private:
	std::vector<Entity> m_entityBuffer;  // stores all entities
	std::vector<uint64> m_entityFlags;  // stores flags of all entities
	std::vector<uint64> m_entityComponents;  // stores bitsets of components of all entities
	ComponentBuffer<TypeListT> m_componentBuffer;  // stores all components

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
	if(m_entityComponents.capacity() < m_maxEntityCount)
	{
		m_entityComponents.reserve(m_maxEntityCount);
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

		m_entityFlags.push_back(flags);  // adding flags
		m_entityComponents.emplace_back(components);  // adding components

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
	m_componentBuffer.addComponentByIndex(comp_dec_index, entity_id);
	// we have to find the right entity's index in m_entityBuffer
	// and flip the bit of appended component
	bool found = false;
	uint64 i = uint64{0};
	for(; i < m_entityCount; i++)
	{
		if(m_entityBuffer[i].getID() == entity_id)  // if the correct id is found
		{
			found = true;
			break;
		}
	}
	// converting decimal component index to bit position
	// dec(0) == (1 << 63)
	// dec(1) == (1 << 62)
	// dec(63) == (1 << 0)
	// dec(i) == (1 << (63 - i))
	if(found)
	{
		m_entityComponents[i] |= uint64{1} << (63 - i);
	}
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
	std::swap(m_entityComponents[pos], m_entityComponents.back());
	m_entityComponents.pop_back();
}

template <typename TypeListT>
void Manager<TypeListT>::deleteAllEntities()
{
	m_componentBuffer.removeAllComponents();
	m_entityBuffer.clear();
	m_entityFlags.clear();
	m_entityComponents.clear();
	m_entityCount = uint64{0};
}

template <typename TypeListT>
const uint16 Manager<TypeListT>::bufferSize() const
{
	return m_componentBuffer.size();
}

template <typename TypeListT>
const Entity &Manager<TypeListT>::getEntity(const uint64 entity_id) const
{
	for(auto &e : m_entityBuffer)
	{
		if(e.getID() == entity_id)
		{
			return e;
		}
	}
	throw std::invalid_argument("There's no such entity with given ID.");
}

template <typename TypeListT>
const std::any &Manager<TypeListT>::getComponent(const uint16 comp_dec_index, const uint64 entity_id) const
{
	return m_componentBuffer.getComponentByIndex(comp_dec_index, entity_id);
}

template <typename TypeListT>
std::any &Manager<TypeListT>::getComponent(const uint16 comp_dec_index, const uint64 entity_id)
{
	return m_componentBuffer.getComponentByIndex(comp_dec_index, entity_id);
}

template <typename TypeListT>
std::vector<std::any> &Manager<TypeListT>::getComponentBucket(const uint16 comp_dec_index)
{
	return m_componentBuffer.getComponentBucket(comp_dec_index);
}

template <typename TypeListT>
void Manager<TypeListT>::printComponentBuffer() const
{
	std::cout << "BUFFER SIZE = " << this->bufferSize() << std::endl;
	m_componentBuffer.printAll();
}

}  // namespace ecs
