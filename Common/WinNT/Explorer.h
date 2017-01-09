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
// HKML ��ġ�� ������Ʈ���� �ǵ帮�Ƿ� ȣ���ϴ� ���ؽ�Ʈ���� ������ ��ū�� �ʿ��ϴ�.
//
bool SetVolumeIcon(
	__in wchar_t volumeDesignator,
	__in const std::wstring& canonicalIconPath
	);
bool IsRecycleBin(const std::wstring& path);
void MarkShortcutRunAs(const std::wstring& sShortcut);
void BroadcastDeviceChange(WPARAM message, int nDosDriveNo, DWORD driveMap);
void InformWindowsOfDriveChange(LPCWSTR MountPoint,bool bAddNotRemove);