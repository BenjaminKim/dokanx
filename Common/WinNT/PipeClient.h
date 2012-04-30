#pragma once

#include <windows.h> 
#include <tchar.h>
#pragma warning(push)
#pragma warning(disable:4995)
#include <string>
#pragma warning(pop)

/**
*/
class Pipe
{
public:
	Pipe(LPCWSTR lpPipeName);
	~Pipe();
	BOOL Close();
	BOOL Connect();
	BOOL Connect(BOOL fAsync);
	BOOL Connect(LPCWSTR lpszPipename, BOOL fCreateAsyncPipe);
	BOOL Connect(const std::wstring& sPipename);
	BOOL Send(PBYTE buf, DWORD cbBuffer);
	BOOL Send(PBYTE buf, DWORD maxlen, DWORD& cbWritten, LPOVERLAPPED pOverlapped);
	BOOL Recv(PBYTE buf, DWORD maxlen, DWORD& cbRead);
	BOOL Recv(PBYTE buf, DWORD maxlen, DWORD& cbRead, LPOVERLAPPED pOverlapped);    

private:
	HANDLE m_hPipe;
	std::wstring m_PipeName;
};