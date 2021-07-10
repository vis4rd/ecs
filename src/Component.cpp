#include "../include/Entity.h"

int64 Component::m_id = 0;

namespace ecs
{

static const int64 &Component::getID() noexcept
{
	return m_id;
}

const int64 &Component::getEntityID() const noexcept
{
	return m_entityID;
}

}  // namespace ecs
