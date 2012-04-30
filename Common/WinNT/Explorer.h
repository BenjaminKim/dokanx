#pragma once
#include <Windows.h>
#pragma warning(push)
#pragma warning(disable:4995)
#include <string>
#pragma warning(pop)
#include <ShlObj.h>
#include <Dbt.h>


bool OpenWindowsExplorer(
	__in const std::wstring& filePath,
	__in bool withSelectingFileItem
	);
bool UpdateFileFolderIcon(
	__in const std::wstring& filePath
	);
std::wstring GetSpecialFolderPath(
	__in int csidl
	);
bool ProcessNameMatched(
	__in const std::wstring& imageFileName,
	__in DWORD pid
	);
std::wstring ProcessImageName(
	__in DWORD pid
	);
bool IsExplorerProcess(
	__in DWORD pid
	);
bool IsNdriveProcess(
	__in DWORD pid
	);
bool ShellDeleteFile(
	__in const std::wstring& sFilePath
	);

//
// HKML 위치의 레지스트리를 건드리므로 호출하는 컨텍스트에서 관리자 토큰이 필요하다.
//
bool SetVolumeIcon(
	__in wchar_t volumeDesignator,
	__in const std::wstring& canonicalIconPath
	);
bool IsRecycleBin(const std::wstring& path);
void MarkShortcutRunAs(const std::wstring& sShortcut);
void BroadcastDeviceChange(WPARAM message, int nDosDriveNo, DWORD driveMap);