#pragma once

#include "Root.h"

#ifndef PRINT_ERRORS
	#define PRINT_ERRORS 0
#endif

#ifndef ERROR_LOG
	#define ERROR_LOG(arg) ((PRINT_ERRORS)==1 ? ( \
		std::cerr << std::endl \
			<< "[ERROR] [THREAD " << std::this_thread::get_id() << "] " \
			<< "[LINE " << __LINE__ << "] " \
			<< __func__ << "(): " \
			<< (arg) << std::endl) \
		: std::cerr<<"" \
		)
#endif

#ifndef PRINT_WARNINGS
	#define PRINT_WARNINGS 0
#endif

#ifndef WARNING_LOG
	#define WARNING_LOG(arg) ((PRINT_WARNINGS)==1 ? ( \
		std::cerr << std::endl \
			<< "[WARNING] [THREAD " << std::this_thread::get_id() << "] " \
			<< "[LINE " << __LINE__ << "] " \
			<< __func__ << "(): " \
			<< (arg) << std::endl) \
		: std::cerr<<"" \
		)
#endif

#ifndef PRINT_DEBUG
	#define PRINT_DEBUG 0
#endif

#ifndef DEBUG_LOG
	#define DEBUG_LOG(arg) ((PRINT_DEBUG==1) ? ( \
		std::cerr << std::endl \
			<< "[DEBUG] " << "[LINE " << __LINE__ << "] " \
			<< __func__ << "(): " \
			<< (arg) << std::endl) \
		: std::cerr<<"" \
		)
#endif

namespace ecs
{
namespace util
{
	std::size_t replace_all(std::string& inout, std::string_view what, std::string_view with);
	std::size_t remove_all(std::string& inout, std::string_view what);
	bool remove_string(std::string& inout, std::string_view what);

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
