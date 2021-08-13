#pragma once

#include "Meta.h"
#include "ComponentWrapper.h"

namespace ecs
{

namespace meta
{
	template <typename... Typepack>
	using ComponentPool = TypeList<Typepack ...>;
}  // namespace meta

// ################################################################################################
// ComponentBuffer

/**
 * @brief Class predeclaration.
 * @tparam TypeListT List of component types used in the buffer.
 */
template <typename TypeListT>
class ComponentBuffer;

/**
 * @brief The ComponentBuffer handling entities' conmponents.
 * @tparam Typepack Pack of component types used in the buffer.
 * 
 * Every component is wrapped in the buffer so that in some places interface is unified.
 * ComponentWrapper also helps in recognizing which components belong to which entities.
 */
template <typename... Typepack>
class ComponentBuffer<meta::TypeList<Typepack...>>
{
	using m_cPool = meta::ComponentPool<ComponentWrapper<Typepack> ...>;  // WRAPPED
	using m_tPool = meta::TypeList<Typepack...>;  // NOT WRAPPED
public:
	ComponentBuffer(const uint64 max_entity_count = uint64{1000});
	
	/**
	 * @brief Gets the vector of components of given type.
	 * @tparam ComponentT The type of the requested component.
	 * @return The component bucket if the given type exists, otherwise an exception is thrown.
	 * 
	 * @note This method returns vector of wrapped components.
	 */
	template <typename ComponentT>
	std::vector<ComponentWrapper<ComponentT>> &getComponentBucket();
	
	/**
	 * @brief Tries to get the specific component from the buffer.
	 * @param entity_id The entity identifier (automatically attached to every created entity).
	 * @tparam ComponentT The type of the requested component.
	 * @return std::optional object. If given componentn type does not exist, this method returns
	 *         std::nullopt. Otherwise gets an existing component or creates and returns a new one.
	 */
	template <typename ComponentT>
	std::optional<ComponentT> &tryGetComponent(const uint64 entity_id) noexcept;

	/**
	 * @brief Gets the requested component of given type and entity id.
	 * @param entity_id The entity identifier (automatically attached to every created entity).
	 * @tparam ComponentT The type of the requested component.
	 * @return If the type and entity id exists, the requested component is returned.
	 * 
	 * @warning This method is unsafe, beacuse if any of the passed arguments/params are invalid,
	 *          it throws an exception.
	 */
	template <typename ComponentT>
	ComponentT &getComponent(const uint64 entity_id);
	
	/**
	 * @brief Returns std::tuple with objects of component types matching given entity_id.
	 * @param entity_id The entity identifier (automatically attached to every created entity).
	 * @tparam ComponentListT The list of types of requested components.
	 * @return The list of components matching passed ComponentListT.
	 * 
	 * This method neither does check whether user passed any component types as template
	 *   parameters nor checks if given component types exist in the pool.
	 * 
	 * @warning This metod is unsafe, because if any of the passed arguments/params are invalid,
	 *          it throws an exception.
	 */
	template <typename... ComponentListT>
	auto getComponentsMatching(const uint64 entity_id);

	/**
	 * @brief Adds a new component of given type and entity id to the buffer.
	 * @param entity_id The entity identifier (automatically attached to every created entity).
	 * @tparam ComponentT The type of the added component.
	 * @return If both component type and entity id are valid, the created instance of component is
	 *         returned.
	 * 
	 * @warning This method does not check whether such component already exists. If there are two
	 *          instances of the same component with equal entity id, the behaviour of whole buffer
	 *          is undefined. For additional safety in this matter, see tryGetComponent() method.
	 * 
	 * @see std::optional<ComponentT> &tryGetComponent(const uint64 entity_id) noexcept
	 * 
	 * The returned component reference is unwrapped from ComponentWrapper.
	 */
	template <typename ComponentT>
	auto &addComponent(const uint64 entity_id);

	/**
	 * @brief Adds a new component using decimal index of its type in the pool.
	 * @param entity_id The entity identifier (automatically attached to every created entity).
	 * @tparam decimalIndex Index of the component type in the pool.
	 * @return If both component index and entity id are valid, the created instance of component
	 *         is returned.
	 *
	 * @warning This method does not check whether such component already exists. If there are two
	 *          instances of the same component with equal entity id, the behaviour of whole buffer
	 *          is undefined. For additional safety in this matter, see tryGetComponent() method.
	 * 
	 * @see std::optional<ComponentT> &tryGetComponent(const uint64 entity_id) noexcept
	 * 
	 * The returned component reference is unwrapped from ComponentWrapper.
	 */
	template <uint16 decimalIndex>
	auto &addComponentByIndex(const uint64 entity_id);

	/**
	 * @brief Gets the existing component using decimal index of its type in the pool.
	 * @param entity_id The entity identifier (automatically attached to every created entity).
	 * @tparam decimalIndex Index of the component type in the pool.
	 * @return If both component index and entity id are valid, the requested instance of component
	 *         is returned wrapped in std::optional object.
	 *
	 * @note If there's no component with given entity id or type index is invalid, the returned
	 *       value is std::nullopt instead.
	 */
	template <uint16 decimalIndex>
	const std::optional<meta::TypeAt<decimalIndex, m_tPool>> getComponentByIndex(const uint64 entity_id) const noexcept;

	/**
	 * @brief Gets the existing component using decimal index of its type in the pool.
	 * @param entity_id The entity identifier (automatically attached to every created entity).
	 * @tparam decimalIndex Index of the component type in the pool.
	 * @return If both component index and entity id are valid, the requested instance of component
	 *         is returned wrapped in std::optional object.
	 *
	 * @note If there's no component with given entity id or type index is invalid, the returned
	 *       value is std::nullopt instead.
	 * 
	 * This is a non-const version of this method provided for convenience.
	 */
	template <uint16 decimalIndex>
	std::optional<meta::TypeAt<decimalIndex, m_tPool>> getComponentByIndex(const uint64 entity_id) noexcept;

	/**
	 * @brief Removes all existing components in the buffer.
	 * @tparam Index This parameter should not be explicitly passed by the user, as it is used only
	 *         as a help for template recursion.
	 * 
	 * Explicitly passing a value to the Index will cause program termination or incorrect clear of
	 *   the buffer.
	 */
	template <uint16 Index = (sizeof... (Typepack) - 1)>
	void clear() noexcept;

	/**
	 * @brief Checks whether component of given index of type in the pool and entity id exists.
	 * @param entity_id The entity identifier (automatically attached to every created entity).
	 * @tparam decimalIndex Index of the component type in the pool.
	 * @return The boolean value specifying whether such component exists or not.
	 * 
	 * @warning Although this method is marked noexcept, it still can terminate the program when
	 *          given decimalIndex is invalid (out of range).
	 */
	template <uint16 decimalIndex>
	const bool checkComponent(const uint64 entity_id) const noexcept;

	/**
	 * @brief Removes all components from the buffer belonging to the given entity id.
	 * @param entity_id The entity identifier (automatically attached to every created entity).
	 * 
	 * @note This method is safe to use as it will not throw any exception and given arguments can
	 *       be invalid.
	 */
	void removeComponents(const uint64 entity_id) noexcept;

	// Remove single component with given type and Entity ID. Both arguments must be valid.

	/**
	 * @brief Removes a component of given type and entity id from the buffer.
	 * @param entity_id The entity identifier (automatically attached to every created entity).
	 * @tparam ComponentT Type of the component which is supposed to be removed.
	 * 
	 * @warning This method will throw an exception if given component type or entity id are
	 *          invalid.
	 */
	template <typename ComponentT>
	void removeComponent(const uint64 entity_id);

	/**
	 * @brief Returns the current number of components in the buffer.
	 * @return Decimal number of components in the buffer.
	 */
	const uint64 size() const;

	/**
	 * @brief Returns the current size of the specific bucket containing components of given type.
	 * @tparam ComponentT Type of the component, which container's size is requested.
	 * @return Decimal number of components in the bucket of passed type.
	 * 
	 * The size value is acquired by calling getComponentBucket() method, so all safety/exception
	 *   rules apply from it.
	 * 
	 * @see std::vector<ComponentWrapper<ComponentT>> &getComponentBucket()
	 */
	template <typename ComponentT>
	const uint64 bucketSize() const;

	/**
	 * @brief Convenience method used for debugging.
	 * 
	 * It prints all types and component values currently existing in the buffer.
	 */
	void printAll() const;

private:
	meta::metautil::TupleOfVectorsOfTypes<m_cPool> m_cBuffer;  /**< Container holding all components in the buffer. */
	uint64 m_maxEntityCount;                                   /**< Maximal possible number of entities which can fit into the buffer. */
};

}  // namespace ecs

#include "../src/ComponentBuffer.inl"
