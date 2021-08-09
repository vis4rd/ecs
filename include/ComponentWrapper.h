#pragma once

#include "Util.h"

namespace ecs
{

/**
 * @brief Class wrapping components for ComponentBuffer
 * @tparam ComponentT Type of component instance which is to be wrapped
 * 
 * Main reason the wrapper exists is that components in the buffer must be tied somehow with their
 *   respective entity id.
 */
template <typename ComponentT>
class ComponentWrapper
{
public:
	/**
	 * @brief Default constructor
	 */
	ComponentWrapper() = default;

	/**
	 * @brief Special constructor which allows creation of ComponentWrapper instance given type
	 *        of the component and its entity id.
	 * @param entity_id The entity identifier (automatically attached to every created entity).
	 */
	explicit ComponentWrapper(const uint64 &entity_id);

	/**
	 * @brief Special constructor which allows wrapping component without specifying its entity id.
	 * @param comp The component instance which will be wrapped.
	 */
	explicit ComponentWrapper(const ComponentT &comp);

	/**
	 * @brief Parenthesis operator overload which gets the unwrapped component instance.
	 * @return The unwrapped component instance
	 */
	const ComponentT &operator()() const;

	/**
	 * @brief Parenthesis operator overload which gets the unwrapped component instance.
	 * @return The unwrapped component instance
	 *
	 * This is a non-const version of this method provided for convenience.
	 */
	ComponentT &operator()();

	/**
	 * @brief Gets the entity id tied to the wrapped component instance.
	 * @return Decimal unsigned number representing entity id.
	 */
	const uint64 &eID() const;

	/**
	 * @brief Gets the unwrapped component instance.
	 * @return The const unwrapped component instance.
	 */
	const ComponentT &eComponent() const;

	/**
	 * @brief Gets the unwrapped component instance.
	 * @return The unwrapped component instance.
	 */
	ComponentT &eComponent();

	/**
	 * @brief Prints component type name to the console.
	 * 
	 * It's purpose is for debugging only.
	 */
	void printType() const;

private:
	ComponentT m_component;  /**< The wrapped component instance. */
	uint64 m_entityID;       /**< The entity id tied to the componentn instance. */
};

template <typename ComponentT>
ComponentWrapper<ComponentT>::ComponentWrapper(const uint64 &entity_id)
: m_component(), m_entityID(entity_id)
{ }

template <typename ComponentT>
ComponentWrapper<ComponentT>::ComponentWrapper(const ComponentT &comp)
: m_component(comp), m_entityID(0)
{ }

template <typename ComponentT>
const ComponentT &ComponentWrapper<ComponentT>::operator()() const
{
	return m_component;
}

template <typename ComponentT>
ComponentT &ComponentWrapper<ComponentT>::operator()()
{
	return m_component;
}

template <typename ComponentT>
const uint64 &ComponentWrapper<ComponentT>::eID() const
{
	return m_entityID;
}

template <typename ComponentT>
const ComponentT &ComponentWrapper<ComponentT>::eComponent() const
{
	return m_component;
}

template <typename ComponentT>
ComponentT &ComponentWrapper<ComponentT>::eComponent()
{
	return m_component;
}

template <typename ComponentT>
void ComponentWrapper<ComponentT>::printType() const
{
	std::cout << util::type_name_to_string<ComponentT>();
}

}  // namespace ecs
