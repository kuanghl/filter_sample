/*++

Module Name:

    queue.h

Abstract:

    This file contains the queue definitions.

Environment:

    Kernel-mode Driver Framework

--*/
#ifndef __QUEUE_H__
#define __QUEUE_H__


EXTERN_C_START

//
// This is the context that can be placed per queue
// and would contain per queue information.
//
#define BUFFER_LEN 80

typedef struct _QUEUE_CONTEXT {

    PCHAR pin;  // 定义输入缓冲区指针
    PCHAR pout; // 定义输出缓冲区指针

} QUEUE_CONTEXT, * PQUEUE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(QUEUE_CONTEXT, GetQueueContext)

typedef struct _REQUEST_CONTEXT {

    ULONG nin; //定义输入字节长度变量
    PCHAR cin; //定义输入缓冲区指针
    ULONG nout;//定义输出字节长度变量
    PCHAR cout;//定义输出缓冲区指针

} REQUEST_CONTEXT, * PREQUEST_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(REQUEST_CONTEXT, GetRequestContext)

NTSTATUS
FilterSampleQueueInitialize(
    _In_ WDFDEVICE Device,
    _Inout_ PWDFDEVICE_INIT DeviceInit
    );

//
// Events from the IoQueue object
//
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL FilterSampleEvtIoDeviceControl;
EVT_WDF_IO_QUEUE_IO_STOP FilterSampleEvtIoStop;

VOID
FilterSample_EvtQueueContextCleanup(
    IN WDFQUEUE Queue
);

VOID
FilterSample_EvtRequestIoctlCompletionRoutine(
    IN WDFREQUEST                  Request,
    IN WDFIOTARGET                 Target,
    PWDF_REQUEST_COMPLETION_PARAMS CompletionParams,
    IN WDFCONTEXT                  Context
);



EXTERN_C_END

#endif // !__QUEUE_H__
