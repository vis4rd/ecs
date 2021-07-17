#pragma once

#include "Root.h"
#include "Util.h"

namespace ecs
{
namespace meta
{
	/*template <typename ...Typepack>
	struct TypeList
	{
		using Type = TypeList<Typepack ...>;

		template <uint64 Index>
  		using TypeAt = typename std::tuple_element<Index, std::tuple<Typepack ...>>::type;

  		template <typename Type>
  		static constexpr uint64 IndexOf()
  		{
  			return IndexOfHelper<0, Type>();
  		}

  		template <typename NewType>
  		using Append = TypeList<Typepack..., NewType>;

  		template <typename NewType>
  		using Prepend = TypeList<NewType, Typepack...>;

  	private:
  		template <uint64 Index, typename Type>
		static constexpr uint64 IndexOfHelper()
		{
		    if constexpr(std::is_same<Type, TypeAt<Index>>::value )
		    {
		        return Index;
		    }
		    else
		    {
		    	if constexpr(Index + 1 < TypeCount)
		    	{
		    		return IndexOfHelper<Index+1, Type>();
		    	}
		        else
		        {
		        	static_assert(Index + 1 < TypeCount, "Given type is not in the TypeList");
		        	return 65;
		        }
		    }
		}
	};*/

	// ############################################################################################
	// TypeList
	
	template <typename ...Typepack>
	struct TypeList;

	// ############################################################################################
	// Implementation of TypeListSize.
	
	template<typename TypeListT>
	struct TypeListSizeImpl;  // dummy struct used just to hold TypeList

	template<typename... Typepack>
	struct TypeListSizeImpl<TypeList<Typepack...>>  // specialization of TypeListSizeImpl
	{
	    static constexpr uint16 Size = sizeof...(Typepack);
	};
	template<typename TypeListT>
	constexpr uint16 TypeListSize = TypeListSizeImpl<TypeListT>::Size;

	// ############################################################################################
	// Implementation of TypeListAt.
	
	// 1)
	template<uint64 Index, typename TypeListT>
	struct TypeAtImpl;  // dummy struct used just to hold TypeList and call specialization
	// 2)
	template<uint64 Index, typename FirstType, typename... RestOfTypes>
	struct TypeAtImpl<Index, TypeList<FirstType, RestOfTypes...>>  // specialization of TypeAtImpl for N
	{
	    using Type = typename TypeAtImpl<Index - 1, TypeList<RestOfTypes...>>::Type;
	};
	// 3)
	template<typename FirstType, typename... RestOfTypes>
	struct TypeAtImpl<0, TypeList<FirstType, RestOfTypes...>>  // specialization of TypeAtImpl for 0
	{
	    using Type = FirstType;
	};
	// 4)
	template<uint64 Index, typename TypeListT>
	using TypeAt = typename TypeAtImpl<Index, TypeListT>::Type;

	// User calls (4).
	// (4) calls (1).
	// (1) calls (2), because (2) is a specialization of (1) and
	//   TypeList -> (through argument deduction) FirstType + ...RestOfTypes
	//   (extraction of the first type from TypeList).
	// (2) loops until calls specialization for 0 which is (3).
	// 
	// example:
	// TypeListAt<2, TypeList<T1, T2, T3>> =>
	// TypeListAtImpl<2, TypeList<T1, T2, T3>>::Type =>
	// TypeListAtImpl<1, TypeList<T2, T3>>::Type =>
	// TypeListAtImpl<0, TypeList<T3>>::Type =>
	// T3
	// 
	// example2:
	// TypeListAt<1, TypeList<T1, T2, T3>> =>
	// TypeListAtImpl<1, TypeList<T1, T2, T3>>::Type =>
	// TypeListAtImpl<0, TypeList<T2, T3>>::Type =>
	// T2

	// ############################################################################################

	// TBD: IndexOf (https://devblogs.microsoft.com/cppblog/build-throughput-series-more-efficient-template-metaprogramming/)

	/*template <typename ...Typepack>
	using ComponentPool = TypeList<Typepack...>;

	template <typename ...Typepack>
	std::tuple<std::vector<Typepack> ...> ComponentBufferContainerHelper(ComponentPool<std::vector<Typepack> ...>);

	template <typename ComponentPoolT>
	using ComponentBufferContainer = decltype(ComponentBufferContainerHelper(std::declval<ComponentPoolT>()));

	template <typename ComponentPool>
	struct ComponentBuffer
	{
	public:
		template <typename ComponentT, typename CPT = ComponentPool>
		auto &PushComponent()
		{
			const auto component_id = CPT::IndexOf<ComponentT>();
			auto &result = std::get<component_id>(m_buffer).emplace_back(std::declval<ComponentT>());
			return result;
		}
	private:
		ComponentBufferContainer<CPT> m_buffer;
	};*/

///////////////////////////////////////////////////////////////////////////////////////////////////

	namespace metautil
	{
		template <uint64 iter, typename List>
		constexpr void print_TypeList_Helper()
		{
		    if constexpr(iter >= uint64{0}
		    			&& iter < List::TypeCount)  // compile-time if
		    {
		    	static_assert(iter >= uint64{0},
		    		"void print_TypeList_Helper() : iter < 0");
			    std::cout << util::type_name_to_string<typename List::TypeOfIndex<iter>>() << std::endl;
			    print_TypeList_Helper<iter + 1, List>();
		    }
		    else
		    {
		    	return;
		    }
		}

		template <typename List>
		constexpr void print_TypeList()
		{
		    print_TypeList_Helper<0, List>();
		}
	}  // namespace metautil
}  // namespace meta
}  // namespace ecs
