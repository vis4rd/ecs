#pragma once

#include "Root.h"

namespace ecs
{

/**
 * @brief Class representing interface used in ECS systems.
 */
class Interface
{
public:
	/**
	 * @brief The constructor.
	 *
	 * @param id The ID of an entity.
	 * @param index The index of an entity in the buffer.
	 * @param flag_bitset The flag bitset of an entity.
	 * @param component_bitset The component bitset of an entity.
	 *
	 * All these arguments can be used by the user in an ECS system. applySystem() method passes
	 *   values corresponding to entities it operates on.
	 */
	Interface(const uint64 &id, const uint64 &index, uint64 &flag_bitset, const uint64 &component_bitset);

	/**
	 * @brief Gets the ID of an entity.
	 * @return The ID.
	 */
	const uint64 &id() const;
	
	/**
	 * @brief Gets the index of an entity.
	 * @return The index.
	 */
	const uint64 &index() const;

	/**
	 * @brief Gets the flag bitset of an entity.
	 * @return The flag bitset.
	 */
	uint64 &flags();
	
	/**
	 * @brief Gets the component bitset of an entity.
	 * @return The component bitset.
	 */
	const uint64 &components() const;

private:
	const uint64 &m_id;  /**< The entity's ID. */
	const uint64 &m_index;  /**< The entity's index in the buffer. */
	uint64 &m_flagBitset;  /**< The entity's flag bitset. */
	const uint64 &m_compBitset;  /**< The entity's component bitset. */
};

}  // namespace ecs
