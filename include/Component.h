#pragma once

#include "Root.h"

namespace ecs
{

class Component
{
public:
	static const int64 &getID() noexcept;
	const int64 &getEntityID() const noexcept;

protected:
	static int64 m_id;
	int64 m_entityID;
};

}  // namespace ecs
