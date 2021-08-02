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

	// uint64 contains bitset of components.
	// Manager assumes that the user knows the order of components in the pool.
	// Most-important bit represents the first component in the pool.
	template <uint16 ComponentCount = uint16{64}> void addEntity(const uint64 components, const uint64 flags);
	template <uint16 TypeIndex> void addComponent(const uint64 entity_id);
	template <uint16 TypeIndex> meta::TypeAt<TypeIndex, TypeListT> &getComponent(const uint64 entity_id);
	template <typename ComponentT> ComponentT &getComponent(const uint64 entity_id);
	template <uint16 TypeIndex> std::vector<ComponentWrapper<meta::TypeAt<TypeIndex, TypeListT>>> &getComponentBucket();
	template <typename ComponentT> std::vector<ComponentWrapper<ComponentT>> &getComponentBucket();
	template <uint16 TypeIndex> const bool checkComponent(const uint64 entity_id) const noexcept;

	void deleteEntity(const uint64 entity_id);
	const bool checkEntity(const uint64 entity_id) const noexcept;

private:
	template <uint16 Index>
	void addEntityComponents(const uint64 components, const uint64 &entity_id);

private:
	std::vector<Entity> m_entityBuffer;  // stores all entities
	std::vector<uint64> m_entityFlags;  // stores flags of all entities
	std::vector<uint64> m_entityComponents;  // stores component bitsets of all entities
	ComponentBuffer<TypeListT> m_componentBuffer;  // stores all components

	uint16 m_flagCount;  // number of existing entity flags
	static constexpr const uint16 m_componentCount = meta::TypeListSize<TypeListT>;  // number of components
	uint64 m_maxEntityCount;  // max number of entities
	uint64 m_entityCount;  // number of currently existing entities
	// uint64 m_disposedEntityCount;  // number of destroyed entities in m_disposedEntityPool
};

template <typename TypeListT>
Manager<TypeListT>::Manager(const uint64 max_entity_count)
:
m_flagCount(uint16{0}),
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
template <uint16 ComponentCount>
void Manager<TypeListT>::addEntity(const uint64 components, const uint64 flags)
{
	if(m_entityCount < m_maxEntityCount)
	{
		m_entityCount++;
		m_entityBuffer.emplace_back(Entity());

		// parsing components
		const uint64 &id = m_entityBuffer.back().getID();
		this->addEntityComponents<ComponentCount-1>(components, id);

		// adding flags
		m_entityFlags.push_back(flags);

		// adding components
		m_entityComponents.push_back(components);

		// TBD : use disposed entities instead of assigning new ones everytime
	}
	else
	{
		std::cout << "[WARNING] Number of allocated entities has reached the cap," <<
			" ignoring the new ones..." << std::endl;
	}
}

template <typename TypeListT>
template <uint16 TypeIndex>
void Manager<TypeListT>::addComponent(const uint64 entity_id)
{
	if(m_componentBuffer.getComponentByIndex<TypeIndex>(entity_id))
	{
		std::cout << "[WARNING] Given component already exists under " << 
			"passed Entity ID - " <<std::endl <<"ignoring void Manager<TypeListT>::addComponent" <<
			"(const uint64 entity_id)" << std::endl;
		return;
	}
	else
	{
		// adding component to the buffer
		m_componentBuffer.addComponentByIndex<TypeIndex>(entity_id);

		// flipping the bit to 1
		uint16 i = uint16{0};
		for(; i < m_entityCount; i++)
		{
			if(m_entityBuffer[i].getID() == entity_id)
			{
				break;
			}
		}
		m_entityComponents[i] |= (uint64{1} << TypeIndex);
	}
}

template <typename TypeListT>
template <uint16 TypeIndex>
meta::TypeAt<TypeIndex, TypeListT> &Manager<TypeListT>::getComponent(const uint64 entity_id)
{
	return this->getComponent<meta::TypeAt<TypeIndex, TypeListT>>(entity_id);
}

template <typename TypeListT>
template <typename ComponentT>
ComponentT &Manager<TypeListT>::getComponent(const uint64 entity_id)
{
	return m_componentBuffer.template getComponent<ComponentT>(entity_id);
}

template <typename TypeListT>
template <uint16 TypeIndex>
std::vector<ComponentWrapper<meta::TypeAt<TypeIndex, TypeListT>>> &Manager<TypeListT>::getComponentBucket()
{
	return m_componentBuffer.template getComponentBucket<meta::TypeAt<TypeIndex, TypeListT>>();
}

template <typename TypeListT>
template <typename ComponentT>
std::vector<ComponentWrapper<ComponentT>> &Manager<TypeListT>::getComponentBucket()
{
	return m_componentBuffer.template getComponentBucket<ComponentT>();
}

template <typename TypeListT>
template <uint16 TypeIndex>
const bool Manager<TypeListT>::checkComponent(const uint64 entity_id) const noexcept
{
	if(meta::DoesTypeExist<meta::TypeAt<TypeIndex, TypeListT>, TypeListT>)
	{
		// returned value is of type std::optional<type>
		const auto result = m_componentBuffer.template getComponentByIndex<TypeIndex>(entity_id);
		if(result.has_value())  // result contains some value
		{
			return true;
		}
		else  // otherwise (ex. std::nullopt or default-constructed value)
		{
			return false;
		}
	}
	else
	{
		return false;
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
	std::swap(m_entityComponents[pos], m_entityComponents.back());
	m_entityComponents.pop_back();
}

template <typename TypeListT>
const bool Manager<TypeListT>::checkEntity(const uint64 entity_id) const noexcept
{
	for(auto &e : m_entityBuffer)
	{
		if(e.getID() == entity_id)
		{
			return true;
		}
	}
	return false;
}

// PRIVATE
template <typename TypeListT>
template <uint16 Index>
void Manager<TypeListT>::addEntityComponents(const uint64 components, const uint64 &entity_id)
{
	if constexpr(Index >= m_componentCount)
	{
		std::cout << "[WARNING] Component bit position has exceeded " <<
			"known component count (position: " << Index << ", component count: " <<
			m_componentCount << ") - breaking operation..." << std::endl;
	}
	else
	{
		if(((uint64{1} << Index) & components) == (uint64{1} << Index))  // if bit at position Index is equal to 1
		{
			m_componentBuffer.template addComponentByIndex<Index>(entity_id);
		}
	}
	if constexpr(Index > uint16{0})
	{
		this->addEntityComponents<Index - 1>(components, entity_id);
	}
}

}  // namespace ecs
