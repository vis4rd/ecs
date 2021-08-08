#pragma once

#include "Root.h"
#include "Component.h"
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
	 * @brief Adds a new entity to the buffer.
	 * @param components The bitset of components, where every component has it's own bitwise position.
	 * @param flags The bitset of flags attached to entity, where every flag has it's own bitwise position.
	 * @tparam ComponentCount Number of components bound to this entity passed to the buffer.
	 * 
	 * It is recommended to specify ComponentCount when calling this method as components are added
	 *   recursively, but on the other hand it is not required to do so (at cost of performance).
	 */
	template <uint16 ComponentCount = uint16{64}>
	void addEntity(const uint64 components, const uint64 flags);

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
	 * The method is split in two. Each part is chosen depending on number of entities in the
	 *   buffer. If there are less than 5000, this method is making use of OpenMP pragma for
	 *   parallel for loops. Otherwise, there are std::threads used instead, since with such a high
	 *   entity quantity it's theoretically faster than #pragma omp parallel for.
	 */
	template <typename... ComponentListT>
	void applySystem(std::function<void(ComponentListT& ...)> system);

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

template <typename TypeListT>
uint64 Manager<TypeListT>::m_nextEntityID = uint64{0};

template <typename TypeListT>
Manager<TypeListT>::Manager(const uint64 max_entity_count)
:
m_flagCount(uint16{0}),
m_maxEntityCount(max_entity_count),
m_entityCount(uint64{0})
{
	if(m_entityBuffer.capacity() < m_maxEntityCount)
	{
		m_entityBuffer.reserve(m_maxEntityCount);
	}
	if(m_entityFlags.capacity() < m_maxEntityCount)
	{
		m_entityFlags.reserve(m_maxEntityCount);
	}
	if(m_entityComponents.capacity() < m_maxEntityCount)
	{
		m_entityComponents.reserve(m_maxEntityCount);
	}
}

template <typename TypeListT>
template <uint16 TypeIndex>
void Manager<TypeListT>::addComponent(const uint64 entity_id)
{
	if(m_componentBuffer.getComponentByIndex<TypeIndex>(entity_id))
	{
		std::cout << "[WARNING] Given component already exists under " << 
			"passed Entity ID - " <<std::endl <<"ignoring void Manager<TypeListT>::addComponent" <<
			"(const uint64 entity_id)" << std::endl;
		return;
	}
	else
	{
		// adding component to the buffer
		m_componentBuffer.addComponentByIndex<TypeIndex>(entity_id);

		// flipping the bit to 1
		uint16 i = uint16{0};
		for(; i < m_entityCount; i++)
		{
			if(m_entityBuffer.at(i) == entity_id)
			{
				break;
			}
		}
		m_entityComponents[i] |= (uint64{1} << TypeIndex);
	}
}

template <typename TypeListT>
template <uint16 TypeIndex>
meta::TypeAt<TypeIndex, TypeListT> &Manager<TypeListT>::getComponent(const uint64 entity_id)
{
	return this->getComponent<meta::TypeAt<TypeIndex, TypeListT>>(entity_id);
}

template <typename TypeListT>
template <typename ComponentT>
ComponentT &Manager<TypeListT>::getComponent(const uint64 entity_id)
{
	return m_componentBuffer.template getComponent<ComponentT>(entity_id);
}

template <typename TypeListT>
template <uint16 TypeIndex>
std::vector<ComponentWrapper<meta::TypeAt<TypeIndex, TypeListT>>> &Manager<TypeListT>::getComponentBucket()
{
	return m_componentBuffer.template getComponentBucket<meta::TypeAt<TypeIndex, TypeListT>>();
}

template <typename TypeListT>
template <typename ComponentT>
std::vector<ComponentWrapper<ComponentT>> &Manager<TypeListT>::getComponentBucket()
{
	return m_componentBuffer.template getComponentBucket<ComponentT>();
}

template <typename TypeListT>
template <uint16 TypeIndex>
const bool Manager<TypeListT>::checkComponent(const uint64 entity_id) const noexcept
{
	if(meta::DoesTypeExist<meta::TypeAt<TypeIndex, TypeListT>, TypeListT>)
	{
		// returned value is of type std::optional<type>
		if(m_componentBuffer.template checkComponent<TypeIndex>(entity_id))  // result contains some value
		{
			return true;
		}
	}
	return false;
}

template <typename TypeListT>
template <uint16 ComponentCount>
void Manager<TypeListT>::addEntity(const uint64 components, const uint64 flags)
{
	if(m_entityCount < m_maxEntityCount)
	{
		m_entityCount++;
		m_entityBuffer.push_back(m_nextEntityID++);

		// parsing components
		this->addEntityComponents<ComponentCount-1>(components, m_entityBuffer.back());

		// adding flags
		m_entityFlags.push_back(flags);

		// adding components
		m_entityComponents.push_back(components);

		// TBD : use disposed entities instead of assigning new ones everytime
	}
	else
	{
		std::cout << "[WARNING] Number of allocated entities has reached the cap," <<
			" ignoring the new ones..." << std::endl;
	}
}

template <typename TypeListT>
void Manager<TypeListT>::deleteEntity(const uint64 entity_id)
{
	// TBD : do nothing if no such entity_id is found
	auto e = m_entityBuffer.begin();
	bool exists = false;
	for(; e < m_entityBuffer.end(); e++)
	{
		if(*e == entity_id)
		{
			break;
			exists = true;
		}
	}
	if(exists)
	{
		m_componentBuffer.removeComponents(*e);
		auto pos = e - m_entityBuffer.begin();
		std::swap(*e, m_entityBuffer.back());
		m_entityBuffer.pop_back();
		std::swap(m_entityFlags.at(pos), m_entityFlags.back());
		m_entityFlags.pop_back();
		std::swap(m_entityComponents.at(pos), m_entityComponents.back());
		m_entityComponents.pop_back();
		m_entityCount--;
	}
}

template <typename TypeListT>
const bool Manager<TypeListT>::checkEntity(const uint64 entity_id) const noexcept
{
	for(auto &e : m_entityBuffer)
	{
		if(e == entity_id)
		{
			return true;
		}
	}
	return false;
}

template <typename TypeListT>
void Manager<TypeListT>::deleteAllEntities()
{
	// remove all components
	m_componentBuffer.clear();
	m_entityComponents.clear();

	// remove all flags
	m_entityFlags.clear();
	m_flagCount = uint16{0};

	// clear entity buffer
	m_entityBuffer.clear();
	m_entityCount = uint64{0};
}

template <typename TypeListT>
const uint64 &Manager<TypeListT>::getCurrentEntityCount() const noexcept
{
	return m_entityCount;
}

template <typename TypeListT>
const uint64 &Manager<TypeListT>::getMaxEntityCount() const noexcept
{
	return m_maxEntityCount;
}

template <typename TypeListT>
const bool Manager<TypeListT>::getFlag(const uint64 flagBit, const uint64 entity_id) const
{
	auto fl = m_entityFlags.begin();
	for(auto id = m_entityBuffer.begin(); id != m_entityBuffer.end(); id++, fl++)
	{
		if(*id == entity_id)
		{
			return (flagBit & *fl);
		}
	}
	return false;
}

template <typename TypeListT>
void Manager<TypeListT>::setFlag(const uint64 flagBit, const uint64 entity_id, const bool value) noexcept
{
	// TBD: do nothing if entity_id doesn't exist, claim this noexcept
	auto fl = m_entityFlags.begin();
	for(auto id = m_entityBuffer.begin(); id != m_entityBuffer.end(); id++, fl++)
	{
		if(*id == entity_id)
		{
			(*fl) ^= (-value ^ (*fl)) & flagBit;  // sets the flagBit bit to value
			break;
		}
	}
}

template <typename TypeListT>
void Manager<TypeListT>::setFlagsForAll(const uint64 flagBit, const bool value)
{
	#pragma omp parallel for
	for(auto fl = m_entityFlags.begin(); fl != m_entityFlags.end(); fl++)
	{
		(*fl) ^= (-value ^ (*fl)) & flagBit;  // sets the flagBit bit to value
	}
}

template <typename TypeListT>
std::vector<uint64> &Manager<TypeListT>::getFlagBuffer()
{
	return m_entityFlags;
}


// apply function to all entities holding required components
//
// example:
// 	void system(int &arg1) { arg1 = 5; }
// 	std::function<void(int&)> fun = system;
// 
// 	// Let's assume that ComponentBuffer has an int as one of the components. Then:
// 
// 	using CPool = ecs::meta::ComponentPool<int>;  // creating pool of components for the manager
// 	ecs::Manager<CPool> manager;  // holds only one component - int
// 	manager.addEntity<1>(ecs::uint64{1} << 63, ecs::uint64{0});
// 	// The second argument are flags, we pass 0 here to simplify the example.
// 	// Notice that the first (and only) component has the left-most bit position.
// 
// 	manager.applySystem<int>(fun);
//  
//  // The system(int&) function has been applied to every entity with int component.
// example-end
// 
// Template parameters are components that filter entities for which the system will be applied.
// The fewer components are required, the faster the application will work.
// Given components and system arguments have to match 100% (including the same order of passing)!
// System's arguments have to be references, otherwise the value will be copied and the whole
//   operation would not make any sense.
// 
// The method is split in two. Each part is being chosen depending on number of entities
//   in the buffer. If there are less than 5000, this method is making use of OpenMP pragma
//   for parallel for loops. Otherwise, there are std::threads used instead, since with such a high
//   entity quantity it's theoretically faster than #pragma omp parallel for.

template <typename TypeListT>
template <typename... ComponentListT>
void Manager<TypeListT>::applySystem(std::function<void(ComponentListT& ...)> system)
{
	// creating a bitset which will be compared with m_entityComponents vector
	uint64 bitset = uint64{0};
	((bitset |= (uint64{1} << (63 - meta::IndexOf<ComponentListT, TypeListT>))), ...);

	if(m_entityCount > 5000)  // should multithreading be applied
	{
		// constructing function which will be executed by parallel threads
		auto execute = [&bitset, system, this](const uint64 start, const uint64 stop)
		{
			for(uint64 i = start; i < stop; i++)
			{
				if((bitset & m_entityComponents[i]) == bitset)  // if tested entity has requested components
				{
					// for every matching entity, pass to system (which in fact is an ECS System) tuple of arguments
					std::apply(system, this->getMatchingComponentPack<ComponentListT...>(m_entityBuffer[i]));
				}
			}
		};

		// we are splitting indices between threads to make this function more efficient
		// ex.:
		// batch = entity_count / thread_count
		// start_index = i * batch
		// stop_index = (i + 1) * batch - 1
		// ex. batch = 3
		// thread(i): execute(start_index, stop_index)
		// thread(0): execute(0, 2)
		// thread(1): execute(3, 5)
		// thread(11): execute(33, 35)
		auto thread_number = std::thread::hardware_concurrency() * 2;  // number of threads recommended
		float batch = m_entityCount / static_cast<float>(thread_number);  // number of handled indices per thread
		std::vector<std::thread> threads;
		for(auto i = 0u; i < thread_number; i++)
		{
			threads.push_back(std::thread(
				execute,
				std::lround(i * batch),  // start
				std::lround((i + 1) * batch - 1)));  // stop
		}
		for(auto &th : threads)
		{
			th.join();
		}
	}
	else  // there are too few entities to have multithreading more performant
	{
		// using OpenMP
		#pragma omp parallel for
		for(uint64 i = uint64{0}; i < m_entityCount; i++)
		{
			if((bitset & m_entityComponents[i]) == bitset)  // if tested entity has requested components
			{
				// for every matching entity, pass to system (which in fact is an ECS System) tuple of arguments
				std::apply(system, this->getMatchingComponentPack<ComponentListT...>(m_entityBuffer[i]));
			}
		}  // for
	}  // else
}  // method

template <typename TypeListT>
template <typename... ComponentListT>
void Manager<TypeListT>::applySystem(void (*system)(ComponentListT& ...))
{
	// creating a bitset which will be compared with m_entityComponents vector
	uint64 bitset = uint64{0};
	((bitset |= (uint64{1} << (63 - meta::IndexOf<ComponentListT, TypeListT>))), ...);

	if(m_entityCount > 5000)  // should multithreading be applied
	{
		// constructing function which will be executed by parallel threads
		auto execute = [&bitset, system, this](const uint64 start, const uint64 stop)
		{
			for(uint64 i = start; i < stop; i++)
			{
				if((bitset & m_entityComponents[i]) == bitset)  // if tested entity has requested components
				{
					// for every matching entity, pass to system (which in fact is an ECS System) tuple of arguments
					std::apply(system, this->getMatchingComponentPack<ComponentListT...>(m_entityBuffer[i]));
				}
			}
		};

		// description in void Manager<TypeListT>::applySystem(std::function<void(ComponentListT& ...)> system)
		auto thread_number = std::thread::hardware_concurrency() * 2;  // number of threads recommended
		float batch = m_entityCount / static_cast<float>(thread_number);  // number of handled indices per thread
		std::vector<std::thread> threads;
		for(auto i = 0u; i < thread_number; i++)
		{
			threads.push_back(std::thread(
				execute,
				std::lround(i * batch),  // start
				std::lround((i + 1) * batch - 1)));  // stop
		}
		for(auto &th : threads)
		{
			th.join();
		}
	}
	else  // there are too few entities to have multithreading more performant
	{
		// using OpenMP
		#pragma omp parallel for
		for(uint64 i = uint64{0}; i < m_entityCount; i++)
		{
			if((bitset & m_entityComponents[i]) == bitset)  // if tested entity has requested components
			{
				// for every matching entity, pass to system (which in fact is an ECS System) tuple of arguments
				std::apply(system, this->getMatchingComponentPack<ComponentListT...>(m_entityBuffer[i]));
			}
		}  // for
	}  // else
}  // method

// PRIVATE
template <typename TypeListT>
template <uint16 Index>
void Manager<TypeListT>::addEntityComponents(const uint64 components, const uint64 &entity_id)
{
	if constexpr(Index >= m_componentCount)
	{
		std::cout << "[WARNING] Component bit position has exceeded " <<
			"known component count (position: " << Index << ", component count: " <<
			m_componentCount << ") - breaking operation..." << std::endl;
	}
	else
	{
		if(((uint64{1} << Index) & components) == (uint64{1} << Index))  // if bit at position Index is equal to 1
		{
			m_componentBuffer.template addComponentByIndex<Index>(entity_id);
		}
	}
	if constexpr(Index > uint16{0})
	{
		this->addEntityComponents<Index - 1>(components, entity_id);
	}
}

template <typename TypeListT>
template <typename... ComponentListT>
auto Manager<TypeListT>::getMatchingComponentPack(const uint64 &entity_id)
{
	return m_componentBuffer.template getComponentsMatching<ComponentListT...>(entity_id);
}

}  // namespace ecs
