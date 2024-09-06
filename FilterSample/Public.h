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

//DEFINE_GUID (GUID_DEVINTERFACE_FilterSample,
//    0x9858759c,0xe226,0x48e3,0xbd,0x07,0xf3,0x3b,0x09,0x76,0x6f,0x79);
DEFINE_GUID(CharSample_DEVINTERFACE_GUID, \
    0x9e69636d, 0x6b4f, 0x4472, 0xa3, 0x17, 0x27, 0x51, 0x27, 0x98, 0xa4, 0xa0);
// {9858759c-e226-48e3-bd07-f33b09766f79}
