#include "PipeClient.h"

#include <windows.h> 
#pragma warning(push)
#pragma warning(disable:4995)
#include <vector>
#pragma warning(pop)

#include "../Log/NdLog.h"

Pipe::Pipe(LPCWSTR lpPipeName): m_PipeName(lpPipeName), m_hPipe(INVALID_HANDLE_VALUE)
{
}

Pipe::~Pipe()
{
	Close();
}

BOOL Pipe::Close()
{
	if (m_hPipe != INVALID_HANDLE_VALUE)
	{
		BOOL fOk = CloseHandle(m_hPipe);
		m_hPipe = INVALID_HANDLE_VALUE;
		return fOk;
	}

	return TRUE;
}

BOOL Pipe::Connect()
{
	if (m_PipeName.empty())
	{
		return FALSE;
	}

	return Connect(m_PipeName.c_str(), false);
}

BOOL Pipe::Connect(BOOL fAsync)
{
	if (m_PipeName.empty())
	{
		return FALSE;
	}

	return Connect(m_PipeName.c_str(), fAsync);
}

BOOL Pipe::Connect(LPCWSTR lpszPipename, BOOL fCreateAsyncPipe)
{
	BOOL fSuccess; 
	DWORD dwMode;

	// Try to open a named pipe; wait for it, if necessary. 

	for ( ; ; )
	{
		m_hPipe = CreateFile( 
			lpszPipename,   // pipe name 
			GENERIC_READ |  // read and write access 
			GENERIC_WRITE, 
			0,              // no sharing
			NULL,           // default security attributes
			OPEN_EXISTING,  // opens existing pipe 
			fCreateAsyncPipe ? FILE_FLAG_OVERLAPPED : 0,              // default attributes 
			NULL);          // no template file 

		// Break if the pipe handle is valid.
		if (m_hPipe != INVALID_HANDLE_VALUE) 
		{
			logw(L"The pipe created.");
			break;
		}

		// Exit if an error other than ERROR_PIPE_BUSY occurs. 

		if (GetLastError() != ERROR_PIPE_BUSY) 
		{
			logw(L"Could not open pipe(%d)", GetLastError()); 
			return FALSE;
		}

		// All pipe instances are busy, so wait for 20 seconds. 

		if (!WaitNamedPipe(lpszPipename, 20000)) 
		{ 
			logw(L"Could not open pipe(%d)", GetLastError()); 
			return FALSE;
		}
	}

	// The pipe connected; change to message-read mode. 

	dwMode = PIPE_READMODE_MESSAGE; 
	fSuccess = SetNamedPipeHandleState( 
		m_hPipe,    // pipe handle 
		&dwMode,  // new pipe mode 
		NULL,     // don't set maximum bytes 
		NULL);    // don't set maximum time 
	if (!fSuccess) 
	{
		logw(L"SetNamedPipeHandleState failed(%d)", GetLastError());
		return FALSE;
	}

	return TRUE;
}

BOOL Pipe::Connect(const std::wstring& sPipename)
{
	return Connect(sPipename.c_str(), false);
}

BOOL Pipe::Send(PBYTE buf, DWORD cbBuffer)
{
	DWORD sentTotalBytes = 0;
	while (sentTotalBytes < cbBuffer)
	{
		int remainSize = cbBuffer - sentTotalBytes;
		DWORD cbWritten;
		BOOL fOk = WriteFile(m_hPipe, buf + sentTotalBytes, remainSize, &cbWritten, nullptr);
		if (!fOk || cbWritten == 0)
		{
			break;
		}
		sentTotalBytes += cbWritten;
	}

	return (cbBuffer == sentTotalBytes);
}

BOOL Pipe::Send(PBYTE buf, DWORD maxlen, DWORD& cbWritten, LPOVERLAPPED lpOverlapped)
{
	return WriteFile( 
		m_hPipe,                  // pipe handle 
		buf,             // message 
		maxlen, // message length 
		&cbWritten,             // bytes written 
		lpOverlapped);                  // not overlapped
}

BOOL Pipe::Recv(PBYTE buf, DWORD maxlen, DWORD& cbRead)
{
	return ReadFile( 
		m_hPipe,    // pipe handle 
		buf,
		maxlen,  // size of buffer 
		&cbRead,  // number of bytes read 
		NULL);    // not overlapped
}

BOOL Pipe::Recv(PBYTE buf, DWORD maxlen, DWORD& cbRead, LPOVERLAPPED lpOverlapped)
{
	return ReadFile( 
		m_hPipe,    // pipe handle 
		buf,
		maxlen,  // size of buffer 
		&cbRead,  // number of bytes read 
		lpOverlapped);    // not overlapped
}