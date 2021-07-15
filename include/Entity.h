#pragma once

#include "Root.h"

namespace ecs
{

class Entity final
{
public:
	Entity();

	const uint64 &getID() const noexcept;

private:
	uint64 m_id;
};

}  // namespace ecs
