
#pragma once

/**
*/
class CConsole
{
public:

	static void EnableConsole(bool bWideChar);

private:
	CConsole();
	explicit CConsole(bool bWideChar);
	~CConsole();
};