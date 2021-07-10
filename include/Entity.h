#pragma once

#include "Root.h"

namespace ecs
{

class Entity final
{
public:
	Entity();
	Entity(const Entity &copy) = delete;
	Entity(Entity &&move) = delete;
	Entity &operator=(const Entity &copy) = delete;
	Entity &operator=(Entity &&move) = delete;

	const uint64 &getID() const noexcept;

private:
	uint64 m_id;
};

}  // namespace ecs
