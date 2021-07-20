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
	ComponentWrapper(const uint64 &entity_id) : m_component(), m_entityID(entity_id) { }
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
	using m_cPool = meta::ComponentPool<ComponentWrapper<Typepack> ...>;
public:
	ComponentBuffer() = default;

	template <typename ComponentT>
	auto &getComponentBucket()  // WRAPS COMPONENT
	{
		// TBD : throw exception when given component does not exist in ComponentPool
		return std::get<meta::IndexOf<ComponentWrapper<ComponentT>, m_cPool>>(m_cBuffer);
	}

	template <typename ComponentT>
	auto &getComponent(const uint64 entity_id)  // DOES NOT WRAP COMPONENT, UNWRAPS ON RETURN
	{
		for(auto &iter : this->getComponentBucket<ComponentT>())
		{
			if(iter.eID() == entity_id)
			{
				return iter();
			}
		}
		throw std::out_of_range("template <typename ComponentT> auto &getComponent(const uint64 entity_id): There are no components under given Entity ID.");
	}

	// Returns std::tuple of components matching entity_id
	template <typename... ComponentListT>
	auto getComponentsMatching(const uint64 &entity_id)
	{
		// TBD : Check whether such Entity ID was ever introduced into any component (maybe separate vector of ids?)
		auto result = (std::make_tuple(getComponent<ComponentListT>(entity_id)...));
		return result;
	}

	template <typename ComponentT>
	auto &addComponent(const uint64 &entity_id) noexcept  // WRAPS COMPONENT, UNWRAPS ON RETURN
	{
		return this->getComponentBucket<ComponentT>().emplace_back(ComponentWrapper<ComponentT>(entity_id))();
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
