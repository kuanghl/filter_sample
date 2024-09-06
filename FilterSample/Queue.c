/*++

Module Name:

    queue.c

Abstract:

    This file contains the queue entry points and callbacks.

Environment:

    Kernel-mode Driver Framework

--*/

#include "driver.h"
#include "queue.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, FilterSampleQueueInitialize)
#endif

NTSTATUS
FilterSampleQueueInitialize(
    _In_ WDFDEVICE Device,
    _Inout_ PWDFDEVICE_INIT DeviceInit
)
/*++

Routine Description:

     The I/O dispatch callbacks for the frameworks device object
     are configured in this function.

     A single default I/O Queue is configured for parallel request
     processing, and a driver context memory allocation is created
     to hold our structure QUEUE_CONTEXT.

Arguments:

    Device - Handle to a framework device object.

Return Value:

    VOID

--*/
{
    WDFQUEUE queue;
    NTSTATUS status;
    WDF_IO_QUEUE_CONFIG queueConfig;
    WDF_OBJECT_ATTRIBUTES	queueAttributes;
    PQUEUE_CONTEXT			pQueueContext;

    PAGED_CODE();

    DDEBUG("Entry\n");

    //
    // Configure a default queue so that requests that are not
    // configure-fowarded using WdfDeviceConfigureRequestDispatching to goto
    // other queues get dispatched here.
    //
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(
         &queueConfig,
        WdfIoQueueDispatchParallel
        );

    queueConfig.EvtIoDeviceControl = FilterSampleEvtIoDeviceControl;
    queueConfig.EvtIoStop = FilterSampleEvtIoStop;

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&queueAttributes, QUEUE_CONTEXT);

    //这里设置队列对象清除例程，因为下面为队列对象环境变量申请了内存
    //删除队列对象时，在清除例程中应释放这些内存
    queueAttributes.EvtCleanupCallback = FilterSample_EvtQueueContextCleanup;

    status = WdfIoQueueCreate(
                 Device,
                 &queueConfig,
                 &queueAttributes,
                 &queue
                 );

    if(!NT_SUCCESS(status)) {
        DDEBUG("WdfIoQueueCreate failed %d\n", status);
        return status;
    }

    pQueueContext = GetQueueContext(queue);

    pQueueContext->pin =				//申请一个内存，用于存放数字
        ExAllocatePool2(POOL_FLAG_NON_PAGED, BUFFER_LEN, 'tlif');
    pQueueContext->pout =				//申请一个内存，用于存放中文
        ExAllocatePool2(POOL_FLAG_NON_PAGED, BUFFER_LEN * 2, 'tlif');

    return status;
}

VOID
FilterSampleEvtIoDeviceControl(
    _In_ WDFQUEUE Queue,
    _In_ WDFREQUEST Request,
    _In_ size_t OutputBufferLength,
    _In_ size_t InputBufferLength,
    _In_ ULONG IoControlCode
    )
/*++

Routine Description:

    This event is invoked when the framework receives IRP_MJ_DEVICE_CONTROL request.

Arguments:

    Queue -  Handle to the framework queue object that is associated with the
             I/O request.

    Request - Handle to a framework request object.

    OutputBufferLength - Size of the output buffer in bytes

    InputBufferLength - Size of the input buffer in bytes

    IoControlCode - I/O control code.

Return Value:

    VOID

--*/
{
    NTSTATUS			status;
    PQUEUE_CONTEXT		pQueueContext;
    PREQUEST_CONTEXT	pRequestContext;
    PVOID				buffer;
    WDF_REQUEST_SEND_OPTIONS options;
    BOOLEAN				ret;

    PAGED_CODE();

    DDEBUG("Queue 0x%p, Request 0x%p OutputBufferLength %d InputBufferLength %d IoControlCode %d\n", 
                Queue, Request, (int) OutputBufferLength, (int) InputBufferLength, IoControlCode);

    switch (IoControlCode) {

    case CharSample_IOCTL_800:

        if (InputBufferLength == 0
            || InputBufferLength > BUFFER_LEN
            || OutputBufferLength != InputBufferLength * 2)
        {
            WdfRequestComplete(Request, STATUS_INVALID_PARAMETER);
            break;
        }

        pQueueContext = GetQueueContext(Queue);
        pRequestContext = GetRequestContext(Request);

        pRequestContext->nin = (ULONG)InputBufferLength;  //要转换的数字数目，如3 个数字
        pRequestContext->nout = (ULONG)OutputBufferLength; //输出缓冲区大小，如6 个字节

        pRequestContext->cin = pQueueContext->pin;
        pRequestContext->cout = pQueueContext->pout;

        status = WdfRequestRetrieveInputBuffer(Request, 1, &buffer, NULL);
        if (!NT_SUCCESS(status)) {
            WdfRequestComplete(Request, STATUS_UNSUCCESSFUL);
            break;
        }

        DDEBUG("buffer char %c value 0x%x\n", *(CHAR*)buffer, *(CHAR*)buffer);

        //将数字字符拷贝到cin 中
        strncpy(pRequestContext->cin, (PCHAR)buffer, InputBufferLength);

        //将I/O 请求发送到下层设备前，作相应的数据处理
        WdfRequestFormatRequestUsingCurrentType(Request);

        //设置I/O 请求完成回调例程
        WdfRequestSetCompletionRoutine(
            Request,
            FilterSample_EvtRequestIoctlCompletionRoutine,
            NULL);
        //将I/O 请求发送到下层设备
        ret = WdfRequestSend(
            Request,
            WdfDeviceGetIoTarget(WdfIoQueueGetDevice(Queue)),
            WDF_NO_SEND_OPTIONS);

        if (!ret) {
            status = WdfRequestGetStatus(Request);
            WdfRequestComplete(Request, status);
        }

        break;

    case CharSample_IOCTL_801:

        //不设置完成例程时，下面这句可有可无
        //WdfRequestFormatRequestUsingCurrentType(Request);

        //不对CharSample_IOCTL_801 作过滤处理，仅将其I/O 请求传递下去
        WDF_REQUEST_SEND_OPTIONS_INIT(
            &options,
            WDF_REQUEST_SEND_OPTION_SEND_AND_FORGET
        );

        ret = WdfRequestSend(
            Request,
            WdfDeviceGetIoTarget(WdfIoQueueGetDevice(Queue)),
            &options
        );
        if (!ret) {
            status = WdfRequestGetStatus(Request);
            WdfRequestComplete(Request, status);
        }

        break;
    default:
        status = STATUS_INVALID_DEVICE_REQUEST;
        WdfRequestCompleteWithInformation(Request, status, 0);
        break;
    }

    //WdfRequestComplete(Request, STATUS_SUCCESS);

    return;
}

VOID
FilterSampleEvtIoStop(
    _In_ WDFQUEUE Queue,
    _In_ WDFREQUEST Request,
    _In_ ULONG ActionFlags
)
/*++

Routine Description:

    This event is invoked for a power-managed queue before the device leaves the working state (D0).

Arguments:

    Queue -  Handle to the framework queue object that is associated with the
             I/O request.

    Request - Handle to a framework request object.

    ActionFlags - A bitwise OR of one or more WDF_REQUEST_STOP_ACTION_FLAGS-typed flags
                  that identify the reason that the callback function is being called
                  and whether the request is cancelable.

Return Value:

    VOID

--*/
{ 
    DDEBUG("Queue 0x%p, Request 0x%p ActionFlags %d\n", 
                Queue, Request, ActionFlags);

    //
    // In most cases, the EvtIoStop callback function completes, cancels, or postpones
    // further processing of the I/O request.
    //
    // Typically, the driver uses the following rules:
    //
    // - If the driver owns the I/O request, it calls WdfRequestUnmarkCancelable
    //   (if the request is cancelable) and either calls WdfRequestStopAcknowledge
    //   with a Requeue value of TRUE, or it calls WdfRequestComplete with a
    //   completion status value of STATUS_SUCCESS or STATUS_CANCELLED.
    //
    //   Before it can call these methods safely, the driver must make sure that
    //   its implementation of EvtIoStop has exclusive access to the request.
    //
    //   In order to do that, the driver must synchronize access to the request
    //   to prevent other threads from manipulating the request concurrently.
    //   The synchronization method you choose will depend on your driver's design.
    //
    //   For example, if the request is held in a shared context, the EvtIoStop callback
    //   might acquire an internal driver lock, take the request from the shared context,
    //   and then release the lock. At this point, the EvtIoStop callback owns the request
    //   and can safely complete or requeue the request.
    //
    // - If the driver has forwarded the I/O request to an I/O target, it either calls
    //   WdfRequestCancelSentRequest to attempt to cancel the request, or it postpones
    //   further processing of the request and calls WdfRequestStopAcknowledge with
    //   a Requeue value of FALSE.
    //
    // A driver might choose to take no action in EvtIoStop for requests that are
    // guaranteed to complete in a small amount of time.
    //
    // In this case, the framework waits until the specified request is complete
    // before moving the device (or system) to a lower power state or removing the device.
    // Potentially, this inaction can prevent a system from entering its hibernation state
    // or another low system power state. In extreme cases, it can cause the system
    // to crash with bugcheck code 9F.
    //

    return;
}


VOID
FilterSample_EvtQueueContextCleanup(
    IN WDFQUEUE Queue
)
{	
    DDEBUG("Entry\n");

    //队列对象清除例程
    PQUEUE_CONTEXT pQueueContext;

    PAGED_CODE();

    pQueueContext = GetQueueContext(Queue);

    //删除队列对象时，释放其申请的内存
    ExFreePool(pQueueContext->pin);
    ExFreePool(pQueueContext->pout);
    return;
}

VOID
FilterSample_EvtRequestIoctlCompletionRoutine(
    IN WDFREQUEST                  Request,
    IN WDFIOTARGET                 Target,
    PWDF_REQUEST_COMPLETION_PARAMS CompletionParams,
    IN WDFCONTEXT                  Context
)
{	//I/O 请求完成例程
    NTSTATUS		 status;
    PREQUEST_CONTEXT pRequestContext;
    PVOID			 buffer;
    BOOLEAN			 ret;

    PAGED_CODE();

    DDEBUG("Entry\n");

    pRequestContext = GetRequestContext(Request);

    status = WdfRequestRetrieveOutputBuffer(Request, 2, &buffer, NULL);
    if (!NT_SUCCESS(status)) {
        WdfRequestComplete(Request, STATUS_UNSUCCESSFUL);
        return;
    }

    //将转换后的中文字符拷贝到cout 中
    strncpy(pRequestContext->cout, (PCHAR)buffer, 2);

    DDEBUG("output buffer len 0x%x input buffer len 0x%x\n", pRequestContext->nout, pRequestContext->nin);

    pRequestContext->cout += 2;//调整cout 指针，+2 是因为一个中文占2 个字节空间
    pRequestContext->nin -= 1;//完成一个转换后，nin 便减1
    if (pRequestContext->nin == 0)
    {	//nin 减至0，表示转换工作全部完成

        DDEBUG("output\n");

        //调整cout 指针，指向缓冲区的头
        pRequestContext->cout -= pRequestContext->nout;

        status = WdfRequestRetrieveOutputBuffer(Request, 2, &buffer, NULL);
        if (!NT_SUCCESS(status)) {
            WdfRequestComplete(Request, STATUS_UNSUCCESSFUL);
            return;
        }
        //将转换后的全部中文字符拷贝到应用程序的缓冲区中
        strncpy((PCHAR)buffer, pRequestContext->cout, pRequestContext->nout);

        //完成I/O 请求
        WdfRequestCompleteWithInformation(Request, STATUS_SUCCESS, pRequestContext->nout);
    }
    else
    {
        DDEBUG("input\n");

        pRequestContext->cin++;//调整cin 指针，指向下一个数字

        status = WdfRequestRetrieveInputBuffer(Request, 1, &buffer, NULL);
        if (!NT_SUCCESS(status)) {
            WdfRequestComplete(Request, STATUS_UNSUCCESSFUL);
            return;
        }

        //将下一个数字拷贝到应用程序的缓冲区中
        strncpy((PCHAR)buffer, pRequestContext->cin, 1);

        //设置I/O 请求完成例程，将I/O 请求再次传递下去

        WdfRequestFormatRequestUsingCurrentType(Request);

        WdfRequestSetCompletionRoutine(
            Request,
            FilterSample_EvtRequestIoctlCompletionRoutine,
            NULL);

        ret = WdfRequestSend(
            Request,
            Target,
            WDF_NO_SEND_OPTIONS);

        if (!ret) {
            status = WdfRequestGetStatus(Request);
            WdfRequestComplete(Request, status);
        }
    }
    return;
}
