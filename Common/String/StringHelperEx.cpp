#include "StringHelper.h"
#include "StringHelperEx.h"
#include <tchar.h>
#include <math.h>

CString ReplaceBackslashToSlash(__in CONST CString& strPath)
{
	CString s(strPath);
	s.Replace(L"\\", L"/");
	return TrimLastRightPathSeperator((LPCWSTR)s, L'/').c_str();
}

CString ReplaceSlashToBackslash(__in CONST CString& strPath)
{
	CString s(strPath);
	s.Replace(L"/", L"\\");
	return TrimLastRightPathSeperator((LPCWSTR)s, L'\\').c_str();
}

CString GetResString(UINT uStringID, HINSTANCE hResource)
{
	CString resString;
	//resString.LoadString(g_hResInst, uStringID);
	_TCHAR* ch = resString.GetBuffer(512);
	if (hResource)
	{
		::LoadString(hResource, uStringID, ch, 512);
	}
	else
	{
		::LoadString(GetModuleHandle(NULL), uStringID, ch, 512);
	}

	resString.ReleaseBuffer();
	resString.Replace(_T("\\n"), _T("\n"));
	return resString;
}

CString ConvertToHumanReadableSize(INT64 jSize, UINT nDefault)
{
	double size = static_cast<double>(jSize);
	double temp = 0;

	for ( ; ; )
	{
		temp = size / 1024;
		if (temp < 1)
		{
			break;
		}

		++nDefault;		

		size = temp;

		if (nDefault == FILESIZE_MBYTE)
		{
			size = floor(10. * (temp)) / 10.;
		}
		else if ( nDefault == FILESIZE_GBYTE )
		{
			size = floor(100. * (temp)) / 100.;
		}
		else if ( nDefault == FILESIZE_TBYTE )
		{
			size = floor(1000. * (temp)) / 1000.;
		}
		else
		{
			size = temp;
		}
	}

	TCHAR *Byte;
	switch (nDefault)
	{
		case FILESIZE_BYTE:
			Byte = L"Bytes";
			break;
		case FILESIZE_KBYTE:
			Byte = L"KB";
			break;
		case FILESIZE_MBYTE:
			Byte = L"MB";
			break;
		case FILESIZE_GBYTE:
			Byte = L"GB";
			break;
		case FILESIZE_TBYTE:
			Byte = L"TB";
			break;
		default:
			Byte = L"MB";
			break;
	}

	CString rString;
	rString.Format(L"%f", size);

	if (rString.GetLength() > 3)
	{
		rString = rString.Left(4);
		if (rString[3] == L'.')
		{
			rString = rString.Left(3);
		}
	}
	rString += Byte;

	return rString;
}

CString ConvertToHumanReadableSize(CONST CString& strSize, UINT nDefault)
{
	INT64 jSize = _wtoi64(strSize);
	return ConvertToHumanReadableSize(jSize, nDefault);
}