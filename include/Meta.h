#pragma once

#include "Root.h"

namespace ecs
{
namespace util
{
	void remove_substr(std::string& base_string, const std::string& key_string)
	{
		auto key_length = key_string.length();

		for(auto str_pos = base_string.find(key_string);
			str_pos != std::string_view::npos;
			str_pos = base_string.find(key_string))
		{
			base_string.erase(str_pos, key_length);
		}
	}

	template <typename T>
	constexpr auto type_name_to_string() noexcept
	{
		std::string_view name = "Error: unsupported compiler", prefix, suffix;
	#ifdef __clang__
		name = __PRETTY_FUNCTION__;
		prefix = "[T = ";
		suffix = "]";
	#elif defined(__GNUC__)
		name = __PRETTY_FUNCTION__;
		prefix = "[with T = ";
		suffix = "]";
	#elif defined(_MSC_VER)
		name = __FUNCSIG__;
		prefix = "type_name_to_string<";
		suffix = ">(void) noexcept";
	#endif

		name.remove_suffix(suffix.size());

		std::string temp{name};
		util::remove_substr(temp, "std::");
		name = temp;

		name = name.substr(name.find(prefix) + prefix.length());
		name = name.substr(name.find_last_of(std::string_view{"::"}) + 1);
		if(name.back() == '&')
		{
			// name = name.substr(0, name.length() - 1);
			name.remove_suffix(1);
		}
		return name;
	}
}  // namespace util

namespace meta
{
	template <typename ...Types>
	struct TypeList
	{
		template <uint64 Index>
  		using TypeOfIndex = typename std::tuple_element<Index, std::tuple<Types ...>>::type;

  		static constexpr uint16 TypeCount = std::tuple_size<std::tuple<Types ...>>::value;

  		template <typename Type>
  		static constexpr uint64 IndexOfType()
  		{
  			return IndexOfTypeHelper<0, Type>();
  		}

  	private:
  		template <uint64 Index, typename Type>
		static constexpr uint64 IndexOfTypeHelper()
		{
		    if constexpr(std::is_same<Type, TypeOfIndex<Index>>::value )
		    {
		        return Index;
		    }
		    else
		    {
		    	if constexpr(Index + 1 < TypeCount)
		    	{
		    		return IndexOfTypeHelper<Index+1, Type>();
		    	}
		        else
		        {
		        	static_assert(Index + 1 < TypeCount, "Given type is not in the TypeList");
		        	return 65;
		        }
		    }
		}
	};

	/*template <typename ...Types>
	using TupleOfVectors = std::tuple<std::vector<Types> ...>;

	template <typename ...Types>
	std::tuple<Types...> TupleFromTypeListHelper(TypeList<Types...>);

	template <typename List>
	using TupleFromTypeList = decltype(TupleFromTypeListHelper(std::declval<List>()));*/

	// template <uint64 index, typename Types>
	// constexpr auto &GetVector(TupleOfVectors &container)
	// {
	// 	return std::tuple_element<decltype(container)
	// }

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

}  // namespace meta

}  // namespace ecs
