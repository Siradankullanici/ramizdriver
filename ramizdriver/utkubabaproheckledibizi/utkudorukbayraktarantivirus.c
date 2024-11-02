#include <ntifs.h>  // This should be first
#include <ntddk.h>
#include <ntstrsafe.h>

#define DELETE_LIST_FILE_PATH L"\\??\\C:\\utkubabaproistekleri.txt"
#define LOG_FILE_PATH L"\\??\\C:\\utbabaheck.log"
#define MAX_PATH_LENGTH 260
#define POOL_TAG 'Tag1'  // Define a tag for memory allocation

// Declare the DriverEntry function with the DRIVER_INITIALIZE macro
DRIVER_INITIALIZE DriverEntry;

// Declare the UnloadDriver function with the DRIVER_UNLOAD macro
DRIVER_UNLOAD UnloadDriver;

NTSTATUS ReadFileContent(PCWSTR filePath, PVOID* content, ULONG* size) {
    NTSTATUS status;
    HANDLE fileHandle;
    OBJECT_ATTRIBUTES objAttr;
    IO_STATUS_BLOCK ioStatusBlock;
    UNICODE_STRING uFileName;
    FILE_STANDARD_INFORMATION fileInfo;

    RtlInitUnicodeString(&uFileName, filePath);
    InitializeObjectAttributes(&objAttr, &uFileName, OBJ_KERNEL_HANDLE, NULL, NULL);

    status = ZwOpenFile(&fileHandle, GENERIC_READ, &objAttr, &ioStatusBlock, FILE_OPEN, FILE_NON_DIRECTORY_FILE);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    *size = 0;  // Initialize size to zero
    *content = NULL;  // Initialize content to NULL

    // Get the file size
    status = ZwQueryInformationFile(fileHandle, &ioStatusBlock, &fileInfo, sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation);
    if (!NT_SUCCESS(status)) {
        ZwClose(fileHandle);
        return status;
    }

    *size = fileInfo.EndOfFile.LowPart;  // Assuming file size is within 4GB

    // Allocate memory for the file content
    *content = ExAllocatePoolWithTag(NonPagedPool, *size, POOL_TAG);
    if (*content == NULL) {
        ZwClose(fileHandle);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    // Read the file content
    status = ZwReadFile(fileHandle, NULL, NULL, NULL, &ioStatusBlock, *content, *size, NULL, NULL);
    ZwClose(fileHandle);
    
    return status;
}

NTSTATUS DeleteFile(PCWSTR filePath) {
    NTSTATUS status;
    HANDLE fileHandle;
    OBJECT_ATTRIBUTES objAttr;
    UNICODE_STRING uFileName;
    IO_STATUS_BLOCK ioStatusBlock;
    FILE_DISPOSITION_INFORMATION fileDispInfo;

    RtlInitUnicodeString(&uFileName, filePath);
    InitializeObjectAttributes(&objAttr, &uFileName, OBJ_KERNEL_HANDLE, NULL, NULL);

    status = ZwOpenFile(&fileHandle, DELETE, &objAttr, &ioStatusBlock, FILE_OPEN, FILE_NON_DIRECTORY_FILE);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    // Set the file to be deleted
    fileDispInfo.DeleteFile = TRUE;
    status = ZwSetInformationFile(fileHandle, &ioStatusBlock, &fileDispInfo, sizeof(FILE_DISPOSITION_INFORMATION), FileDispositionInformation);

    ZwClose(fileHandle);
    return status;
}

// Add the correct signature for the UnloadDriver function
void UnloadDriver(PDRIVER_OBJECT DriverObject) {
    // Cleanup operations should be performed here
}

// The DriverEntry function should now be declared using the DRIVER_INITIALIZE macro
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
    NTSTATUS status;
    PVOID fileContent = NULL;
    ULONG fileSize = 0;

    // Read the file content
    status = ReadFileContent(DELETE_LIST_FILE_PATH, &fileContent, &fileSize);
    if (NT_SUCCESS(status)) {
        // Process the file content as needed
    } else {
        // Log the error
        KdPrint(("Failed to read file: 0x%X\n", status));
    }

    // Check if we are not deleting the log file or delete list file
    if (wcscmp(DELETE_LIST_FILE_PATH, LOG_FILE_PATH) != 0 &&
        wcscmp(DELETE_LIST_FILE_PATH, DELETE_LIST_FILE_PATH) != 0) {
        // Delete the file
        status = DeleteFile(DELETE_LIST_FILE_PATH);
        if (!NT_SUCCESS(status)) {
            // Log the error
            KdPrint(("Failed to delete file: 0x%X\n", status));
        }
    } else {
        KdPrint(("Skipping deletion of file: %ws\n", DELETE_LIST_FILE_PATH));
    }

    // Free the allocated memory
    if (fileContent != NULL) {
        ExFreePoolWithTag(fileContent, POOL_TAG);
    }

    // Set the Unload routine
    DriverObject->DriverUnload = UnloadDriver;

    return STATUS_SUCCESS;
}
