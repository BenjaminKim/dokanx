#pragma once

#pragma warning(push)
#pragma warning(disable:4995)
#include <vector>
#pragma warning(pop)
#include <Windows.h>

int GetFirstAvailableDrive();
int GetLastAvailableDrive();
std::vector<WCHAR> GetAvailableVolumes();
bool IsDriveAvailable(int driveNo);
bool IsVolumeDesignatorAvailable(WCHAR chVolumeDesignator);
bool IsDeviceMounted(WCHAR* deviceName);
bool GetDriveLabel(int driveNo, wchar_t *label, int labelSize);
BOOL FormatNtfs(LPCWSTR pszVolume, int clusterSize, __out_opt PUSHORT pProgressCount);