

#include "Console.h"

#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>

CConsole::CConsole(bool bWideChar)
{
	int nCharMode = (bWideChar) ? _O_WTEXT : _O_TEXT;

	AllocConsole();
	int hCrtOut = _open_osfhandle((LONG)GetStdHandle(STD_OUTPUT_HANDLE), nCharMode);
	FILE *hOut = _fdopen(hCrtOut, "w");
	*stdout = *hOut;
	int hCrtIn = _open_osfhandle((LONG)GetStdHandle(STD_INPUT_HANDLE), nCharMode);
	FILE *hIn = _fdopen(hCrtIn, "r");
	*stdin = *hIn;
	setvbuf(stdin, NULL, _IONBF, 0);

	SetConsoleTitle(L"NDrive's console");
}

CConsole::~CConsole()
{
	_fcloseall();
	FreeConsole();
}

void CConsole::EnableConsole(bool bWideChar)
{
	static CConsole console(bWideChar);
}