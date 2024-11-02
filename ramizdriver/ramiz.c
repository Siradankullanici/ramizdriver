#define _AMD64_

#include <wdm.h>
#include <nttddk.h>

NTSTATUS DriverEntry(void* a, void* b) {
    KeBugCheck(0xaabbccdd);
    return STATUS_SUCCES;
    
}