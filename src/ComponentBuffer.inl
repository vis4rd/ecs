namespace ecs
{

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
	if constexpr(meta::DoesTypeExist<meta::TypeAt<decimalIndex, meta::TypeList<Typepack...>>, m_tPool>)  // type does not exist in component pool
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
	else
	{
		std::cout << "[WARNING] const std::optional<meta::TypeAt<decimalIndex, meta::TypeList<Typepack...>>> ComponentBuffer<meta::TypeList<Typepack...>>::getComponentByIndex(const uint64 entity_id) const noexcept: There's no sych type in the component pool." << std::endl;
		return std::nullopt;
	}
}

// ################################################################################################
// getComponentByIndex

template <typename... Typepack>
template <uint16 decimalIndex>
std::optional<meta::TypeAt<decimalIndex, meta::TypeList<Typepack...>>> ComponentBuffer<meta::TypeList<Typepack...>>::getComponentByIndex(const uint64 entity_id) noexcept
{
	if constexpr(meta::DoesTypeExist<meta::TypeAt<decimalIndex, meta::TypeList<Typepack...>>, m_tPool>)  // type does not exist in component pool
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
	else
	{
		std::cout << "[WARNING] std::optional<meta::TypeAt<decimalIndex, meta::TypeList<Typepack...>>> ComponentBuffer<meta::TypeList<Typepack...>>::getComponentByIndex(const uint64 entity_id) noexcept: There's no sych type in the component pool." << std::endl;
		return std::nullopt;
	}
}

// ################################################################################################
// clear()

template <typename... Typepack>
template <uint16 Index>
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
