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
		//name = name.substr(name.find_last_of(std::string_view{"::"}) + 1);
		if(name.back() == '&')
		{
			name.remove_suffix(1);
		}
		return name;
	}
}  // namespace util
}  // namespace ecs
