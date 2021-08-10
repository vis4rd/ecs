namespace ecs
{

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
