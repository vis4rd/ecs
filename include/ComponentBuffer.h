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
	explicit ComponentWrapper(const ComponentT &comp) : m_component(comp), m_entityID(0) { }  // TEMPORARY???
	const ComponentT &operator()() const { return m_component; }
	ComponentT &operator()() { return m_component; }
	const uint64 &eID() const { return m_entityID; }
	void printType() const { std::cout << util::type_name_to_string<ComponentT>(); }

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
	ComponentBuffer()
	{
		this->FillRuntimePool();
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
				"template <typename ComponentT> ComponentT &getComponent(const uint64 entity_id): There is no such component under given Entity ID.");
		}
		else
		{
			throw std::invalid_argument(
				"template <typename ComponentT> ComponentT &getComponent(const uint64 entity_id): There's no such component in ComponentPool.");
		}
	}

	// If such component already exists, return it. Otherwise, create a new one with given arguments.
	template <typename ComponentT>
	ComponentT &tryGetComponent(const uint64 entity_id) noexcept
	// DOES NOT WRAP COMPONENT, UNWRAPS ON RETURN, CHECKS IF EXISTS
	{
		static_assert(meta::DoesTypeExist<ComponentT, m_tPool>, "template <typename ComponentT> ComponentT &getComponent(const uint64 entity_id) noexcept: There is no such component in the pool.");
		for(auto &iter : this->getComponentBucket<ComponentT>())
		{
			if(iter.eID() == entity_id)
			{
				return iter();
			}
		}
		return this->addComponent<ComponentT>(entity_id);
	}

	// Returns std::tuple of components matching entity_id
	// It does not check whether user passed any component types as template parameters.
	// Exception will be thrown only when returned value will be tried to be accessed.
	template <typename... ComponentListT>
	auto getComponentsMatching(const uint64 entity_id)
	{
		// TBD : Check whether such Entity ID was ever introduced into any component (maybe
		//       separate vector of ids?)
		auto result = (std::make_tuple(getComponent<ComponentListT>(entity_id)...));
		return result;
	}

	template <typename ComponentT>
	auto &addComponent(const uint64 entity_id) noexcept 
	// WRAPS COMPONENT, UNWRAPS ON RETURN, NO "EXISTS" CHECK
	{
		return this->getComponentBucket<ComponentT>().emplace_back(
			ComponentWrapper<ComponentT>(entity_id))();
		// there's additional parenthesis at the end to unwrap the component from ComponentWrapper
	}

	template <uint16 decimalIndex>
	auto &addComponentByIndex(const uint64 entity_id) noexcept
	// WRAPS COMPONENT, UNWRAPS ON RETURN, NO "EXISTS" CHECK
	{
		return this->tryGetComponent<meta::TypeAt<decimalIndex, m_tPool>>(entity_id);
	}

	void addComponentByIndex(const uint16 type_index, const uint64 entity_id)
	{
		// very ugly temporary workaround
		switch(type_index)
		{
			case uint16{0}: { if constexpr(0 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<0, m_tPool>>(entity_id));} break; }
			case uint16{1}: { if constexpr(1 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<1, m_tPool>>(entity_id));} break; }
			case uint16{2}: { if constexpr(2 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<2, m_tPool>>(entity_id));} break; }
			case uint16{3}: { if constexpr(3 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<3, m_tPool>>(entity_id));} break; }
			case uint16{4}: { if constexpr(4 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<4, m_tPool>>(entity_id));} break; }
			case uint16{5}: { if constexpr(5 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<5, m_tPool>>(entity_id));} break; }
			case uint16{6}: { if constexpr(6 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<6, m_tPool>>(entity_id));} break; }
			case uint16{7}: { if constexpr(7 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<7, m_tPool>>(entity_id));} break; }
			case uint16{8}: { if constexpr(8 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<8, m_tPool>>(entity_id));} break; }
			case uint16{9}: { if constexpr(9 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<9, m_tPool>>(entity_id));} break; }
			case uint16{10}: { if constexpr(10 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<10, m_tPool>>(entity_id));} break; }
			case uint16{11}: { if constexpr(11 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<11, m_tPool>>(entity_id));} break; }
			case uint16{12}: { if constexpr(12 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<12, m_tPool>>(entity_id));} break; }
			case uint16{13}: { if constexpr(13 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<13, m_tPool>>(entity_id));} break; }
			case uint16{14}: { if constexpr(14 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<14, m_tPool>>(entity_id));} break; }
			case uint16{15}: { if constexpr(15 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<15, m_tPool>>(entity_id));} break; }
			case uint16{16}: { if constexpr(16 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<16, m_tPool>>(entity_id));} break; }
			case uint16{17}: { if constexpr(17 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<17, m_tPool>>(entity_id));} break; }
			case uint16{18}: { if constexpr(18 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<18, m_tPool>>(entity_id));} break; }
			case uint16{19}: { if constexpr(19 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<19, m_tPool>>(entity_id));} break; }
			case uint16{20}: { if constexpr(20 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<20, m_tPool>>(entity_id));} break; }
			case uint16{21}: { if constexpr(21 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<21, m_tPool>>(entity_id));} break; }
			case uint16{22}: { if constexpr(22 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<22, m_tPool>>(entity_id));} break; }
			case uint16{23}: { if constexpr(23 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<23, m_tPool>>(entity_id));} break; }
			case uint16{24}: { if constexpr(24 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<24, m_tPool>>(entity_id));} break; }
			case uint16{25}: { if constexpr(25 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<25, m_tPool>>(entity_id));} break; }
			case uint16{26}: { if constexpr(26 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<26, m_tPool>>(entity_id));} break; }
			case uint16{27}: { if constexpr(27 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<27, m_tPool>>(entity_id));} break; }
			case uint16{28}: { if constexpr(28 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<28, m_tPool>>(entity_id));} break; }
			case uint16{29}: { if constexpr(29 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<29, m_tPool>>(entity_id));} break; }
			case uint16{30}: { if constexpr(30 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<30, m_tPool>>(entity_id));} break; }
			case uint16{31}: { if constexpr(31 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<31, m_tPool>>(entity_id));} break; }
			case uint16{32}: { if constexpr(32 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<32, m_tPool>>(entity_id));} break; }
			case uint16{33}: { if constexpr(33 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<33, m_tPool>>(entity_id));} break; }
			case uint16{34}: { if constexpr(34 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<34, m_tPool>>(entity_id));} break; }
			case uint16{35}: { if constexpr(35 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<35, m_tPool>>(entity_id));} break; }
			case uint16{36}: { if constexpr(36 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<36, m_tPool>>(entity_id));} break; }
			case uint16{37}: { if constexpr(37 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<37, m_tPool>>(entity_id));} break; }
			case uint16{38}: { if constexpr(38 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<38, m_tPool>>(entity_id));} break; }
			case uint16{39}: { if constexpr(39 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<39, m_tPool>>(entity_id));} break; }
			case uint16{40}: { if constexpr(40 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<40, m_tPool>>(entity_id));} break; }
			case uint16{41}: { if constexpr(41 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<41, m_tPool>>(entity_id));} break; }
			case uint16{42}: { if constexpr(42 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<42, m_tPool>>(entity_id));} break; }
			case uint16{43}: { if constexpr(43 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<43, m_tPool>>(entity_id));} break; }
			case uint16{44}: { if constexpr(44 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<44, m_tPool>>(entity_id));} break; }
			case uint16{45}: { if constexpr(45 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<45, m_tPool>>(entity_id));} break; }
			case uint16{46}: { if constexpr(46 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<46, m_tPool>>(entity_id));} break; }
			case uint16{47}: { if constexpr(47 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<47, m_tPool>>(entity_id));} break; }
			case uint16{48}: { if constexpr(48 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<48, m_tPool>>(entity_id));} break; }
			case uint16{49}: { if constexpr(49 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<49, m_tPool>>(entity_id));} break; }
			case uint16{50}: { if constexpr(50 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<50, m_tPool>>(entity_id));} break; }
			case uint16{51}: { if constexpr(51 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<51, m_tPool>>(entity_id));} break; }
			case uint16{52}: { if constexpr(52 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<52, m_tPool>>(entity_id));} break; }
			case uint16{53}: { if constexpr(53 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<53, m_tPool>>(entity_id));} break; }
			case uint16{54}: { if constexpr(54 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<54, m_tPool>>(entity_id));} break; }
			case uint16{55}: { if constexpr(55 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<55, m_tPool>>(entity_id));} break; }
			case uint16{56}: { if constexpr(56 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<56, m_tPool>>(entity_id));} break; }
			case uint16{57}: { if constexpr(57 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<57, m_tPool>>(entity_id));} break; }
			case uint16{58}: { if constexpr(58 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<58, m_tPool>>(entity_id));} break; }
			case uint16{59}: { if constexpr(59 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<59, m_tPool>>(entity_id));} break; }
			case uint16{60}: { if constexpr(60 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<60, m_tPool>>(entity_id));} break; }
			case uint16{61}: { if constexpr(61 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<61, m_tPool>>(entity_id));} break; }
			case uint16{62}: { if constexpr(62 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<62, m_tPool>>(entity_id));} break; }
			case uint16{63}: { if constexpr(63 >= sizeof...(Typepack)){ return; } else{ m_cBuffer[type_index].emplace_back(ComponentWrapper<meta::TypeAt<63, m_tPool>>(entity_id));} break; }
			default:
			{
				throw std::out_of_range(
				"auto &addComponentByIndex(const uint16 type_index, const uint64 entity_id): Given index is out of range (0-63).");
				break;
			}
		}
	}

	// Removes all compononents with given entity_id (in all vectors)
	void removeComponents(const uint64 entity_id) noexcept
	{
		if(this->size() == 0)
		{
			std::cout << "[WARNING] The buffer is empty." << std::endl;
		}
		else
		{
			(this->removeComponent<Typepack>(entity_id), ...);
		}
	}

	// Remove signle component with given type and Entity ID. Both arguments must be valid.
	template <typename ComponentT>
	void removeComponent(const uint64 entity_id)
	{
		if constexpr(meta::DoesTypeExist<ComponentT, m_tPool>)
		{
			auto &vec = this->getComponentBucket<ComponentT>();
			if(vec.size() != 0)
			{
				for(auto it = vec.begin(); it < vec.end(); it++)
				{
					if(std::any_cast<ComponentWrapper<ComponentT>>(*it).eID() == entity_id)
					{
						std::swap(*it, vec.back());
						vec.pop_back();
						return;
					}
				}
				throw std::out_of_range(
					"template <typename ComponentT> auto &getComponent(const uint64 entity_id): There is no such component under given Entity ID.");
			}
		}
		else
		{
			throw std::invalid_argument(
				"template <typename ComponentT> auto &getComponentBucket(): There's no such component in ComponentPool.");
		}
	}

	const uint64 size() const
	{
		uint64 result = uint64{0};
		for(auto &it : m_cBuffer)
		{
			result += it.size();
		}
		return result;
	}

	template <typename ComponentT>
	const uint64 bucketSize() const
	{
		return this->getComponentBucket<ComponentT>().size();
	}

	void printAll() const
	{
		std::cout << "ComponentBuffer:" << std::endl << "types = ";
		meta::metautil::Print<m_tPool>();
		std::cout << std::endl;
		(this->printComponentVector<Typepack>(), ...);
	}

private:

	// Returns a vector of specified component types
	template <typename ComponentT>
	std::vector<std::any> &getComponentBucket()
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
	const std::vector<std::any> &getComponentBucket() const
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

	constexpr void FillRuntimePool()
	{
		FillRuntimePoolImpl<sizeof... (Typepack) - 1>();
	}

	template <uint16 Iter>
	constexpr void FillRuntimePoolImpl()
	{
		m_runtimeTypePool[Iter] = meta::TypeAt<Iter, m_tPool>();
		if constexpr(Iter > uint16{0})
		{
			FillRuntimePoolImpl<Iter-uint16{1}>();
		}
	}

	template <typename ComponentT>
	ComponentWrapper<ComponentT> &getBufferComponent(const uint16 index)
	{
		return std::any_cast<ComponentWrapper<ComponentT>>(m_cBuffer[meta::IndexOf<ComponentT, m_tPool>][index]);
	}

	template <typename ComponentT>
	ComponentWrapper<ComponentT> &getBufferComponent(const uint16 comp_index, const uint16 index)
	{
		return std::any_cast<ComponentWrapper<ComponentT>>(m_cBuffer[comp_index][index]);
	}

	template <typename ComponentT>
	ComponentT &getUnwrappedBufferComponent(const uint16 index)
	{
		return std::any_cast<ComponentT>(m_cBuffer[meta::IndexOf<ComponentT, m_tPool>][index]());
	}

	template <typename ComponentT>
	ComponentT &getUnwrappedBufferComponent(const uint16 comp_index, const uint16 index)
	{
		return std::any_cast<ComponentT>(m_cBuffer[comp_index][index]());
	}

	template <typename ComponentT>
	const void printComponentVector() const
	{
		std::cout << util::type_name_to_string<ComponentT>() << ": { ";
		for(auto it : this->getComponentBucket<ComponentT>())
		{
			std::cout << std::any_cast<ComponentWrapper<ComponentT>>(it)() << " ";
		}
		std::cout << "}" << std::endl;
	}

private:
	std::array<std::vector<std::any>, sizeof... (Typepack)> m_cBuffer;
	std::array<std::any, sizeof... (Typepack)> m_runtimeTypePool;
};

// ComponentBuffer has to know somehow which components belong to which entities.
// To achieve that, there are several ways:
// 1) Create template struct wrapper containing component, id of entity and operator() overload;
// 2) Create vector of vectors of entity ids which would have indentical structure as the buffer;
// 3) Let entity ids be handled outside ComponentBuffer class by some other manager.

}  // namespace ecs
