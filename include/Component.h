#pragma once

#include "Root.h"

namespace ecs
{

class Component
{
public:
	const uint64 &getEntityID() const noexcept;

protected:
	uint64 m_entityID;
};

}  // namespace ecs
