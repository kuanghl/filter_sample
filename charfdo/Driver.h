/*++

Module Name:

    driver.h

Abstract:

    This file contains the driver definitions.

Environment:

    Kernel-mode Driver Framework

--*/

#include <ntddk.h>
#include <wdf.h>
#include <initguid.h>

#include "device.h"
#include "queue.h"
#include "trace.h"

EXTERN_C_START

//
// WDFDRIVER Events
//

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD charfdoEvtDeviceAdd;
EVT_WDF_OBJECT_CONTEXT_CLEANUP charfdoEvtDriverContextCleanup;


#define GET_FILENAME_FROM_PATH_FILE()   strrchr((const char*)__FILE__, '\\') ? strrchr((const char*)__FILE__, '\\') + 1 : \
                                        strrchr((const char*)__FILE__, '/') ? strrchr((const char*)__FILE__, '/') + 1 : (const char*)__FILE__

#define DDEBUG(fmt,...)				DbgPrint("%016s %s %-5s [%s:%d:%s()]: " fmt, __DATE__, __TIME__, "DEBUG", GET_FILENAME_FROM_PATH_FILE(), __LINE__, __func__, ##__VA_ARGS__)

#define CharSample_IOCTL_800 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define CharSample_IOCTL_801 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)

EXTERN_C_END
