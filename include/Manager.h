#pragma once

#include "ComponentBuffer.h"

namespace ecs
{

/**
 * Template class Manager
 * 
 * Manager is the main interface provided for the user of this entity component system.
 * It provides methods for adding/checking/deleting entities and components.
 * 
 * @tparam The component pool (types of components) used by all entities in the buffer.
 */
template <typename TypeListT>
class Manager final
{
public:
	/**
	 * @brief Constructor
	 * @param max_entity_count The maximum entity count possible to add to the buffer.
	 * 
	 * Constructor reserves enough memory for all entities fitting in the max cap.
	 */
	Manager(const uint64 max_entity_count = uint64{1000});
	
	/**
	 * @brief Adds a component to the component buffer.
	 * @param entity_id The entity identifier (automatically attached to every created entity).
	 * @tparam TypeIndex Decimal index of the type of a component in the component pool.
	 * 
	 * Manager assumes that the user knows the order of components in the pool.
	 * Most-important bit represents the first component in the pool.
	 */
	template <uint16 TypeIndex>
	void addComponent(const uint64 entity_id);

	/**
	 * @brief Gets the component reference from the pool.
	 * @param entity_id The entity identifier (automatically attached to every created entity).
	 * @tparam TypeIndex Decimal index of the type of a component in the component pool.
	 * @return If exists, the requested component is returned.
	 * 
	 * If there is no such given TypeIndex or entity_id is incorrect, this method will throw
	 *   an exception.
	 */
	template <uint16 TypeIndex>
	meta::TypeAt<TypeIndex, TypeListT> &getComponent(const uint64 entity_id);

	/**
	 * @brief Gets the component reference from the pool using component type.
	 * @param entity_id The entity identifier (automatically attached to every created entity).
	 * @tparam ComponentT Type of the requested component.
	 * @return If exists, the requested component is returned.
	 * 
	 * If there is no such given component type in the pool or entity_id is incorrect, this method
	 *   throws an exception.
	 */
	template <typename ComponentT>
	ComponentT &getComponent(const uint64 entity_id);

	/**
	 * @brief Gets the vector of wrapped (see explanation below) components.
	 * @param entity_id The entity identifier (automatically attached to every created entity).
	 * @tparam TypeIndex Decimal index of the type of a component in the component pool.
	 * @return The bucket containing wrapped components.
	 * 
	 * All components are wrapped in the ComponentWrapper. To access them, we can simply use
	 *   () operator or eComponent() method.
	 * 
	 * Example:
	 * @code{.cpp}
	 * 
	 * Manager<int, float, char> manager;
	 * 
	 * // adding some entities with all three components...
	 * 
	 * auto &vec_of_floats = manager.getComponentBucket<1>();
	 * float &data = vec_of_floats[0].eComponent();
	 * // or
	 * float &data2 = vec_of_floats[0]();
	 * 
	 * @endcode
	 * 
	 * If there is no such given TypeIndex, this method will throw an exception.
	 */
	template <uint16 TypeIndex>
	std::vector<ComponentWrapper<meta::TypeAt<TypeIndex, TypeListT>>> &getComponentBucket();

	/**
	 * @brief Gets the vector of wrapped (see explanation below) components.
	 * @tparam ComponentT Type of the requested component bucket.
	 * @return The bucket containing wrapped components.
	 * 
	 * All components are wrapped in the ComponentWrapper. To access them, we can simply use
	 *   () operator or eComponent() method.
	 * 
	 * Example:
	 * @code{.cpp}
	 * 
	 * Manager<int, float, char> manager;
	 * 
	 * // adding some entities with all three components...
	 * 
	 * auto &vec_of_floats = manager.getComponentBucket<float>();
	 * float &data = vec_of_floats[0].eComponent();
	 * // or
	 * float &data2 = vec_of_floats[0]();
	 * 
	 * @endcode
	 * 
	 * If there is no such given TypeIndex, this method will throw an exception.
	 */
	template <typename ComponentT>
	std::vector<ComponentWrapper<ComponentT>> &getComponentBucket();

	/**
	 * @brief Checks if the given components exists in the buffer.
	 * @param entity_id The entity identifier (automatically attached to every created entity).
	 * @tparam TypeIndex @tparam TypeIndex Decimal index of the type of a component in the component pool.
	 * @return Boolean value indicating whether the component exists.
	 * 
	 * @note Also this method is safe to use as it will not throw any exception when passed
	 *         arguments don't exist.
	 */
	template <uint16 TypeIndex>
	const bool checkComponent(const uint64 entity_id) const noexcept;

	/**
	 * @brief Gets the vector of entity ids.
	 * @return The entity buffer.
	 */
	const std::vector<uint64> &getEntityBuffer() const;

	/**
	 * @brief Adds a new entity to the buffer.
	 * @param components The bitset of components, where every component has it's own bitwise position.
	 * @param flags The bitset of flags attached to entity, where every flag has it's own bitwise position.
	 * @tparam ComponentCount Number of components bound to this entity passed to the buffer.
	 * 
	 * It is recommended to specify ComponentCount when calling this method as components are added
	 *   recursively, but on the other hand it is not required to do so (at cost of performance).
	 */
	template <uint16 ComponentCount = uint16{64}>
	const uint64 &addEntity(const uint64 components, const uint64 flags);

	/**
	 * @brief Removes entities from the buffer.
	 * @param entity_id The entity identifier (automatically attached to every created entity).
	 * 
	 * If the given id is incorrect (entity doesn't exist) this method does nothing.
	 */
	void deleteEntity(const uint64 entity_id);

	/**
	 * @brief Checks if the given entity exists in the buffer.
	 * @param entity_id The entity identifier (automatically attached to every created entity).
	 * @return Boolean value indicating whether the entity exists.
	 * 
	 * @note Also this method is safe to use as it will not throw any exception when passed
	 *         arguments don't exist.
	 */
	const bool checkEntity(const uint64 entity_id) const noexcept;

	/**
	 * @brief Removes all entities from the buffer.
	 * 
	 * @warning This method also removes all flags and all components, because they are strictly bound to entities.
	 */
	void deleteAllEntities();

	/**
	 * @brief Gets the current number of existing entities in the buffer.
	 * @return The current entity count.
	 * 
	 * @note Also this method is safe to use as it will not throw any exception when passed
	 *         arguments don't exist.
	 */
	const uint64 &getCurrentEntityCount() const noexcept;

	/**
	 * @brief Gets the maximal possible number of entities that can fit into the buffer.
	 * @return The maximum entity count.
	 * 
	 * @note Also this method is safe to use as it will not throw any exception when passed
	 *         arguments don't exist.
	 */
	const uint64 &getMaxEntityCount() const noexcept;

	/**
	 * @brief Gets the flag status (1 or 0) of a given flag type and entity.
	 * @param flagBit The flag's bitwise position.
	 * @param entity_id The entity identifier (automatically attached to every created entity).
	 * @return The flag.
	 */
	const bool getFlag(const uint64 flagBit, const uint64 entity_id) const;

	/**
	 * @brief Sets the flag status of the given bitwise position, entity id to the passed boolean value.
	 * @param flagBit The flag's bitwise position.
	 * @param entity_id The entity identifier (automatically attached to every created entity).
	 * @param value The value which the flag's status will be set to.
	 * 
	 * If given entity_id does not exist, this method does nothing.
	 * 
	 * @note Also this method is safe to use as it will not throw any exception when passed
	 *         arguments don't exist.
	 */
	void setFlag(const uint64 flagBit, const uint64 entity_id, const bool value) noexcept;

	/**
	 * @brief Sets the flag status of the given bitwise position to the passed boolean value.
	 * @param flagBit The flag's bitwise position.
	 * @param value The value which the flag's status will be set to.
	 */
	void setFlagsForAll(const uint64 flagBit, const bool value);  // sets a flag (or many flags) for all entities

	/**
	 * @brief Gets the vector of entities' flags
	 * @return The flag buffer.
	 */
	std::vector<uint64> &getFlagBuffer();

	/**
	 * @brief Applies passed function/functor/lambda (ECS system) to all entities matching required conditions.
	 * @param system The system working on/changing components' data.
	 * @tparam ComponentListT The list of components required for the system to work properly.
	 * 
	 * @code{.cpp}
	 * void system(int &arg1) { arg1 = 5; }
	 * std::function<void(int&)> fun = system;
	 * 
	 * // Let's assume that ComponentBuffer has an int as one of the components. Then:
	 * 
	 * using CPool = ecs::meta::ComponentPool<int>;  // creating pool of components for the manager
	 * ecs::Manager<CPool> manager;  // holds only one component type - int
	 * manager.addEntity<1>(ecs::uint64{1} << 63, ecs::uint64{0});
	 * // The second argument are flags, we pass 0 here to simplify the example.
	 * // Notice that the first (and only) component has the left-most bit position.
	 * 
	 * manager.applySystem<int>(fun);
	 *  
	 * // The system(int&) function has been applied to every entity with int component.
	 * @endcode
	 * 
	 * Template parameters are components that filter entities for which the system will be applied.
	 * The fewer components are required, the faster the application will work. On the other hand,
	 *   the more components are passed, the more strict and precise is the filter. Given
	 *   components and system arguments have to match 100% (including the same order of passing)!
	 * System's arguments have to be references, otherwise the value will be copied and the whole
	 *   operation would not make any sense.
	 * 
	 * If the entity number is reaching over 5000, this method will use parallel threads.
	 */
	template <typename... ComponentListT>
	void applySystem(std::function<void(ComponentListT& ...)> &system);

	/**
	 * @brief Applies passed function/functor/lambda (ECS system) to all entities matching required conditions.
	 * @param system The system working on/changing components' data.
	 * @tparam ComponentListT The list of components required for the system to work properly.
	 * 
	 * This method is provided for convenience, allowing to pass raw function pointers instead of
	 *   std::function objects.
	 * 
	 * @see void applySystem(std::function<void(ComponentListT& ...)> system)
	 */
	template <typename... ComponentListT>
	void applySystem(void (*system)(ComponentListT& ...));

private:
	/**
	 * @brief Convenience helper method used in addEntity().
	 */
	template <uint16 Index> void addEntityComponents(const uint64 components, const uint64 &entity_id);

	/**
	 * @brief Convenience helper method getting components for use in applySystem()
	 */
	template <typename... ComponentListT> auto getMatchingComponentPack(const uint64 &entity_id);

private:
	std::vector<uint64> m_entityBuffer;            /**< Stores all entities. */
	std::vector<uint64> m_entityFlags;             /**< Stores flags of all entities. */
	std::vector<uint64> m_entityComponents;        /**< Stores component bitsets of all entities. */
	ComponentBuffer<TypeListT> m_componentBuffer;  /**< Stores all components. */

	static uint64 m_nextEntityID;  /**< Used and incremented in every case when entity is added to the buffer */
	uint16 m_flagCount;            /**< Number of existing entity flags. */
	uint64 m_maxEntityCount;       /**< The max number of entities. */
	uint64 m_entityCount;          /**< Number of currently existing entities. */
	static constexpr const uint16 m_componentCount = meta::TypeListSize<TypeListT>;  /**< Number of component types. */
};

}  // namespace ecs

#include "../src/Manager.inl"
