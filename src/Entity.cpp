#include "../include/Entity.h"

namespace ecs
{

Entity::Entity()
{
	static uint64 id;
	m_id = id;
	id++;
}

const uint64 &Entity::getID() const noexcept
{
	return m_id;
}

}  // namespace ecs
