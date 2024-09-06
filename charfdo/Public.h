/*++

Module Name:

    public.h

Abstract:

    This module contains the common declarations shared by driver
    and user applications.

Environment:

    user and kernel

--*/

//
// Define an Interface Guid so that apps can find the device and talk to it.
//

//DEFINE_GUID (GUID_DEVINTERFACE_charfdo,
//    0x9187c03d,0xdfb8,0x4e99,0xba,0x2d,0x57,0x18,0x0d,0x65,0x23,0xe1);
DEFINE_GUID(CharSample_DEVINTERFACE_GUID, \
    0x9e69636d, 0x6b4f, 0x4472, 0xa3, 0x17, 0x27, 0x51, 0x27, 0x98, 0xa4, 0xa0);
// {9187c03d-dfb8-4e99-ba2d-57180d6523e1}
