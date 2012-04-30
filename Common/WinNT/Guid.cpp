#include "Guid.h"

#include <Guiddef.h>
#include <ObjBase.h>

std::wstring GenerateGUID()
{
	GUID guid;
	::CoCreateGuid(&guid);
	WCHAR strGuid[40] = {0};
	StringFromGUID2(guid,  strGuid, _countof(strGuid));

	return std::wstring(strGuid);
}