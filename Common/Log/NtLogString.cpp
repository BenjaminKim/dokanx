#include <Windows.h>

#include "NtLogString.h"
#include "../../dokanx/fileinfo.h"

#define GET_NT_STATUS_STRING_SWITCH()\
	switch (status)\
	{\
	CASE_RETURN(STATUS_SUCCESS);\
	CASE_RETURN(STATUS_ABANDONED);\
	CASE_RETURN(STATUS_MORE_PROCESSING_REQUIRED);\
	CASE_RETURN(STATUS_DATA_NOT_ACCEPTED);\
	CASE_RETURN(STATUS_INVALID_DEVICE_REQUEST);\
	CASE_RETURN(STATUS_BAD_NETWORK_NAME);\
	CASE_RETURN(STATUS_BAD_NETWORK_PATH);\
	CASE_RETURN(STATUS_INSUFFICIENT_RESOURCES);\
	CASE_RETURN(STATUS_PENDING);\
	CASE_RETURN(STATUS_INVALID_PARAMETER);\
	CASE_RETURN(STATUS_NO_MORE_FILES);\
	CASE_RETURN(STATUS_NO_SUCH_FILE);\
	CASE_RETURN(STATUS_NOT_IMPLEMENTED);\
	CASE_RETURN(STATUS_OBJECT_NAME_NOT_FOUND);\
	CASE_RETURN(STATUS_OBJECT_PATH_NOT_FOUND);\
	CASE_RETURN(STATUS_NETWORK_ACCESS_DENIED);\
	CASE_RETURN(STATUS_BUFFER_TOO_SMALL);\
	CASE_RETURN(STATUS_WMI_GUID_NOT_FOUND);\
	CASE_RETURN(STATUS_QUOTA_EXCEEDED);\
	CASE_RETURN(STATUS_DISK_FULL);\
	CASE_RETURN(STATUS_NOT_SUPPORTED);\
	CASE_RETURN(STATUS_FILE_IS_A_DIRECTORY);\
	CASE_RETURN(STATUS_FILES_OPEN);\
	CASE_RETURN(STATUS_CANCELLED);\
	CASE_RETURN(STATUS_CONNECTION_IN_USE);\
	CASE_RETURN(STATUS_LOCK_NOT_GRANTED);\
	CASE_RETURN(STATUS_NOT_FOUND);\
	CASE_RETURN(STATUS_ACCESS_DENIED);\
	CASE_RETURN(STATUS_OBJECT_NAME_COLLISION);\
	CASE_RETURN(STATUS_OBJECT_NAME_INVALID);\
	CASE_RETURN(STATUS_OBJECT_TYPE_MISMATCH);\
	CASE_RETURN(STATUS_END_OF_FILE);\
	CASE_RETURN(STATUS_DELETE_PENDING);\
	CASE_RETURN(STATUS_REDIRECTOR_HAS_OPEN_HANDLES);\
	CASE_RETURN(STATUS_REDIRECTOR_PAUSED);\
	CASE_RETURN(STATUS_REDIRECTOR_NOT_STARTED);\
	CASE_RETURN(STATUS_REDIRECTOR_STARTED);\
	CASE_RETURN(STATUS_NOT_A_DIRECTORY);\
	CASE_RETURN(STATUS_RETRY);\
	CASE_RETURN(STATUS_BUFFER_OVERFLOW);\
	CASE_RETURN(STATUS_SHARING_VIOLATION);\
	}

LPCWSTR GetNtStatusString(NTSTATUS status)
{
#define CASE_RETURN(code) case code: return L#code

	GET_NT_STATUS_STRING_SWITCH();

#undef CASE_RETURN

	return L"Unknown";
}


#define FILE_INFORMATION_CLASS_STRING_SWITCH()\
	switch (clazz)\
	{\
	CASE_RETURN(FileDirectoryInformation);\
	CASE_RETURN(FileFullDirectoryInformation);\
	CASE_RETURN(FileBothDirectoryInformation);\
	CASE_RETURN(FileBasicInformation);\
	CASE_RETURN(FileStandardInformation);\
	CASE_RETURN(FileInternalInformation);\
	CASE_RETURN(FileEaInformation);\
	CASE_RETURN(FileAccessInformation);\
	CASE_RETURN(FileNameInformation);\
	CASE_RETURN(FileRenameInformation);\
	CASE_RETURN(FileLinkInformation);\
	CASE_RETURN(FileNamesInformation);\
	CASE_RETURN(FileDispositionInformation);\
	CASE_RETURN(FilePositionInformation);\
	CASE_RETURN(FileFullEaInformation);\
	CASE_RETURN(FileModeInformation);\
	CASE_RETURN(FileAlignmentInformation);\
	CASE_RETURN(FileAllInformation);\
	CASE_RETURN(FileAllocationInformation);\
	CASE_RETURN(FileEndOfFileInformation);\
	CASE_RETURN(FileAlternateNameInformation);\
	CASE_RETURN(FileStreamInformation);\
	CASE_RETURN(FilePipeInformation);\
	CASE_RETURN(FilePipeLocalInformation);\
	CASE_RETURN(FilePipeRemoteInformation);\
	CASE_RETURN(FileMailslotQueryInformation);\
	CASE_RETURN(FileMailslotSetInformation);\
	CASE_RETURN(FileCompressionInformation);\
	CASE_RETURN(FileObjectIdInformation);\
	CASE_RETURN(FileCompletionInformation);\
	CASE_RETURN(FileMoveClusterInformation);\
	CASE_RETURN(FileQuotaInformation);\
	CASE_RETURN(FileReparsePointInformation);\
	CASE_RETURN(FileNetworkOpenInformation);\
	CASE_RETURN(FileAttributeTagInformation);\
	CASE_RETURN(FileTrackingInformation);\
	CASE_RETURN(FileIdBothDirectoryInformation);\
	CASE_RETURN(FileIdFullDirectoryInformation);\
	CASE_RETURN(FileValidDataLengthInformation);\
	CASE_RETURN(FileShortNameInformation);\
	CASE_RETURN(FileIoCompletionNotificationInformation);\
	CASE_RETURN(FileIoStatusBlockRangeInformation);\
	CASE_RETURN(FileIoPriorityHintInformation);\
	CASE_RETURN(FileSfioReserveInformation);\
	CASE_RETURN(FileSfioVolumeInformation);\
	CASE_RETURN(FileHardLinkInformation);\
	CASE_RETURN(FileProcessIdsUsingFileInformation);\
	CASE_RETURN(FileNormalizedNameInformation);\
	CASE_RETURN(FileNetworkPhysicalNameInformation);\
	CASE_RETURN(FileMaximumInformation);\
	}

LPCWSTR FileInformationClassString(INT clazz)
{
#define CASE_RETURN(clazz) case clazz: return L#clazz

	FILE_INFORMATION_CLASS_STRING_SWITCH();

#undef CASE_RETURN

	return L"Unknown";
}

LPCWSTR GetMajorFunctionString(UCHAR majorFunction)
{
	switch (majorFunction)
	{
		case 0x00:
			return L"IRP_MJ_CREATE";
		/*case 0x01:
			return L"IRP_MJ_CREATE_NAMED_PIPE";*/
		case 0x02:
			return L"IRP_MJ_CLOSE";
		case 0x03:
			return L"IRP_MJ_READ";
		case 0x04:
			return L"IRP_MJ_WRITE";
		case 0x05:
			return L"IRP_MJ_QUERY_INFORMATION";
		case 0x06:
			return L"IRP_MJ_SET_INFORMATION";
		/*case 0x07:
			return L"IRP_MJ_QUERY_EA";
		case 0x08:
			return L"IRP_MJ_SET_EA";*/
		case 0x09:
			return L"IRP_MJ_FLUSH_BUFFERS";
		case 0x0a:
			return L"IRP_MJ_QUERY_VOLUME_INFORMATION";
		case 0x0b:
			return L"IRP_MJ_SET_VOLUME_INFORMATION";
		case 0x0c:
			return L"IRP_MJ_DIRECTORY_CONTROL";
		case 0x0d:
			return L"IRP_MJ_FILE_SYSTEM_CONTROL";
		case 0x0e:
			return L"IRP_MJ_DEVICE_CONTROL";
		/*case 0x0f:
			return L"IRP_MJ_INTERNAL_DEVICE_CONTROL";*/
		case 0x10:
			return L"IRP_MJ_SHUTDOWN";
		case 0x11:
			return L"IRP_MJ_LOCK_CONTROL";
		case 0x12:
			return L"IRP_MJ_CLEANUP";
		/*case 0x13:
			return L"IRP_MJ_CREATE_MAILSLOT";*/
		case 0x14:
			return L"IRP_MJ_QUERY_SECURITY";
		case 0x15:
			return L"IRP_MJ_SET_SECURITY";
		/*case 0x16:
			return L"IRP_MJ_POWER";*/
		/*case 0x17:
			return L"IRP_MJ_SYSTEM_CONTROL";*/
		/*case 0x18:
			return L"IRP_MJ_DEVICE_CHANGE";*/
		case 0x19:
			return L"IRP_MJ_QUERY_QUOTA";
		case 0x1a:
			return L"IRP_MJ_SET_QUOTA";
		/*case 0x1b:
			return L"IRP_MJ_PNP";*/
		default:
			return L"Unknown Major Function";
	}
}

LPCWSTR Win32CreateDispositionStr(DWORD dwDisposition)
{
	switch (dwDisposition)
	{
		case CREATE_NEW:
			return L"CREATE_NEW";
		case CREATE_ALWAYS:
			return L"CREATE_ALWAYS";
		case OPEN_EXISTING:
			return L"OPEN_EXISTING";
		case OPEN_ALWAYS:
			return L"OPEN_ALWAYS";
		case TRUNCATE_EXISTING:
			return L"TRUNCATE_EXISTING";
		default:
			return L"Unknown disposition";
	}
}

LPCWSTR GetNtCreateDispositionStr(DWORD dwDisposition)
{
	switch (dwDisposition)
	{
		case 0x00000000:
			return L"FILE_SUPERSEDE";
		case 0x00000001:
			return L"FILE_OPEN";
		case 0x00000002:
			return L"FILE_CREATE";
		case 0x00000003:
			return L"FILE_OPEN_IF";
		case 0x00000004:
			return L"FILE_OVERWRITE";
		case 0x00000005:
			return L"FILE_OVERWRITE_IF";
		default:
			return L"Unknown disposition";
	}
}