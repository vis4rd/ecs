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
	 * @note If there's no component with given entity id, the returned value is std::nullopt
	 *       instead.
	 * 
	 * @warning Although this method is marked noexcept, it still can terminate the program when
	 *          given decimalIndex is invalid (out of range).
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
	 * @note If there's no component with given entity id, the returned value is std::nullopt
	 *       instead.
	 * 
	 * @warning Although this method is marked noexcept, it still can terminate the program when
	 *          given decimalIndex is invalid (out of range).
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

// ################################################################################################
// Constructor

template <typename... Typepack>
ComponentBuffer<meta::TypeList<Typepack...>>::ComponentBuffer(const uint64 max_entity_count)
:
m_maxEntityCount(max_entity_count)
{
	auto help = [&](const uint64 &max, auto &vec)
	{
		if(vec.capacity() < max)
		{
			vec.reserve(max);
		}
	};
	((help(max_entity_count, this->getComponentBucket<Typepack>())), ...);
}


// ################################################################################################
// getComponentBucket()

template <typename... Typepack>
template <typename ComponentT>
std::vector<ComponentWrapper<ComponentT>> &ComponentBuffer<meta::TypeList<Typepack...>>::getComponentBucket()
{
	if constexpr(meta::DoesTypeExist<ComponentT, m_tPool>)
	{
		return std::get<meta::IndexOf<ComponentWrapper<ComponentT>, m_cPool>>(m_cBuffer);
	}
	else
	{
		throw std::invalid_argument(
			"template <typename ComponentT> auto &getComponentBucket(): There's no such component in ComponentPool.");
	}
}

// ################################################################################################
// tryGetComponent()

template <typename... Typepack>
template <typename ComponentT>
std::optional<ComponentT> &ComponentBuffer<meta::TypeList<Typepack...>>::tryGetComponent(const uint64 entity_id) noexcept
{
	if constexpr(meta::DoesTypeExist<ComponentT, m_tPool>)
	{
		ComponentT &result = ComponentT{};
		try
		{
			result = this->getComponent<ComponentT>(entity_id);
		}
		catch(const std::exception &e)
		{
			std::cout << "[WARNING] std::optional<ComponentT> &tryGetComponent(const uint64 entity_id) noexcept" << std::endl
				<< "    " << e.what() << std::endl;
			return this->addComponent<ComponentT>(entity_id);
		}
		// if not caught any exception, the component exists, hence it is returned
		return result;
	}
	else
	{
		std::cout << "[WARNING] std::optional<ComponentT> &tryGetComponent(const uint64 entity_id) noexcept: There's no such component in ComponentPool." << std::endl;
		return std::nullopt;
	}
}

// ################################################################################################
// getComponent()

template <typename... Typepack>
template <typename ComponentT>
ComponentT &ComponentBuffer<meta::TypeList<Typepack...>>::getComponent(const uint64 entity_id)
{
	if constexpr(meta::DoesTypeExist<ComponentT, m_tPool>)
	{
		for(auto &iter : this->getComponentBucket<ComponentT>())
		{
			if(iter.eID() == entity_id)
			{
				return iter();
			}
		}
		throw std::out_of_range(
			"template <typename ComponentT> ComponentT &getComponent(const uint64 entity_id): There is no such component under given Entity ID.");
	}
	else
	{
		throw std::invalid_argument(
			"template <typename ComponentT> ComponentT &getComponent(const uint64 entity_id): There's no such component in ComponentPool.");
	}
}

// ################################################################################################
// getComponentsMatching()

template <typename... Typepack>
template <typename... ComponentListT>
auto ComponentBuffer<meta::TypeList<Typepack...>>::getComponentsMatching(const uint64 entity_id)
{
	// TBD : Check whether such Entity ID was ever introduced into any component (maybe
	//       separate vector of ids?)
	return (std::forward_as_tuple(getComponent<ComponentListT>(entity_id)...));
}

// ################################################################################################
// addComponent()

template <typename... Typepack>
template <typename ComponentT>
auto &ComponentBuffer<meta::TypeList<Typepack...>>::addComponent(const uint64 entity_id)
{
	return this->getComponentBucket<ComponentT>().emplace_back(
		ComponentWrapper<ComponentT>(entity_id))();
	// there's additional parenthesis at the end to unwrap the component from ComponentWrapper
}

// ################################################################################################
// addComponentByIndex

template <typename... Typepack>
template <uint16 decimalIndex>
auto &ComponentBuffer<meta::TypeList<Typepack...>>::addComponentByIndex(const uint64 entity_id)
{
	return std::get<decimalIndex>(m_cBuffer).emplace_back(
		ComponentWrapper<meta::TypeAt<decimalIndex, m_tPool>>(entity_id))();
}

// ################################################################################################
// getComponentByIndex

template <typename... Typepack>
template <uint16 decimalIndex>
const std::optional<meta::TypeAt<decimalIndex, meta::TypeList<Typepack...>>> ComponentBuffer<meta::TypeList<Typepack...>>::getComponentByIndex(const uint64 entity_id) const noexcept
{
	auto &vec = std::get<decimalIndex>(m_cBuffer);
	for(auto &c : vec)
	{
		if(c.eID() == entity_id)
		{
			return c();
		}
	}
	return std::nullopt;
}

// ################################################################################################
// getComponentByIndex

template <typename... Typepack>
template <uint16 decimalIndex>
std::optional<meta::TypeAt<decimalIndex, meta::TypeList<Typepack...>>> ComponentBuffer<meta::TypeList<Typepack...>>::getComponentByIndex(const uint64 entity_id) noexcept
{
	auto &vec = std::get<decimalIndex>(m_cBuffer);
	for(auto &c : vec)
	{
		if(c.eID() == entity_id)
		{
			return c();
		}
	}
	return std::nullopt;
}

// ################################################################################################
// clear()

template <typename... Typepack>
template <uint16 Index = (sizeof... (Typepack) - 1)>
void ComponentBuffer<meta::TypeList<Typepack...>>::clear() noexcept
{
	std::get<Index>(m_cBuffer).clear();
	if constexpr(Index > uint16{0})
	{
		this->clear<Index - uint16{1}>();
	}
}

// ################################################################################################
// checkComponent()

template <typename... Typepack>
template <uint16 Index>
const bool ComponentBuffer<meta::TypeList<Typepack...>>::checkComponent(const uint64 entity_id) const noexcept
{
	auto &vec = std::get<Index>(m_cBuffer);
	for(auto cw = vec.begin(); cw < vec.end(); cw++)
	{
		if(!(cw->eID() - entity_id))
		{
			return true;
		}
	}
	return false;
}

// ################################################################################################
// removeComponents()

template <typename... Typepack>
void ComponentBuffer<meta::TypeList<Typepack...>>::removeComponents(const uint64 entity_id) noexcept
{
	auto func = [&entity_id](auto& vec)
	{
		for(auto it = vec.begin(); it < vec.end(); it++)
		{
			if(it->eID() == entity_id)
			{
				// std::cout << "removing (" << (*it)() <<") of eID = " << it->eID() << std::endl;
				std::swap(*it, vec.back());
				vec.pop_back();
				break;
			}
		}
	};
	// std::cout << util::type_name_to_string<decltype(func)>() << std::endl;
	std::apply(
		[&](auto& ...vec)
		{
			(func(vec), ...);
		},
		m_cBuffer
	);
	// TBD : implement meta::ForEachType<TypeList>(std::function<>)
	//       Use std::apply above (?), should work (no temp_param, std::function as argument??)
}

// ################################################################################################
// removeComponent()

template <typename... Typepack>
template <typename ComponentT>
void ComponentBuffer<meta::TypeList<Typepack...>>::removeComponent(const uint64 entity_id)
{
	if constexpr(meta::DoesTypeExist<ComponentT, m_tPool>)
	{
		auto &vec = this->getComponentBucket<ComponentT>();
		for(auto it = vec.begin(); it < vec.end(); it++)
		{
			if(it->eID() == entity_id)
			{
				std::swap(*it, vec.back());
				vec.pop_back();
				return;
			}
		}
		throw std::out_of_range(
			"template <typename ComponentT> auto &getComponent(const uint64 entity_id): There is no such component under given Entity ID.");
	}
	else
	{
		throw std::invalid_argument(
			"template <typename ComponentT> auto &getComponentBucket(): There's no such component in ComponentPool.");
	}
}

// ################################################################################################
// size()

template <typename... Typepack>
const uint64 ComponentBuffer<meta::TypeList<Typepack...>>::size() const
{
	uint64 result = uint64{0};
	auto count = [&result](auto& vec)
	{
		result += vec.size();
	};
	// std::cout << util::type_name_to_string<decltype(func)>() << std::endl;
	std::apply(
		[&](auto& ...vec)
		{
			(count(vec), ...);
		},
		m_cBuffer
	);
	return result;
}

// ################################################################################################
// bucketSize()

template <typename... Typepack>
template <typename ComponentT>
const uint64 ComponentBuffer<meta::TypeList<Typepack...>>::bucketSize() const
{
	return this->getComponentBucket<ComponentT>().size();
}

// ################################################################################################
// printAll()

template <typename... Typepack>
void ComponentBuffer<meta::TypeList<Typepack...>>::printAll() const
{
	auto prt = [&](auto& vec)
	{
		if(!vec.empty())
		{
			vec[0].printType();
			std::cout << " = { ";
			for(auto &el : vec)
			{
				std::cout << el() << " ";
			}
			std::cout << "}" << std::endl;
		}
		else
		{
			vec[0].printType();
			std::cout << " = { }" << std::endl;
		}
	};

	std::cout << "ComponentBuffer:" << std::endl
		<< "types = ";
	meta::metautil::Print<m_tPool>();
	std::apply(
		[&](auto& ...vec)
		{
			(prt(vec), ...);
		},
		m_cBuffer
	);
	std::cout << std::endl;
}

// ComponentBuffer has to know somehow which components belong to which entities.
// To achieve that, there are several ways:
// 1) Create template struct wrapper containing component, id of entity and operator() overload;
// 2) Create vector of vectors of entity ids which would have indentical structure as the buffer;
// 3) Let entity ids be handled outside ComponentBuffer class by some other manager.

}  // namespace ecs
