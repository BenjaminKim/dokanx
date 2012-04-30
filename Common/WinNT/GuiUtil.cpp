
#include "GuiUtil.h"

void PullWindowToTopWithActive(HWND hWnd)
{
	if(::GetForegroundWindow() != hWnd)
	{
		HWND hActiveWnd = ::GetForegroundWindow();
		if(hActiveWnd != nullptr)
		{
			DWORD dwThId = GetWindowThreadProcessId(hActiveWnd, nullptr);
			DWORD dwCurrentThId = GetCurrentThreadId();
			if(dwCurrentThId != dwThId)
			{
				if(AttachThreadInput(dwCurrentThId, dwThId, TRUE))
				{
					if(BringWindowToTop(hWnd) == FALSE)
					{
						DebugBreak();
					}
					AttachThreadInput(dwCurrentThId, dwThId, FALSE);
				}
			}
		}
	}
}

void PullWindowToTopWithInactive(HWND hWnd)
{
	// 최상위 윈도우로 설정하여 강제로 앞으로 나오게 한다. 
	::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	// 최상위 윈도우 속성을 제거한다. 하지만 윈도우는 다른 윈도우보다 앞에 존재한다. 
	::SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
}
