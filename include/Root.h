#pragma once

#include <iostream>
#include <iomanip>
#include <exception>
#include <thread>
#include <omp.h>
#include <future>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <algorithm>
#include <numeric>

#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <string>
#include <string_view>
#include <sstream>
#include <queue>
#include <vector>
#include <list>
#include <memory>
#include <optional>
#include <functional>
#include <bitset>

#include <cstdlib>
#include <ctime>
#include <cmath>

//type typedefs for consistency across all ECS files
namespace ecs
{
	using int8		= char;
	using int16 	= short int;
	using int32		= int;
	using int64		= int64_t;
	using uint8		= unsigned char;
	using uint16	= unsigned short int;
	using uint32	= unsigned int;
	using uint64	= uint64_t;
}  // namespace ecs
