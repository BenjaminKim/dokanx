#pragma once

#include <Windows.h>

/**
*/
class AutoReleaseLock
{
public:
	AutoReleaseLock(CRITICAL_SECTION* pLock)
	{
		_ASSERT(pLock);
		m_pLock = pLock;
		EnterCriticalSection(m_pLock);
	}
	~AutoReleaseLock()
	{
		if (m_pLock)
		{
			LeaveCriticalSection(m_pLock);
		}
	}
	VOID Release()
	{
		if (m_pLock)
		{
			LeaveCriticalSection(m_pLock);
			m_pLock = NULL;
		}
	}
private:
	CRITICAL_SECTION* m_pLock;
};