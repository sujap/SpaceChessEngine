#pragma once

#include <string>
#include <map>

namespace space {

	void space_assert(bool expr, std::string message);

	template<typename KeyType, typename ValueType>
	KeyType get_best(const std::map<KeyType, ValueType>& x, bool high = true);

	double urand();


}