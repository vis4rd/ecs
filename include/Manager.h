#pragma once

#include "Root.h"
#include "Entity.h"
#include "Component.h"

namespace ecs
{

class Manager final
{
public:
	Manager(const uint64 max_entity_count = uint64{1000});

private:
	std::vector<Entity> m_entityBuffer;  // stores all entities
	std::vector<uint64> m_entityFlags;  // stores flags of all entities

	uint8 m_flagCount;  // number of existing entity flags
	uint8 m_componentCount;  // number of components
	uint64 m_maxEntityCount;  // max number of entities
	uint64 m_entityCount;  // number of currently existing entities
	uint64 m_disposedEntityCount;  // number of destroyed entities in m_disposedEntityPool
};

}  // namespace ecs
