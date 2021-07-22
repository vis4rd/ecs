#include "../include/Util.h"

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

}  // namespace util
}  // namespace ecs
