#pragma once

#include "Meta.h"

namespace ecs
{

namespace meta
{
	template <typename... Typepack>
	using ComponentPool = TypeList<Typepack ...>;
}  // namespace meta

template <typename ComponentT>
class ComponentWrapper
{
public:
	ComponentWrapper() = default;
	explicit ComponentWrapper(const uint64 &entity_id) : m_component(), m_entityID(entity_id) { }
	ComponentWrapper(const ComponentT &comp) : m_component(comp), m_entityID(0) { }  // TEMPORARY???
	ComponentT &operator()() { return m_component; }
	const uint64 &eID() const { return m_entityID; }

private:
	ComponentT m_component;
	uint64 m_entityID;
};

template <typename TypeListT>
class ComponentBuffer;

template <typename... Typepack>
class ComponentBuffer<meta::TypeList<Typepack...>>
{
	using m_cPool = meta::ComponentPool<ComponentWrapper<Typepack> ...>;  // WRAPPED
	using m_tPool = meta::TypeList<Typepack...>;  // NOT WRAPPED
public:
	ComponentBuffer() = default;

	// Returns a vector of specified component types
	template <typename ComponentT>
	std::vector<ComponentWrapper<ComponentT>> &getComponentBucket()
	// WRAPS COMPONENT, DOES NOT UNWRAP ON RETURN
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

	template <typename ComponentT>
	ComponentT &getComponent(const uint64 entity_id)  // DOES NOT WRAP COMPONENT, UNWRAPS ON RETURN
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
				"template <typename ComponentT> auto &getComponent(const uint64 entity_id): There is no such component under given Entity ID.");
		}
		else
		{
			throw std::invalid_argument(
				"template <typename ComponentT> auto &getComponentBucket(): There's no such component in ComponentPool.");
		}
	}

	// Returns std::tuple of components matching entity_id
	// It does not check whether user passed any component types as template parameters.
	// Exception will be thrown only when returned value will be tried to be accessed.
	template <typename... ComponentListT>
	auto getComponentsMatching(const uint64 &entity_id)
	{
		// TBD : Check whether such Entity ID was ever introduced into any component (maybe
		//       separate vector of ids?)
		auto result = (std::make_tuple(getComponent<ComponentListT>(entity_id)...));
		return result;
	}

	template <typename ComponentT>
	auto &addComponent(const uint64 &entity_id)  // WRAPS COMPONENT, UNWRAPS ON RETURN
	{
		return this->getComponentBucket<ComponentT>().emplace_back(
			ComponentWrapper<ComponentT>(entity_id))();
		// there's additional parenthesis at the end to unwrap the component from COmponentWrapper
	}

private:
	meta::metautil::TupleOfVectorsOfTypes<m_cPool> m_cBuffer;
};

// ComponentBuffer has to know somehow which components belong to which entities.
// To achieve that, there are several ways:
// 1) Create template struct wrapper containing component, id of entity and operator() overload;
// 2) Create vector of vectors of entity ids which would have indentical structure as the buffer;
// 3) Let entity ids be handled outside ComponentBuffer class by some other manager.

}  // namespace ecs
