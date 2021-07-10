#include "../include/Entity.h"

namespace ecs
{

Entity::Entity()
{
	static uint64 id;
	id++;
	m_id = id;
}

}  // namespace ecs
