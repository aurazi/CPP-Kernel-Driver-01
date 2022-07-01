#include <ntddk.h>

UNICODE_STRING Copied_RegistryPath;

void DriverUnload(_In_ PDRIVER_OBJECT DriverObject);

extern "C"
NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) {
	UNREFERENCED_PARAMETER(RegistryPath);

	/* Set ptr to unload procedure */
	DriverObject->DriverUnload = DriverUnload;

	/* Set UNICODE_STRING members */
	/* Allocate a NON_PAGED pool (guaranteed to stay in RAM) */
	Copied_RegistryPath.Buffer = reinterpret_cast<WCHAR*>(ExAllocatePool2(POOL_FLAG_NON_PAGED, RegistryPath->Length, 1));
	if (!Copied_RegistryPath.Buffer) {
		KdPrint(("Failed to allocate memory pool for UNICODE_STRING"));
		return STATUS_FAILED_DRIVER_ENTRY;
	}
	Copied_RegistryPath.Length = RegistryPath->Length;
	Copied_RegistryPath.MaximumLength = RegistryPath->Length;
	/* Copy contents of RegistryPath buffer to Copiedd_RegistryPath */
	RtlCopyUnicodeString(&Copied_RegistryPath, RegistryPath);

	OSVERSIONINFOW OSInfo;
	/* Set size of structure to sizeof(OSVERSIONINFOW) bytes */
	OSInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);

	NTSTATUS RtlGetVersionStatus = RtlGetVersion(&OSInfo);
	if (!NT_SUCCESS(RtlGetVersionStatus)) {
		/* Deallocate allocated memory before exiting */
		ExFreePool(Copied_RegistryPath.Buffer);
		
		KdPrint(("Failed to fetch OS info.\nError Code: 0x%08X\n", RtlGetVersionStatus));

		return STATUS_FAILED_DRIVER_ENTRY;
	}
	KdPrint(("Build Number: %u\n", OSInfo.dwBuildNumber));
	KdPrint(("Major Version: %u\n", OSInfo.dwMajorVersion));
	KdPrint(("Minor Version: %u\n", OSInfo.dwMinorVersion));

	KdPrint(("Sample driver initialized successfully!\n"));
	KdPrint(("INIT: Copied RegistryPath: %wZ\n", &Copied_RegistryPath));

	return STATUS_SUCCESS;
}

/* 
	This function is not called if initialization fails.
	Ensure any allocated resources are deallocated.
*/
void DriverUnload(_In_ PDRIVER_OBJECT DriverObject) {
	UNREFERENCED_PARAMETER(DriverObject);

	KdPrint(("Sample driver DriverUnload called!\n"));
	KdPrint(("INIT: Copied RegistryPath: %wZ\n", &Copied_RegistryPath));

	/* Free Copied_RegistryPath memory buffer */
	ExFreePool(Copied_RegistryPath.Buffer);
}
