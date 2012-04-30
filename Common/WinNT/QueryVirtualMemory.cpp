#include "QueryVirtualMemory.h"

BOOL AvailablePhysicsMemory(__out UINT64& availablePhysicsMemory)
{
	MEMORYSTATUSEX m;
	m.dwLength = sizeof(m);
	BOOL fOk = GlobalMemoryStatusEx(&m);

	if (!fOk)
	{
		availablePhysicsMemory = 0;
		return FALSE;
	}

	availablePhysicsMemory = m.ullAvailPhys;
	return TRUE;
}

INT64 GetAvailableMemory()
{
	UINT64 i;
	BOOL fOk = AvailablePhysicsMemory(i);
	return fOk ? (INT64)i : -1;
}

VOID PrintAvailableMemory()
{
	WCHAR sz[MAX_PATH] = { 0 };
	_i64tow_s(GetAvailableMemory(), sz, _countof(sz), 10);
	std::wstring s = AddComma(sz);
	logw(L"AvailableMemory: %s", s.c_str());
}