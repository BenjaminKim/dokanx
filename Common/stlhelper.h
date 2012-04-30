#pragma once

#include <map>
#include <functional>
#include <algorithm>

template <typename Map, typename Fn1>
inline void map_erase_if(Map& m, Fn1 pred) // NS
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
*	std::remove_if 와는 함께 사용하면 안됨.
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