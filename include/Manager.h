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

}  // namespace ecs
