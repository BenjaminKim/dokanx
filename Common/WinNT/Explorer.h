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
// Because this function writes to registry at HKLM location, the function should be called on adminitrator context.
//
bool SetVolumeIcon(
	__in wchar_t volumeDesignator,
	__in const std::wstring& canonicalIconPath
	);
bool IsRecycleBin(const std::wstring& path);
void BroadcastDeviceChange(WPARAM message, int nDosDriveNo, DWORD driveMap);