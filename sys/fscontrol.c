/*
  Dokan : user-mode file system library for Windows

  Copyright (C) 2008 Hiroki Asakawa info@dokan-dev.net

  http://dokan-dev.net/en

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "precomp.h"
#pragma hdrstop

#include "dokan.h"


NTSTATUS
DokanUserFsRequest(
	__in PDEVICE_OBJECT	DeviceObject,
	__in PIRP			Irp
	)
{
	NTSTATUS			status = STATUS_NOT_IMPLEMENTED;
	PIO_STACK_LOCATION	irpSp;

	irpSp = IoGetCurrentIrpStackLocation(Irp);

	switch(irpSp->Parameters.FileSystemControl.FsControlCode) {

	case FSCTL_REQUEST_OPLOCK_LEVEL_1:
		DDbgPrint("    FSCTL_REQUEST_OPLOCK_LEVEL_1");
		status = STATUS_SUCCESS;
		break;

	case FSCTL_REQUEST_OPLOCK_LEVEL_2:
		DDbgPrint("    FSCTL_REQUEST_OPLOCK_LEVEL_2");
		status = STATUS_SUCCESS;
		break;

	case FSCTL_REQUEST_BATCH_OPLOCK:
		DDbgPrint("    FSCTL_REQUEST_BATCH_OPLOCK");
		status = STATUS_SUCCESS;
		break;

	case FSCTL_OPLOCK_BREAK_ACKNOWLEDGE:
		DDbgPrint("    FSCTL_OPLOCK_BREAK_ACKNOWLEDGE");
		status = STATUS_SUCCESS;
		break;

	case FSCTL_OPBATCH_ACK_CLOSE_PENDING:
		DDbgPrint("    FSCTL_OPBATCH_ACK_CLOSE_PENDING");
		status = STATUS_SUCCESS;
		break;

	case FSCTL_OPLOCK_BREAK_NOTIFY:
		DDbgPrint("    FSCTL_OPLOCK_BREAK_NOTIFY");
		status = STATUS_SUCCESS;
		break;

	case FSCTL_LOCK_VOLUME:
		DDbgPrint("    FSCTL_LOCK_VOLUME");
		status = STATUS_SUCCESS;
		break;

	case FSCTL_UNLOCK_VOLUME:
		DDbgPrint("    FSCTL_UNLOCK_VOLUME");
		status = STATUS_SUCCESS;
		break;

	case FSCTL_DISMOUNT_VOLUME:
		DDbgPrint("    FSCTL_DISMOUNT_VOLUME");
		break;

	case FSCTL_IS_VOLUME_MOUNTED:
		DDbgPrint("    FSCTL_IS_VOLUME_MOUNTED");
		status = STATUS_SUCCESS;
		break;

	case FSCTL_IS_PATHNAME_VALID:
		DDbgPrint("    FSCTL_IS_PATHNAME_VALID");
		break;

	case FSCTL_MARK_VOLUME_DIRTY:
		DDbgPrint("    FSCTL_MARK_VOLUME_DIRTY");
		break;

	case FSCTL_QUERY_RETRIEVAL_POINTERS:
		DDbgPrint("    FSCTL_QUERY_RETRIEVAL_POINTERS");
		break;

	case FSCTL_GET_COMPRESSION:
		DDbgPrint("    FSCTL_GET_COMPRESSION");
		break;

	case FSCTL_SET_COMPRESSION:
		DDbgPrint("    FSCTL_SET_COMPRESSION");
		break;

	case FSCTL_MARK_AS_SYSTEM_HIVE:
		DDbgPrint("    FSCTL_MARK_AS_SYSTEM_HIVE");
		break;

	case FSCTL_OPLOCK_BREAK_ACK_NO_2:
		DDbgPrint("    FSCTL_OPLOCK_BREAK_ACK_NO_2");
		break;

	case FSCTL_INVALIDATE_VOLUMES:
		DDbgPrint("    FSCTL_INVALIDATE_VOLUMES");
		break;

	case FSCTL_QUERY_FAT_BPB:
		DDbgPrint("    FSCTL_QUERY_FAT_BPB");
		break;

	case FSCTL_REQUEST_FILTER_OPLOCK:
		DDbgPrint("    FSCTL_REQUEST_FILTER_OPLOCK");
		break;

	case FSCTL_FILESYSTEM_GET_STATISTICS:
		DDbgPrint("    FSCTL_FILESYSTEM_GET_STATISTICS");
		break;

	case FSCTL_GET_NTFS_VOLUME_DATA:
		DDbgPrint("    FSCTL_GET_NTFS_VOLUME_DATA");
		break;

	case FSCTL_GET_NTFS_FILE_RECORD:
		DDbgPrint("    FSCTL_GET_NTFS_FILE_RECORD");
		break;

	case FSCTL_GET_VOLUME_BITMAP:
		DDbgPrint("    FSCTL_GET_VOLUME_BITMAP");
		break;

	case FSCTL_GET_RETRIEVAL_POINTERS:
		DDbgPrint("    FSCTL_GET_RETRIEVAL_POINTERS");
		break;

	case FSCTL_MOVE_FILE:
		DDbgPrint("    FSCTL_MOVE_FILE");
		break;

	case FSCTL_IS_VOLUME_DIRTY:
		DDbgPrint("    FSCTL_IS_VOLUME_DIRTY");
		break;

	case FSCTL_ALLOW_EXTENDED_DASD_IO:
		DDbgPrint("    FSCTL_ALLOW_EXTENDED_DASD_IO");
		break;

	case FSCTL_FIND_FILES_BY_SID:
		DDbgPrint("    FSCTL_FIND_FILES_BY_SID");
		break;

	case FSCTL_SET_OBJECT_ID:
		DDbgPrint("    FSCTL_SET_OBJECT_ID");
		break;

	case FSCTL_GET_OBJECT_ID:
		DDbgPrint("    FSCTL_GET_OBJECT_ID");
		break;

	case FSCTL_DELETE_OBJECT_ID:
		DDbgPrint("    FSCTL_DELETE_OBJECT_ID");
		break;

	case FSCTL_SET_REPARSE_POINT:
		DDbgPrint("    FSCTL_SET_REPARSE_POINT");
		break;

	case FSCTL_GET_REPARSE_POINT:
		DDbgPrint("    FSCTL_GET_REPARSE_POINT");
		status = STATUS_NOT_A_REPARSE_POINT;
		break;

	case FSCTL_DELETE_REPARSE_POINT:
		DDbgPrint("    FSCTL_DELETE_REPARSE_POINT");
		break;

	case FSCTL_ENUM_USN_DATA:
		DDbgPrint("    FSCTL_ENUM_USN_DATA");
		break;

	case FSCTL_SECURITY_ID_CHECK:
		DDbgPrint("    FSCTL_SECURITY_ID_CHECK");
		break;

	case FSCTL_READ_USN_JOURNAL:
		DDbgPrint("    FSCTL_READ_USN_JOURNAL");
		break;

	case FSCTL_SET_OBJECT_ID_EXTENDED:
		DDbgPrint("    FSCTL_SET_OBJECT_ID_EXTENDED");
		break;

	case FSCTL_CREATE_OR_GET_OBJECT_ID:
		DDbgPrint("    FSCTL_CREATE_OR_GET_OBJECT_ID");
		break;

	case FSCTL_SET_SPARSE:
		DDbgPrint("    FSCTL_SET_SPARSE");
		break;

	case FSCTL_SET_ZERO_DATA:
		DDbgPrint("    FSCTL_SET_ZERO_DATA");
		break;

	case FSCTL_QUERY_ALLOCATED_RANGES:
		DDbgPrint("    FSCTL_QUERY_ALLOCATED_RANGES");
		break;

	case FSCTL_SET_ENCRYPTION:
		DDbgPrint("    FSCTL_SET_ENCRYPTION");
		break;

	case FSCTL_ENCRYPTION_FSCTL_IO:
		DDbgPrint("    FSCTL_ENCRYPTION_FSCTL_IO");
		break;

	case FSCTL_WRITE_RAW_ENCRYPTED:
		DDbgPrint("    FSCTL_WRITE_RAW_ENCRYPTED");
		break;

	case FSCTL_READ_RAW_ENCRYPTED:
		DDbgPrint("    FSCTL_READ_RAW_ENCRYPTED");
		break;

	case FSCTL_CREATE_USN_JOURNAL:
		DDbgPrint("    FSCTL_CREATE_USN_JOURNAL");
		break;

	case FSCTL_READ_FILE_USN_DATA:
		DDbgPrint("    FSCTL_READ_FILE_USN_DATA");
		break;

	case FSCTL_WRITE_USN_CLOSE_RECORD:
		DDbgPrint("    FSCTL_WRITE_USN_CLOSE_RECORD");
		break;

	case FSCTL_EXTEND_VOLUME:
		DDbgPrint("    FSCTL_EXTEND_VOLUME");
		break;

	case FSCTL_QUERY_USN_JOURNAL:
		DDbgPrint("    FSCTL_QUERY_USN_JOURNAL");
		break;

	case FSCTL_DELETE_USN_JOURNAL:
		DDbgPrint("    FSCTL_DELETE_USN_JOURNAL");
		break;

	case FSCTL_MARK_HANDLE:
		DDbgPrint("    FSCTL_MARK_HANDLE");
		break;

	case FSCTL_SIS_COPYFILE:
		DDbgPrint("    FSCTL_SIS_COPYFILE");
		break;

	case FSCTL_SIS_LINK_FILES:
		DDbgPrint("    FSCTL_SIS_LINK_FILES");
		break;

	case FSCTL_RECALL_FILE:
		DDbgPrint("    FSCTL_RECALL_FILE");
		break;

	default:
		DDbgPrint("    Unknown FSCTL %d\n",
			(irpSp->Parameters.FileSystemControl.FsControlCode >> 2) & 0xFFF);
		status = STATUS_INVALID_DEVICE_REQUEST;
	}

	return status;
}



NTSTATUS
DokanDispatchFileSystemControl(
	__in PDEVICE_OBJECT DeviceObject,
	__in PIRP Irp
	)
{
	NTSTATUS			status = STATUS_INVALID_PARAMETER;
	PIO_STACK_LOCATION	irpSp;
	PDokanVCB			vcb;

	//PAGED_CODE();

	__try {
		FsRtlEnterFileSystem();

		DDbgPrint("==> DokanFileSystemControl");
		DDbgPrint("  ProcessId %lu\n", IoGetRequestorProcessId(Irp));

		vcb = DeviceObject->DeviceExtension;
		if (GetIdentifierType(vcb) != VCB) {
			status = STATUS_INVALID_PARAMETER;
			__leave;
		}

		irpSp = IoGetCurrentIrpStackLocation(Irp);

		switch(irpSp->MinorFunction) {
		case IRP_MN_KERNEL_CALL:
			DDbgPrint("	 IRP_MN_KERNEL_CALL");
			break;

		case IRP_MN_LOAD_FILE_SYSTEM:
			DDbgPrint("	 IRP_MN_LOAD_FILE_SYSTEM");
			break;

		case IRP_MN_MOUNT_VOLUME:
			{
				PVPB vpb;
				DDbgPrint("	 IRP_MN_MOUNT_VOLUME");
				if (irpSp->Parameters.MountVolume.DeviceObject != vcb->Dcb->DeviceObject) {
					DDbgPrint("   Not DokanDiskDevice");
					status = STATUS_INVALID_PARAMETER;
				}
				vpb = irpSp->Parameters.MountVolume.Vpb;
				vpb->DeviceObject = vcb->DeviceObject;
				vpb->RealDevice = vcb->DeviceObject;
				vpb->Flags |= VPB_MOUNTED;
				vpb->VolumeLabelLength = wcslen(VOLUME_LABEL) * sizeof(WCHAR);
				RtlStringCchCopyW(vpb->VolumeLabel, sizeof(vpb->VolumeLabel) / sizeof(WCHAR), VOLUME_LABEL);
				vpb->SerialNumber = 0x19831116;
				status = STATUS_SUCCESS;
			}
			break;

		case IRP_MN_USER_FS_REQUEST:
			DDbgPrint("	 IRP_MN_USER_FS_REQUEST");
			status = DokanUserFsRequest(DeviceObject, Irp);
			break;

		case IRP_MN_VERIFY_VOLUME:
			DDbgPrint("	 IRP_MN_VERIFY_VOLUME");
			break;

		default:
			DDbgPrint("  unknown %d\n", irpSp->MinorFunction);
			status = STATUS_INVALID_PARAMETER;
			break;

		}

	} __finally {
		
		Irp->IoStatus.Status = status;
		Irp->IoStatus.Information = 0;
		IoCompleteRequest(Irp, IO_NO_INCREMENT);

		DokanPrintNTStatus(status);
		DDbgPrint("<== DokanFileSystemControl");

		FsRtlExitFileSystem();
	}

	return status;
}

