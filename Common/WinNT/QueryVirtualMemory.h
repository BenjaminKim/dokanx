#pragma once
#include <Windows.h>
#include "../Log/NdLog.h"
#include "../String/StringHelper.h"
#include <strsafe.h>

BOOL AvailablePhysicsMemory(__out UINT64& availablePhysicsMemory);

//
// 메모리가 정말 많다면 값이 짤릴수 있지만 함수를 편하게 쓰기 위해 무시한다.
// 값이 짤리는 것이 싫으면 AvailablePhysicsMemory 함수를 사용할 것.
//
INT64 GetAvailableMemory();

VOID PrintAvailableMemory();