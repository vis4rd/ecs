#pragma once

#include "Root.h"

namespace ecs
{
namespace util
{
	// Source: https://en.cppreference.com/w/cpp/string/basic_string/replace
	std::size_t replace_all(std::string& inout, std::string_view what, std::string_view with)
	{
		std::size_t count{};
		for(std::string::size_type pos{};
			inout.npos != (pos = inout.find(what.data(), pos, what.length()));
			pos += with.length(), ++count)
		{
			inout.replace(pos, what.length(), with.data(), with.length());
		}
		return count;
	}
	 
	std::size_t remove_all(std::string& inout, std::string_view what)
	{
		return replace_all(inout, what, "");
	}
	//
	bool remove_string(std::string& inout, std::string_view what)
	{
		std::string::size_type pos{};
		if(inout.npos != (pos = inout.find(what.data(), pos, what.length())))
		{
			inout.replace(pos, what.length(), "", 0);
			return true;
		}
		else
		{
			return false;
		}
	}

	template <typename T>
	constexpr auto type_name_to_string() noexcept
	{
		std::string name = "Error: unsupported compiler", prefix, suffix;
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
		// name.substr(0, name.length() - suffix.length());

		remove_string(name, prefix);
		remove_string(name, "constexpr auto ecs::util::type_name_to_string() ");
		remove_string(name, suffix);
		remove_all(name, "std::");
		remove_all(name, "__cxx11::");
		if(name.back() == '&')
		{
			name.pop_back();
		}
		return name;
	}
}  // namespace util
}  // namespace ecs
