#include "../include/Component.h"

namespace ecs
{

const uint64 &Component::getEntityID() const noexcept
{
	return m_entityID;
}

}  // namespace ecs
