#ifndef _USER_H
#define _USER_H

#include <initguid.h>
#include <setupapi.h>
#pragma comment (lib, "setupapi.lib") 

DEFINE_GUID(CharSample_DEVINTERFACE_GUID, \
	0x9e69636d, 0x6b4f, 0x4472, 0xa3, 0x17, 0x27, 0x51, 0x27, 0x98, 0xa4, 0xa0);

#define CharSample_IOCTL_800 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define CharSample_IOCTL_801 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)

#endif