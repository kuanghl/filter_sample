// Test_FilterSample.cpp : Defines the entry point for the console application.
//

#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"

#include <windows.h>
#include <setupapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <winioctl.h>

#include "public.h"

#include <locale.h>

#include "stdint.h"

PCHAR
GetDevicePath(
    IN  LPGUID InterfaceGuid
);

// 假设x是一个16位的整数
#define SWAP16(x) ((uint16_t)((x & 0xFF) << 8) | ((x & 0xFF00) >> 8))

// 假设x是一个32位的整数
#define SWAP32(x) ((uint32_t)((x & 0xFF) << 24) | ((x & 0xFF00) << 8) | ((x & 0xFF0000) >> 8) | ((x & 0xFF000000) >> 24))

// 假设x是一个64位的整数
#define SWAP64(x) ((uint64_t)((x & 0xFF) << 56) | ((x & 0xFF00) << 40) | ((x & 0xFF0000) << 24) | ((x & 0xFF000000) << 8) | ((x & 0xFF00000000) >> 8) | ((x & 0xFF0000000000) >> 24) | ((x & 0xFF000000000000) >> 40) | ((x & 0xFF00000000000000) >> 56))

int main(int argc, char* argv[])
{
    PCHAR  DevicePath;
    HANDLE hDevice = INVALID_HANDLE_VALUE;
    SHORT* tmp = NULL;
    CHAR c[] = "零一二三四五六七八九";
    CHAR buf[64] = { 0 };

    strncpy((PCHAR)buf, &c[4 * 2], 2);

    printf("字符串: %s 字 %s value 0x%x 0x%x\n", c, buf, buf[0], buf[1]);

    //setlocale(LC_CTYPE, "");

    printf("Application Test_FilterSample starting...\n");

    DevicePath = GetDevicePath((LPGUID)&CharSample_DEVINTERFACE_GUID);

    hDevice = CreateFile((LPCWSTR)DevicePath,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (hDevice == INVALID_HANDLE_VALUE) {
        printf("ERROR opening device: (%0x) returned from CreateFile\n", GetLastError());
        return 0;
    }

    printf("OK.\n");

    CHAR	bufInput[3];	// Input to device
    CHAR	bufOutput[6];	// Output from device
    ULONG	nOutput, i;		// Count written to bufOutput

    printf("请输入三个数字(0-9)\n");
l0:	bufInput[0] = _getch();
    if ((bufInput[0] < '0') || (bufInput[0] > '9')) goto l0;
    _putch(bufInput[0]);

l1:	bufInput[1] = _getch();
    if ((bufInput[1] < '0') || (bufInput[1] > '9')) goto l1;
    _putch(bufInput[1]);

l2:	bufInput[2] = _getch();
    if ((bufInput[2] < '0') || (bufInput[2] > '9')) goto l2;
    _putch(bufInput[2]);

    printf("\n");

    // Call device IO Control interface (CharSample_IOCTL_800) in driver
    if (!DeviceIoControl(hDevice,
        CharSample_IOCTL_800,
        bufInput,  // input
        3,         // input length
        bufOutput,
        6,
        &nOutput,
        NULL)
        )
    {
        printf("ERROR: DeviceIoControl returns %0x.", GetLastError());
        goto exit;
    }
    
    printf("IOCTL_800:");
    for (i = 0; i < nOutput; i++) {
        if (i % 2) {
            tmp = (SHORT*)&bufOutput[i];
            SWAP16((*tmp));
        }
        //_putch(bufOutput[i]);
        //printf("value: 0x%x\n", bufOutput[i]);
    }
    printf("%s", bufOutput);
    printf("\n");


    memset(bufOutput, 0, sizeof(bufOutput));
    // Call device IO Control interface (CharSample_IOCTL_801) in driver
    if (!DeviceIoControl(hDevice,
        CharSample_IOCTL_801,
        bufInput,   // input
        3,          // input length
        bufOutput,
        6,
        &nOutput,
        NULL)
        )
    {
        printf("ERROR: DeviceIoControl returns %0x.", GetLastError());
        goto exit;
    }

    printf("IOCTL_801:");
    for (i = 0; i < nOutput; i++) {
        if (i % 2) {
            tmp = (SHORT*)&bufOutput[i];
            SWAP16((*tmp));
        }
        //_putch(bufOutput[i]);
        //printf("value: 0x%x\n", bufOutput[i]);
    }  
    printf("%s", bufOutput);
    printf("\n");

exit:

    if (hDevice != INVALID_HANDLE_VALUE) {
        CloseHandle(hDevice);
    }

    system("pause");
    return 0;
}

PCHAR
GetDevicePath(
    IN  LPGUID InterfaceGuid
)
{
    HDEVINFO HardwareDeviceInfo;
    SP_DEVICE_INTERFACE_DATA DeviceInterfaceData;
    PSP_DEVICE_INTERFACE_DETAIL_DATA DeviceInterfaceDetailData = NULL;
    ULONG Length, RequiredLength = 0;
    BOOL bResult;

    HardwareDeviceInfo = SetupDiGetClassDevs(
        InterfaceGuid,
        NULL,
        NULL,
        (DIGCF_PRESENT | DIGCF_DEVICEINTERFACE));

    if (HardwareDeviceInfo == INVALID_HANDLE_VALUE) {
        printf("SetupDiGetClassDevs failed!\n");
        exit(1);
    }

    DeviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    bResult = SetupDiEnumDeviceInterfaces(HardwareDeviceInfo,
        0,
        InterfaceGuid,
        0,
        &DeviceInterfaceData);

    if (bResult == FALSE) {
        printf("SetupDiEnumDeviceInterfaces failed.\n");

        SetupDiDestroyDeviceInfoList(HardwareDeviceInfo);
        exit(1);
    }

    SetupDiGetDeviceInterfaceDetail(
        HardwareDeviceInfo,
        &DeviceInterfaceData,
        NULL,
        0,
        &RequiredLength,
        NULL
    );

    DeviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)LocalAlloc(LMEM_FIXED, RequiredLength);

    if (DeviceInterfaceDetailData == NULL) {
        SetupDiDestroyDeviceInfoList(HardwareDeviceInfo);
        printf("Failed to allocate memory.\n");
        exit(1);
    }

    DeviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

    Length = RequiredLength;

    bResult = SetupDiGetDeviceInterfaceDetail(
        HardwareDeviceInfo,
        &DeviceInterfaceData,
        DeviceInterfaceDetailData,
        Length,
        &RequiredLength,
        NULL);

    if (bResult == FALSE) {
        printf("Error in SetupDiGetDeviceInterfaceDetail\n");

        SetupDiDestroyDeviceInfoList(HardwareDeviceInfo);
        LocalFree(DeviceInterfaceDetailData);
        exit(1);
    }

    return (PCHAR)DeviceInterfaceDetailData->DevicePath;

}
