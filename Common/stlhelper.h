#pragma once

#include <map>
#include <functional>
#include <algorithm>

template <typename Map, typename Fn1>
inline void map_erase_if(Map& m, Fn1 pred)
{
	typename Map::iterator it = m.begin();
	while ((it = std::find_if(it, m.end(), pred)) != m.end())
	{
		m.erase(it++);
	}
}

/**
* A unary functor for accumulation.
* NOTE:
*	Don't use this macro with std::remove_if
*/
struct Accumulator
	: std::unary_function<std::pair<std::wstring, int>, void>
{
	Accumulator()
		: m_totalCount(0)
	{
	}

	void operator()(const std::pair<std::wstring, int>& p)
	{
		m_totalCount += p.second;
	}

	int Result() const
	{
		return m_totalCount;
	}

	int m_totalCount; 
};