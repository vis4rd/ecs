#pragma once

#include "Root.h"

namespace ecs
{

class Entity final
{
public:
	explicit Entity();
	Entity(const Entity &copy) = delete;
	Entity(Entity &&move) = delete;
	Entity &operator=(const Entity &copy) = delete;
	Entity &operator=(Entity &&move) = delete;
	
private:
	uint64 m_id;
};

}  // namespace ecs
