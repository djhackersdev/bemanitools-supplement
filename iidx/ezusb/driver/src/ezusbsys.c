//////////////////////////////////////////////////////////////////////
//
// File:      ezusbsys.c
// $Archive: /USB/Drivers/ezusbdrv/ezusbsys.c $
//
// Purpose:
//    General purpose USB device driver
//
// Environment:
//    kernel mode
//
// $Author: Mdn $
//
// $History: ezusbsys.c $           
//  
//  *****************  Version 6  *****************
//  User: Mdn          Date: 9/17/01    Time: 10:54a
//  Updated in $/USB/Drivers/ezusbdrv
//  Fixed memory leak bug in ISO streaming code.  Was setting the
//  PendingTransfers variable to the wrong value and not properly freeing
//  after stopping the ISO stream.
//  
//  *****************  Version 5  *****************
//  User: Mdn          Date: 10/06/00   Time: 10:08a
//  Updated in $/USB/Drivers/ezusbdrv
//  Added IOCTL_EZUSB_SET_FEATURE
//  
//  *****************  Version 4  *****************
//  User: Mdn          Date: 8/17/00    Time: 9:46a
//  Updated in $/USB/Drivers/ezusbdrv
//  added a generic get descriptor function.
//  
//  *****************  Version 3  *****************
//  User: Mdn          Date: 8/04/00    Time: 11:41a
//  Updated in $/USB/Drivers/ezmon
//  added support for monitor download to FX2
//  
//  *****************  Version 2  *****************
//  User: Mdn          Date: 7/21/00    Time: 4:27p
//  Updated in $/USB/Drivers/ezusbdrv
//  Added a Started flag to the device extension.  This flag gets set after
//  the device has successfully Started.  It is used to prevent a potential
//  race condition where an application could get a handle (with
//  CreateFile()) before the device is completely enumerated.
//  
//  *****************  Version 1  *****************
//  User: Tpm          Date: 6/09/00    Time: 6:32p
//  Created in $/USB/Drivers/ezusbdrv
//  
//  *****************  Version 42  *****************
//  User: Mdn          Date: 5/05/00    Time: 10:24a
//  Updated in $/EzUsb/Drivers/ezusbdrv
//  comments
//  
//  *****************  Version 41  *****************
//  User: Mdn          Date: 4/18/00    Time: 2:45p
//  Updated in $/EzUsb/Drivers/ezusbdrv
//  fixed memory leak where I wasn't freeing an URB allocated for
//  vendor/class requests.
//  
//  *****************  Version 40  *****************
//  User: Mdn          Date: 1/21/00    Time: 10:03a
//  Updated in $/EzUsb/Drivers/ezusbdrv
//  modified device removal code to send an abort message to all pipes
//  before proceding with device removal.  This solves the deadlock problem
//  that can occur when a transfer is pending and the device receives a
//  remove message.  Before this change, the driver would block the removal
//  indefinitely waiting for all pending transfers to complete.
//  
//  *****************  Version 39  *****************
//  User: Markm        Date: 12/17/99   Time: 11:17a
//  Updated in $/EzUsb/Drivers/ezusbdrv
//  Removed interface select workaround for an obscure OSR2.1 bug since the
//  driver no longer supports Win95.  Changed interface select so that it
//  selects the first interface/alternate setting in the config descriptor.
//  Before, it was hardcoded to select interface 0, AS 0.  This should
//  allow the driver to handle composite devices more easily, although I
//  haven't tested it.
//  
//  *****************  Version 38  *****************
//  User: Markm        Date: 10/06/99   Time: 1:18p
//  Updated in $/EzUsb/Drivers/ezusbdrv
//  Made changes to Power IRP handling to fix the blue screen on shutdown
//  that was occuring under Win2K.  Specifically, started using PoXXX()
//  calls for handling power IRPs.  See code comments in
//  Ezusb_DispatchPower(). The PoXXX() calls are not supported by Win95, so
//  as of this revision, the driver will no longer work with Windows 95.
//  
//  *****************  Version 37  *****************
//  User: Markm        Date: 5/18/99    Time: 3:37p
//  Updated in $/EzUsb/Drivers/ezusbdrv
//  
//  *****************  Version 36  *****************
//  User: Markm        Date: 4/29/99    Time: 9:48a
//  Updated in $/EzUsb/Drivers/ezusbdrv
//  Fixed IOCTL_EZUSB_VENDOR_OR_CLASS_REQUEST so that it correctly returns
//  the number of bytes transferred.
//  
//  *****************  Version 35  *****************
//  User: Markm        Date: 4/16/99    Time: 3:39p
//  Updated in $/EzUsb/Drivers/ezusbdrv
//  more minor changes to get rid of compiler warnings.
//  
//  *****************  Version 34  *****************
//  User: Markm        Date: 4/12/99    Time: 10:26a
//  Updated in $/EzUsb/Drivers/ezusbdrv
//  minor changes to get rid of compiler warnings.
//  
//  *****************  Version 33  *****************
//  User: Markm        Date: 3/25/99    Time: 4:16p
//  Updated in $/EzUsb/Drivers/ezusbdrv
//  Fixed a bug in the surprise removal code I just added.  I was returning
//  from the PnP dispatch function without unlocking the device object.
//  
//  *****************  Version 32  *****************
//  User: Markm        Date: 3/25/99    Time: 2:03p
//  Updated in $/EzUsb/Drivers/ezusbdrv
//  Added code to allow unplugs (surprise removal) under NT5 without
//  notifying the user.
//  
//  *****************  Version 31  *****************
//  User: Markm        Date: 2/23/99    Time: 9:50a
//  Updated in $/EzUsb/Drivers/ezusbdrv
//  Driver now supports ISO IN streaming with a path to user mode.
//  
//  *****************  Version 30  *****************
//  User: Markm        Date: 2/10/99    Time: 3:31p
//  Updated in $/EzUsb/Drivers/ezusbdrv
//  removed lots of unused code.  Added ring buffer support functions.
//  
//  *****************  Version 29  *****************
//  User: Markm        Date: 2/01/99    Time: 11:57a
//  Updated in $/EzUsb/Drivers/ezusbdrv
//  Added preliminary support for ISO streaming.
//  
//  *****************  Version 28  *****************
//  User: Markm        Date: 6/12/98    Time: 4:23p
//  Updated in $/EzUsb/Drivers/ezusbdrv
//  modified pipe reset and abort code so that the driver will work
//  correctly under Windows 95 OSR2.1.  For Win98, MS changed the size of
//  the URB_PIPE_REQUEST structure.  They added an extra ulong to it.  So,
//  a driver compiled with the 98 DDk and run under 95 will send the wrong
//  URB size.  The solution is to check the USBDI version whenever I do a
//  pipe reset or abort.  If the USBDI version is pre-win98, I subtract 4
//  from the size of the urb.  
//  
//  *****************  Version 27  *****************
//  User: Markm        Date: 4/10/98    Time: 2:52p
//  Updated in $/EZUSB/ezmon
//  Modified Intel Hex download code to stop the 8051 before downloading to
//  internal RAM.
//  
//  *****************  Version 26  *****************
//  User: Markm        Date: 4/09/98    Time: 4:39p
//  Updated in $/EZUSB/ezusb driver
//  Was not returning status information for ISO transfers.  Now it does!
//  
//  *****************  Version 25  *****************
//  User: Markm        Date: 4/09/98    Time: 4:39p
//  Updated in $/EZUSB/ezmon
//  changes for monitor version of driver
//  
//  *****************  Version 24  *****************
//  User: Markm        Date: 4/09/98    Time: 3:09p
//  Updated in $/EZUSB/ezusb driver
//  nothing much
//  
//  *****************  Version 23  *****************
//  User: Markm        Date: 4/09/98    Time: 3:00p
//  Updated in $/EZUSB/ezusb driver
//  Added function to download Intel Hex File records to EZ-USB.  For now,
//  this function is only used by the conditional build of the driver that
//  automatically downloads the Keil monitor.
//  
//  *****************  Version 22  *****************
//  User: Markm        Date: 4/07/98    Time: 1:52p
//  Updated in $/EZUSB/ezusb driver
//  Added IOCTL_EZUSB_GET_DRIVER_VERSION
//  
//  *****************  Version 21  *****************
//  User: Markm        Date: 4/06/98    Time: 4:26p
//  Updated in $/EZUSB/ezusb driver
//  Modified ISO transfer code.
//  * Transfer descriptors for the ISO transfer are now sent up to the
//  caller along with the actual data, so the caller can get the status of
//  the transfer on a packet-by-packet basis. 
//  * Disabled default values.  Caller must specify all fields in the ISO
//  control structure.
//  * Corrected bug where the Stream and Transfer objects weren't being
//  freed.
//  
//  Added some code to measure the latency of a bulk transfer.
//  
//  *****************  Version 20  *****************
//  User: Markm        Date: 3/19/98    Time: 10:13a
//  Updated in $/EZUSB/ezusb driver
//  Added IOCTL_EZUSB_ANCHOR_DOWNLOAD to support A0 loads to a specific
//  memory offset.
//  
//  *****************  Version 19  *****************
//  User: Markm        Date: 2/26/98    Time: 4:04p
//  Updated in $/EZUSB/ezusb driver
//  Added functions to perform anchor downloads and 8051 reset.
//  Added conditionally compiled code to support a special version of the
//  driver that will automatically download the Keil 8051 monitor after
//  device attachment.
//  
//  *****************  Version 18  *****************
//  User: Markm        Date: 2/25/98    Time: 2:09p
//  Updated in $/EZUSB/ezusb driver
//  changes for adding version resource to the driver
//  
//  *****************  Version 17  *****************
//  User: Markm        Date: 2/11/98    Time: 9:51a
//  Updated in $/EZUSB/ezusb driver
//  1. Added code to handle IRP_MN_CLOSE.
//  2. Now maintain a count of open file handles in the device extension.
//  3. Added workaround in Ezusb_SelectInterfaces() to avoid system hangs
//  during device removal under OSR2.1.  See comments there.
//  
//  *****************  Version 16  *****************
//  User: Markm        Date: 2/02/98    Time: 3:35p
//  Updated in $/EZUSB/ezusb driver
//  
//  *****************  Version 15  *****************
//  User: Markm        Date: 1/27/98    Time: 11:36a
//  Updated in $/EZUSB/ezusb driver
//  Modified ISO transfer code so that the number of transfer buffers and
//  the number of ISO frames per buffer can be specified by the caller.
//  
//  *****************  Version 14  *****************
//  User: Markm        Date: 1/26/98    Time: 10:11a
//  Updated in $/EZUSB/ezusb driver
//  1) modified device removal code that was crashing OSR2.1.  MS sample
//  code says that during device removal, the remove IRP should be passed
//  down the stack, and then the deviceobject should be deleted.  This
//  causes a bugcheck under OSR2.1.  Reversing the order of these
//  operations fixes the problem.
//  2) hardcoded the initial alternate setting to 2.  It was set to 1, but
//  this was also causing system crashes under OSR 2.1 during device
//  removal.  Still under investigation.
//  
//  *****************  Version 13  *****************
//  User: Markm        Date: 1/22/98    Time: 11:50a
//  Updated in $/EZUSB/ezusb driver
//  Fixed a bug in the device removal handler.  It wasn't deleteting the
//  symbolic link for the deviceobject after removal.
//  Removed lots of unused code.
//  Rewrote ISO loopback test code.
//  Wrote new ISO read/write functions.
//  
//  *****************  Version 12  *****************
//  User: Markm        Date: 1/18/98    Time: 3:20p
//  Updated in $/EZUSB/ezusb driver
//  renamed variables.  DeviceExtension becomes pdx and DeviceObject
//  becomes fdo.
//  rewrote handlers for device removal.   Cleaned up power management
//  dispatch.  Added code to maintain a usage count for the device.
//  
//  *****************  Version 11  *****************
//  User: Markm        Date: 1/15/98    Time: 4:36p
//  Updated in $/EZUSB/ezusb driver
//  Added
//  IOCTL_EZUSB_GET_CURRENT_FRAME_NUMBER
//  IOCTL_EZUSB_VENDOR_OR_CLASS_REQUEST
//  IOCTL_EZUSB_GET_LAST_ERROR
//  preliminary code for handling device removal gracefully.
//  
//  *****************  Version 10  *****************
//  User: Markm        Date: 1/14/98    Time: 10:29a
//  Updated in $/EZUSB/ezusb driver
//  Cleanup.
//  New functions for handling bulk transfers.
//  
//  *****************  Version 9  *****************
//  User: Markm        Date: 1/08/98    Time: 5:14p
//  Updated in $/EZUSB/ezusb driver
//  major changes to PnP IRP handling.  lots of reformatting.
//  
//  *****************  Version 8  *****************
//  User: Markm        Date: 1/02/98    Time: 1:41p
//  Updated in $/EZUSB/ezusb driver
//  Added support for setting the interface, preliminary code for naming
//  pipes, get string descriptor
//  
//  *****************  Version 7  *****************
//  User: Markm        Date: 11/18/97   Time: 4:40p
//  Updated in $/EZUSB/ezusb driver
//  Added abort pipe IOCTL
//  Added function to dump a buffer to the debuger
//  Added experimental file I/O code (from Brad Carpenter)
//  
//  *****************  Version 6  *****************
//  User: Markm        Date: 11/17/97   Time: 9:37a
//  Updated in $/EZUSB/ezusb driver
//  fixed bug where I set the pipesize
//  
//  *****************  Version 5  *****************
//  User: Markm        Date: 11/14/97   Time: 4:53p
//  Updated in $/EZUSB/ezusb driver
//  started using USBD_CreateConfigurationRequestEx
//  
//  *****************  Version 4  *****************
//  User: Markm        Date: 11/14/97   Time: 4:31p
//  Updated in $/EZUSB/ezusb driver
//  added code to experiment wth different methods of switiching
//  interfaces.
//  
//  *****************  Version 3  *****************
//  User: Markm        Date: 11/07/97   Time: 1:23p
//  Updated in $/EZUSB/ezusb driver
//  added sourcesafe keywords
//  
// Copyright (c) 1997 Anchor Chips, Inc.  May not be reproduced without
// permission.  See the license agreement for more details.
//
//////////////////////////////////////////////////////////////////////

// #define DRIVER
//
// Include files needed for WDM driver support
//
#include <wdm.h>
#include "stdarg.h"
#include "stdio.h"

//
// Include files needed for USB support
//
#include "usbdi.h"
#include "usbdlib.h"

//
// Include file for the Ezusb Device
//
#include "ezusbsys.h"

//
// incude file containing driver version
//
#include "version.h"

#ifdef DOWNLOAD_KEIL_MONITOR

extern INTEL_HEX_RECORD mon_ext_sio1_ezusb[];
extern INTEL_HEX_RECORD mon_ext_sio1_fx2[];
extern INTEL_HEX_RECORD loader[];

#endif // ifdef DOWNLOAD_KEIL_MONITOR


void
DumpBuffer(PVOID pvBuffer, ULONG length);

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:
   Installable driver initialization entry point.
   This is where the driver is called when the driver is being loaded
   by the I/O system.  This entry point is called directly by the I/O system.

Arguments:
   DriverObject - pointer to the driver object
   RegistryPath - pointer to a unicode string representing the path
                  to driver-specific key in the registry

Return Value:
   STATUS_SUCCESS if successful,
   STATUS_UNSUCCESSFUL otherwise

--*/
{
   NTSTATUS ntStatus = STATUS_SUCCESS;
   PDEVICE_OBJECT deviceObject = NULL;

   Ezusb_KdPrint (("entering (Ezusb) DriverEntry (Build: %s/%s\n",__DATE__,__TIME__));

   //
   // Create dispatch points for the various events handled by this
   // driver.  For example, device I/O control calls (e.g., when a Win32
   // application calls the DeviceIoControl function) will be dispatched to
   // routine specified below in the IRP_MJ_DEVICE_CONTROL case.
   //
   DriverObject->MajorFunction[IRP_MJ_CREATE] = Ezusb_Create;
   DriverObject->MajorFunction[IRP_MJ_CLOSE] = Ezusb_Close;
   DriverObject->DriverUnload = Ezusb_Unload;

   DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = Ezusb_ProcessIOCTL;

   DriverObject->MajorFunction[IRP_MJ_PNP] = Ezusb_DispatchPnp;
   DriverObject->MajorFunction[IRP_MJ_POWER] = Ezusb_DispatchPower;
   DriverObject->DriverExtension->AddDevice = Ezusb_PnPAddDevice;

   Ezusb_KdPrint (("exiting (Ezusb) DriverEntry (%x)\n", ntStatus));

   return ntStatus;
}

NTSTATUS
Ezusb_DefaultPnpHandler(
   IN PDEVICE_OBJECT fdo,
   IN PIRP Irp
   )
{
   PDEVICE_EXTENSION pdx = (PDEVICE_EXTENSION) fdo->DeviceExtension;

   IoSkipCurrentIrpStackLocation(Irp);
   return IoCallDriver(pdx->StackDeviceObject, Irp);
}

///////////////////////////////////////////////////////////////////////////////
// @func Handle completion of a request by a lower-level driver
// @parm Functional device object
// @parm I/O request which has completed
// @parm Context argument supplied to IoSetCompletionRoutine, namely address of
// KEVENT object on which ForwardAndWait is waiting
// @comm This is the completion routine used for requests forwarded by ForwardAndWait. It
// sets the event object and thereby awakens ForwardAndWait.
// @comm Note that it's *not* necessary for this particular completion routine to test
// the PendingReturned flag in the IRP and then call IoMarkIrpPending. You do that in many
// completion routines because the dispatch routine can't know soon enough that the
// lower layer has returned STATUS_PENDING. In our case, we're never going to pass a
// STATUS_PENDING back up the driver chain, so we don't need to worry about this.

NTSTATUS 
OnRequestComplete(
   IN PDEVICE_OBJECT fdo,
   IN PIRP Irp,
   IN PKEVENT pev
   )
/*++

Routine Description:
   Handle completion of a request by a lower-level driver

Arguments:
   DriverObject -  Functional device object
   Irp - I/O request which has completed
   pev - Context argument supplied to IoSetCompletionRoutine, namely address of
         KEVENT object on which ForwardAndWait is waiting

Return Value:
   STATUS_MORE_PROCESSING_REQUIRED
--*/
{
   KeSetEvent(pev, 0, FALSE);
   return STATUS_MORE_PROCESSING_REQUIRED;
}

NTSTATUS
ForwardAndWait(
   IN PDEVICE_OBJECT fdo,
   IN PIRP Irp
   )
/*++
Routine Description:
   Forward request to lower level and await completion

   The only purpose of this routine in this particular driver is to pass down
   IRP_MN_START_DEVICE requests and wait for the PDO to handle them.
   
   The processor must be at PASSIVE IRQL because this function initializes
   and waits for non-zero time on a kernel event object.

Arguments:
   fdo - pointer to a device object
   Irp          - pointer to an I/O Request Packet

Return Value:
   STATUS_SUCCESS if successful,
   STATUS_UNSUCCESSFUL otherwise
--*/
{
	KEVENT event;
	PDEVICE_EXTENSION pdx = (PDEVICE_EXTENSION) fdo->DeviceExtension;
	NTSTATUS ntStatus;

   ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);
	
	//
   // Initialize a kernel event object to use in waiting for the lower-level
	// driver to finish processing the object. 
   //
	KeInitializeEvent(&event, NotificationEvent, FALSE);

	IoCopyCurrentIrpStackLocationToNext(Irp);
	IoSetCompletionRoutine(Irp, (PIO_COMPLETION_ROUTINE) OnRequestComplete,
		(PVOID) &event, TRUE, TRUE, TRUE);

	ntStatus = IoCallDriver(pdx->StackDeviceObject, Irp);

	if (ntStatus == STATUS_PENDING)
	{
      KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
      ntStatus = Irp->IoStatus.Status;
   }

	return ntStatus;
}

NTSTATUS
CompleteRequest(
   IN PIRP Irp,
   IN NTSTATUS status,
   IN ULONG info
   )
/*++
Routine Description:
   Mark I/O request complete

Arguments:
   Irp - I/O request in question
   status - Standard status code
   info Additional information related to status code

Return Value:
   STATUS_SUCCESS if successful,
   STATUS_UNSUCCESSFUL otherwise
--*/
{
	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = info;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

   return status;
}

NTSTATUS
Ezusb_DispatchPnp(
   IN PDEVICE_OBJECT fdo,
   IN PIRP           Irp
   )
/*++
Routine Description:
   Process Plug and Play IRPs sent to this device.

Arguments:
   fdo - pointer to a device object
   Irp          - pointer to an I/O Request Packet

Return Value:
   NTSTATUS
--*/
{
   PIO_STACK_LOCATION irpStack;
   PDEVICE_EXTENSION pdx = fdo->DeviceExtension;
   ULONG fcn;
   NTSTATUS ntStatus;

   Ezusb_KdPrint (("Enter Ezusb_DispatchPnp\n"));

   if (!LockDevice(fdo))
		return CompleteRequest(Irp, STATUS_DELETE_PENDING, 0);

   //
   // Get a pointer to the current location in the Irp. This is where
   //     the function codes and parameters are located.
   //
   irpStack = IoGetCurrentIrpStackLocation (Irp);

   ASSERT(irpStack->MajorFunction == IRP_MJ_PNP);

   fcn = irpStack->MinorFunction;

   switch (fcn)
   {
      case IRP_MN_START_DEVICE:

         Ezusb_KdPrint (("IRP_MN_START_DEVICE\n"));

         ntStatus = Ezusb_HandleStartDevice(fdo,Irp);

         if (ntStatus == STATUS_SUCCESS)
         {
            pdx->Started = TRUE;
         }

         break; //IRP_MN_START_DEVICE

      case IRP_MN_STOP_DEVICE:

         Ezusb_KdPrint (("IRP_MN_STOP_DEVICE\n"));

         //
         // first pass the request down the stack
         //
         Ezusb_DefaultPnpHandler(fdo,Irp);

         ntStatus = Ezusb_StopDevice(fdo);

         break; //IRP_MN_STOP_DEVICE

      case IRP_MN_REMOVE_DEVICE:

         Ezusb_KdPrint (("IRP_MN_REMOVE_DEVICE\n"))

         ntStatus = Ezusb_HandleRemoveDevice(fdo,Irp);

         break; //IRP_MN_REMOVE_DEVICE

      case IRP_MN_QUERY_CAPABILITIES:
      {
         //
         // This code swiped from Walter Oney.  Please buy his book!!
         //

      	PDEVICE_CAPABILITIES pdc = irpStack->Parameters.DeviceCapabilities.Capabilities;

         Ezusb_KdPrint (("IRP_MN_QUERY_CAPABILITIES\n"))

         // Check to be sure we know how to handle this version of the capabilities structure

	      if (pdc->Version < 1)
         {
		      ntStatus = Ezusb_DefaultPnpHandler(fdo, Irp);
            break;
         }

         ntStatus = ForwardAndWait(fdo, Irp);
	      if (NT_SUCCESS(ntStatus))
   		{						// IRP succeeded
      		pdc = irpStack->Parameters.DeviceCapabilities.Capabilities;
            // setting this field prevents NT5 from notifying the user when the
            // device is removed.
		      pdc->SurpriseRemovalOK = TRUE;
   		}						// IRP succeeded

	      ntStatus = CompleteRequest(Irp, ntStatus, Irp->IoStatus.Information);
      }
         break; //IRP_MN_QUERY_CAPABILITIES


      //
      // All other PNP IRP's are just passed down the stack by the default handler
      //
      default:
        Ezusb_KdPrint (("Passing down unhandled PnP IOCTL 0x%x\n", fcn));
        ntStatus = Ezusb_DefaultPnpHandler(fdo, Irp);

   } // switch MinorFunction

	if (fcn != IRP_MN_REMOVE_DEVICE)
      UnlockDevice(fdo);

   Ezusb_KdPrint (("Exit Ezusb_DispatchPnp %x\n", ntStatus));
   return ntStatus;

}//Ezusb_Dispatch


NTSTATUS
Ezusb_DispatchPower(
    IN PDEVICE_OBJECT fdo,
    IN PIRP           Irp
    )
/*++
Routine Description:
   Process the IRPs sent to this device.

Arguments:
   fdo - pointer to a device object
   Irp          - pointer to an I/O Request Packet

Return Value:
   NTSTATUS
--*/
{
   PIO_STACK_LOCATION irpStack, nextStack;
   PDEVICE_EXTENSION pdx = fdo->DeviceExtension;
   NTSTATUS ntStatus;

   Ezusb_KdPrint (("Enter Ezusb_DispatchPower\n"));

   Irp->IoStatus.Status = STATUS_SUCCESS;
   Irp->IoStatus.Information = 0;

   //
   // Get a pointer to the current location in the Irp. This is where
   //     the function codes and parameters are located.
   //
   irpStack = IoGetCurrentIrpStackLocation (Irp);



   Ezusb_KdPrint (("IRP_MJ_POWER MIN=0x%x Type=0x%x State=0x%x\n",irpStack->MinorFunction,
      irpStack->Parameters.Power.Type,
      irpStack->Parameters.Power.State.DeviceState));

   switch (irpStack->MinorFunction)
   {
      case IRP_MN_SET_POWER:

         switch (irpStack->Parameters.Power.Type)
         {
            case SystemPowerState:

               break; //SystemPowerState

            case DevicePowerState:
               switch (irpStack->Parameters.Power.State.DeviceState)
               {
                  case PowerDeviceD3:
                     Ezusb_KdPrint (("IRP_MN_SET_D3\n"));
                     break;
                  case PowerDeviceD2:
                     Ezusb_KdPrint (("IRP_MN_SET_D2\n"));
                     break;
                  case PowerDeviceD1:
                     Ezusb_KdPrint (("IRP_MN_SET_D1\n"));
                     break;
                  case PowerDeviceD0:
                     Ezusb_KdPrint (("IRP_MN_SET_D0\n"));
                     break;
                } // switch on Power.State.DeviceState

                break; //DevicePowerState

         }// switch on Power.Type

         break;  //IRP_MN_SET_POWER

      case IRP_MN_QUERY_POWER:

         // Look at what type of power query this is
         switch (irpStack->Parameters.Power.Type)
         {
            case SystemPowerState:

               break; //SystemPowerState

            case DevicePowerState:
               switch (irpStack->Parameters.Power.State.DeviceState)
               {
                  case PowerDeviceD2:
                     Ezusb_KdPrint (("IRP_MN_QUERY_D2\n"));
                     break;
                  case PowerDeviceD1:
                     Ezusb_KdPrint (("IRP_MN_QUERY_D1\n"));
                     break;
                  case PowerDeviceD3:
                     Ezusb_KdPrint (("IRP_MN_QUERY_D3\n"));
                     break;
               } //switch on Power.State.DeviceState

               break; //DevicePowerState
                            
         }//switch on Power.Type

         break; //IRP_MN_QUERY_POWER

      default:
         // A PnP Minor Function was not handled
         Ezusb_KdPrint (("Power IOCTL not handled\n"));

   } /* switch MinorFunction*/


   nextStack = IoGetNextIrpStackLocation(Irp);
   ASSERT(nextStack != NULL);
   RtlCopyMemory(nextStack, irpStack, sizeof(IO_STACK_LOCATION));

   //
   // All PNP_POWER messages get passed to the StackDeviceObject that
   // we were given in PnPAddDevice.
   //
   // This stack device object is managed by the USB software subsystem,
   // and so this IRP must be propagated to the owning device driver for
   // that stack device object, so that driver in turn can perform any
   // device state management (e.g., remove its device object, etc.).
   //
   Ezusb_KdPrint (("Passing Power Irp down\n"));

   //
   // Notes on passing power IRPs down:  Using IoCallDriver() to pass
   // down power IRPs worked until Windows 2000.  Using this method
   // with Win2K causes a blue screen at system shutdown.  Because of this,
   // I am modifying the driver to use the more correct PoXXX() functions
   // to handle power IRPs. Unfortunately, the PoXXX() calls weren't
   // added until the kernel until after the release of Windows 95 OSR2.
   // So, a driver using these calls will not load on a Windows 95 system.
   // If you need to use this driver under Windows 95. then you must
   // #define WIN95.
   //

#ifdef WIN95
   ntStatus = IoCallDriver(pdx->StackDeviceObject, Irp);
#else
   PoStartNextPowerIrp(Irp);
   ntStatus = PoCallDriver(pdx->StackDeviceObject,Irp);
#endif

   //
   // If lower layer driver marked the Irp as pending then reflect that by
   // calling IoMarkIrpPending.
   //
   if (ntStatus == STATUS_PENDING)
   {
      IoMarkIrpPending(Irp);
      Ezusb_KdPrint (("Power Irp came back with STATUS_PENDING (%x)\n", ntStatus));
   }
   else
   {
      Ezusb_KdPrint (("Power Irp came back, status = %x\n", ntStatus));
   } // if ntStatus is PENDING

   goto Ezusb_Dispatch_Done;
            
Ezusb_Dispatch_Done:

   Ezusb_KdPrint (("Exit Ezusb_DispatchPower %x\n", ntStatus));
   return ntStatus;

}//Ezusb_DispatchPower


VOID
Ezusb_Unload(
    IN PDRIVER_OBJECT DriverObject
    )
/*++
Routine Description:
    Free all the allocated resources, etc.
    TODO: This is a placeholder for driver writer to add code on unload

Arguments:
    DriverObject - pointer to a driver object

Return Value:
    None
--*/
{
    Ezusb_KdPrint (("enter Ezusb_Unload\n"));
    /*
    // TODO: Free any global resources allocated in DriverEntry
    */
    Ezusb_KdPrint (("exit Ezusb_Unload\n"));
}

NTSTATUS
Ezusb_HandleRemoveDevice(
   IN PDEVICE_OBJECT fdo,
   IN PIRP Irp
   )
{
   NTSTATUS ntStatus;
   PDEVICE_EXTENSION pdx = (PDEVICE_EXTENSION) fdo->DeviceExtension;
   ULONG i;

   // set the removing flag to prevent any new I/O's
   pdx->removing = TRUE;

   // brute force - send an abort pipe message to all pipes to cancel any
   // pending transfers.  this should solve the problem of the driver blocking
   // on a REMOVE message because there is a pending transfer.
   for (i = 0; i < pdx->Interface->NumberOfPipes; i++)
   {
      Ezusb_AbortPipe(fdo,(USBD_PIPE_HANDLE) pdx->Interface->Pipes[i].PipeHandle);
   }

   UnlockDevice(fdo);			// once for LockDevice at start of dispatch
   UnlockDevice(fdo);			// once for initialization during AddDevice
   KeWaitForSingleObject(&pdx->evRemove, Executive, KernelMode, FALSE, NULL);

	Ezusb_RemoveDevice(fdo);

   ntStatus = Ezusb_DefaultPnpHandler(fdo, Irp);

   return ntStatus;				// lower-level completed IoStatus already

}


NTSTATUS
Ezusb_HandleStartDevice(
   IN PDEVICE_OBJECT fdo,
   IN PIRP Irp
   )
{
   NTSTATUS ntStatus;

   //
   // First let all lower-level drivers handle this request.
   //
   ntStatus = ForwardAndWait(fdo, Irp);
	if (!NT_SUCCESS(ntStatus))
		return CompleteRequest(Irp, ntStatus, Irp->IoStatus.Information);

   //
   // now do whatever we need to do to start the device
   //
   ntStatus = Ezusb_StartDevice(fdo);

	return CompleteRequest(Irp, ntStatus, 0);
}

NTSTATUS
Ezusb_StartDevice(
    IN  PDEVICE_OBJECT fdo
    )
/*++

Routine Description:
   Initializes a given instance of the Ezusb Device on the USB.

   Arguments:
      fdo - pointer to the device object for this instance of a
                      Ezusb Device

Return Value:
   NT status code
--*/
{
    PDEVICE_EXTENSION pdx;
    NTSTATUS ntStatus;
    PUSB_DEVICE_DESCRIPTOR deviceDescriptor = NULL;
    PURB urb;
    ULONG siz;

    Ezusb_KdPrint (("enter Ezusb_StartDevice\n"));

    pdx = fdo->DeviceExtension;
    pdx->NeedCleanup = TRUE;

    /*
    // Get some memory from then non paged pool (fixed, locked system memory)
    // for use by the USB Request Block (urb) for the specific USB Request we
    // will be performing below (a USB device request).
    */
    urb = ExAllocatePool( NonPagedPool,
                          sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));

    if (urb) {

        siz = sizeof(USB_DEVICE_DESCRIPTOR);

        // Get some non paged memory for the device descriptor contents
        deviceDescriptor = ExAllocatePool(NonPagedPool,
                                          siz);

        if (deviceDescriptor) {

            // Use a macro in the standard USB header files to build the URB
            UsbBuildGetDescriptorRequest(urb,
                                         (USHORT) sizeof (struct _URB_CONTROL_DESCRIPTOR_REQUEST),
                                         USB_DEVICE_DESCRIPTOR_TYPE,
                                         0,
                                         0,
                                         deviceDescriptor,
                                         NULL,
                                         siz,
                                         NULL);

            // Get the device descriptor
            ntStatus = Ezusb_CallUSBD(fdo, urb);

            // Dump out the descriptor info to the debugger
            if (NT_SUCCESS(ntStatus)) {
                Ezusb_KdPrint (("Device Descriptor = %x, len %x\n",
                                deviceDescriptor,
                                urb->UrbControlDescriptorRequest.TransferBufferLength));

                Ezusb_KdPrint (("Ezusb Device Descriptor:\n"));
                Ezusb_KdPrint (("-------------------------\n"));
                Ezusb_KdPrint (("bLength %d\n", deviceDescriptor->bLength));
                Ezusb_KdPrint (("bDescriptorType 0x%x\n", deviceDescriptor->bDescriptorType));
                Ezusb_KdPrint (("bcdUSB 0x%x\n", deviceDescriptor->bcdUSB));
                Ezusb_KdPrint (("bDeviceClass 0x%x\n", deviceDescriptor->bDeviceClass));
                Ezusb_KdPrint (("bDeviceSubClass 0x%x\n", deviceDescriptor->bDeviceSubClass));
                Ezusb_KdPrint (("bDeviceProtocol 0x%x\n", deviceDescriptor->bDeviceProtocol));
                Ezusb_KdPrint (("bMaxPacketSize0 0x%x\n", deviceDescriptor->bMaxPacketSize0));
                Ezusb_KdPrint (("idVendor 0x%x\n", deviceDescriptor->idVendor));
                Ezusb_KdPrint (("idProduct 0x%x\n", deviceDescriptor->idProduct));
                Ezusb_KdPrint (("bcdDevice 0x%x\n", deviceDescriptor->bcdDevice));
                Ezusb_KdPrint (("iManufacturer 0x%x\n", deviceDescriptor->iManufacturer));
                Ezusb_KdPrint (("iProduct 0x%x\n", deviceDescriptor->iProduct));
                Ezusb_KdPrint (("iSerialNumber 0x%x\n", deviceDescriptor->iSerialNumber));
                Ezusb_KdPrint (("bNumConfigurations 0x%x\n", deviceDescriptor->bNumConfigurations));
            }
        } else {
            ntStatus = STATUS_NO_MEMORY;
        }

        if (NT_SUCCESS(ntStatus)) {
            /*
            // Put a ptr to the device descriptor in the device extension for easy
            // access (like a "cached" copy).  We will free this memory when the
            // device is removed.  See the "Ezusb_RemoveDevice" code.
            */
            pdx->DeviceDescriptor = deviceDescriptor;
            pdx->Stopped = FALSE;
        } else if (deviceDescriptor) {
            /*
            // If the bus transaction failed, then free up the memory created to hold
            // the device descriptor, since the device is probably non-functional
            */
            ExFreePool(deviceDescriptor);
            pdx->DeviceDescriptor = NULL;
        }

        ExFreePool(urb);

    } else {
        // Failed getting memory for the Urb 
        ntStatus = STATUS_NO_MEMORY;
    }

    // If the Get_Descriptor call was successful, then configure the device.
    if (NT_SUCCESS(ntStatus)) {
        ntStatus = Ezusb_ConfigureDevice(fdo);
    }

#ifdef DOWNLOAD_KEIL_MONITOR
   //
   // download the Keil monitor
   //

   //
   // First download loader firmware.  The loader firmware implements a vendor
   // specific command that will allow us to anchor load to external ram
   //
   Ezusb_8051Reset(fdo,1);
   Ezusb_DownloadIntelHex(fdo,loader);
   Ezusb_8051Reset(fdo,0);

   //
   // Now download the Keil Monitor
   //
   if (IsFx2(fdo))
   {
      Ezusb_KdPrint (("**** Downloading FX2 monitor\n"));
      Ezusb_DownloadIntelHex(fdo,mon_ext_sio1_fx2);
   }
   else
   {
      Ezusb_KdPrint (("**** Downloading EZ-USB monitor\n"));
      Ezusb_DownloadIntelHex(fdo,mon_ext_sio1_ezusb);
   }

   Ezusb_8051Reset(fdo,1);
   Ezusb_8051Reset(fdo,0);

#endif // if DOWNLOAD_KEIL_MONITOR

    Ezusb_KdPrint (("exit Ezusb_StartDevice (%x)\n", ntStatus));

    return ntStatus;
}


NTSTATUS
Ezusb_RemoveDevice(
    IN  PDEVICE_OBJECT fdo
    )
/*++

Routine Description:
    Removes a given instance of a Ezusb Device device on the USB.

Arguments:
    fdo - pointer to the device object for this instance of a Ezusb Device

Return Value:
    NT status code

--*/
{
   PDEVICE_EXTENSION pdx;
   NTSTATUS ntStatus = STATUS_SUCCESS;

   Ezusb_KdPrint (("enter Ezusb_RemoveDevice\n"));

   pdx = fdo->DeviceExtension;

   if (pdx->DeviceDescriptor)
   {
      ExFreePool(pdx->DeviceDescriptor);
   }

   //
   // Free up any interface structures in our device extension
   //
   if (pdx->Interface != NULL)
   {
      ExFreePool(pdx->Interface);
   }

   //
   // remove the device object's symbolic link
   //
   if (pdx->NeedCleanup)
   {
      UNICODE_STRING deviceLinkUnicodeString;

      pdx->NeedCleanup = FALSE;

      RtlInitUnicodeString (&deviceLinkUnicodeString,
                           pdx->DeviceLinkNameBuffer);

      IoDeleteSymbolicLink(&deviceLinkUnicodeString);
    }

   IoDetachDevice(pdx->StackDeviceObject);

   IoDeleteDevice (fdo);

   Ezusb_KdPrint (("exit Ezusb_RemoveDevice (%x)\n", ntStatus));

   return ntStatus;
}


NTSTATUS
Ezusb_StopDevice(
   IN  PDEVICE_OBJECT fdo
   )
/*++
Routine Description:
   Stops a given instance of a Ezusb Device device on USB.

Arguments:
   fdo - pointer to the device object for this instance of a Ezusb Device

Return Value:
   NT status code

  --*/
{
   PDEVICE_EXTENSION pdx;
   NTSTATUS ntStatus = STATUS_SUCCESS;
   PURB urb;
   ULONG siz;

   Ezusb_KdPrint (("enter Ezusb_StopDevice\n"));

   pdx = fdo->DeviceExtension;

   //
   // Send the select configuration urb with a NULL pointer for the configuration
   // handle, this closes the configuration and puts the device in the 'unconfigured'
   // state.
   //

   siz = sizeof(struct _URB_SELECT_CONFIGURATION);

   urb = ExAllocatePool(NonPagedPool,
                      siz);

   if (urb)
   {
      NTSTATUS status;

      UsbBuildSelectConfigurationRequest(urb,
                                       (USHORT) siz,
                                       NULL);

      status = Ezusb_CallUSBD(fdo, urb);

      Ezusb_KdPrint (("Device Configuration Closed status = %x usb status = %x.\n",
                     status, urb->UrbHeader.Status));

      ExFreePool(urb);
   }
   else
   {
      ntStatus = STATUS_NO_MEMORY;
   }

   Ezusb_KdPrint (("exit Ezusb_StopDevice (%x)\n", ntStatus));

   return ntStatus;
}

NTSTATUS
Ezusb_PnPAddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject
    )
/*++
Routine Description:
    This routine is called to create a new instance of the device

Arguments:
    DriverObject - pointer to the driver object for this instance of Ezusb
    PhysicalDeviceObject - pointer to a device object created by the bus

Return Value:
    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise

--*/
{
   NTSTATUS                ntStatus = STATUS_SUCCESS;
   PDEVICE_OBJECT          deviceObject = NULL;
   PDEVICE_EXTENSION       pdx;
   int instance;

   Ezusb_KdPrint(("enter Ezusb_PnPAddDevice\n"));

#define MAX_EZUSB_DEVICES 8

   //
   // create our functional device object (FDO).  This driver supports multiple ezusb devices.
   // This loop will look for an available instance number.  Keep incrementing the instance
   // until a call to Ezusb_CreateDeviceObject succeeds.
   //
   instance = 0;
   do
   {
      ntStatus = Ezusb_CreateDeviceObject(DriverObject, &deviceObject, instance);
      instance++;
   } while (!NT_SUCCESS(ntStatus) && (instance < MAX_EZUSB_DEVICES));

   if (NT_SUCCESS(ntStatus))
   {
      pdx = deviceObject->DeviceExtension;

      //
      // Non plug and play drivers usually create the device object in
      // driver entry, and the I/O manager autimatically clears this flag.
      // Since we are creating the device object ourselves in response to 
      // a PnP START_DEVICE IRP, we need to clear this flag ourselves.
      //
      deviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

      //
      // This driver uses direct I/O for read/write requests
      //
      deviceObject->Flags |= DO_DIRECT_IO;

      deviceObject->Flags |= DO_POWER_PAGABLE;

      //
      //
      // store away the Physical device Object
      //
      pdx->PhysicalDeviceObject=PhysicalDeviceObject;

      //
      // Attach to the StackDeviceObject.  This is the device object that what we 
      // use to send Irps and Urbs down the USB software stack
      //
      pdx->StackDeviceObject =
         IoAttachDeviceToDeviceStack(deviceObject, PhysicalDeviceObject);

      ASSERT (pdx->StackDeviceObject != NULL);

      pdx->LastFailedUrbStatus = 0;

	   pdx->usage = 1;				// locked until RemoveDevice
	   KeInitializeEvent(&pdx->evRemove,
                        NotificationEvent,
                        FALSE);              // set when use count drops to zero
   }

   Ezusb_KdPrint(("exit Ezusb_PnPAddDevice (%x)\n", ntStatus));

   return ntStatus;
}


NTSTATUS
Ezusb_CreateDeviceObject(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT *DeviceObject,
    LONG Instance
    )
/*++

Routine Description:
    Creates a Functional DeviceObject

Arguments:
    DriverObject - pointer to the driver object for device
    DeviceObject - pointer to DeviceObject pointer to return
                   created device object.
    Instance - instnace of the device create.

Return Value:
    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise
--*/
{
   NTSTATUS ntStatus;
   WCHAR deviceLinkBuffer[]  = L"\\DosDevices\\Ezusb-0";
   UNICODE_STRING deviceLinkUnicodeString;
   WCHAR deviceNameBuffer[]  = L"\\Device\\Ezusb-0";
   UNICODE_STRING deviceNameUnicodeString;
   PDEVICE_EXTENSION pdx;
   STRING deviceName;

   Ezusb_KdPrint(("enter Ezusb_CreateDeviceObject instance = %d\n", Instance));

   //
   // fix up device names based on Instance
   //
   deviceLinkBuffer[18] = (USHORT) ('0' + Instance);
   deviceNameBuffer[14] = (USHORT) ('0' + Instance);

   Ezusb_KdPrint(("Create Device name (%ws)\n", deviceNameBuffer));

   RtlInitUnicodeString (&deviceNameUnicodeString,
                         deviceNameBuffer);

   //
   //Print out the unicode string
   //NOTE:  We must first convert the string to Unicode due to a bug in the Debugger that does not allow
   //       Unicode Strings to be printed to the debug device.
   //
   deviceName.Buffer = NULL;

   ntStatus = RtlUnicodeStringToAnsiString (&deviceName,
                                          &deviceNameUnicodeString, 
                                          TRUE);


   if (NT_SUCCESS(ntStatus))
   {
      Ezusb_KdPrint(("Create Device Name (%s)\n", deviceName.Buffer));
      RtlFreeAnsiString (&deviceName);
   }
   else
   {
      Ezusb_KdPrint(("Unicode to Ansi str failed w/ ntStatus: 0x%x\n",ntStatus));
   }

   ntStatus = IoCreateDevice (DriverObject,
                              sizeof (DEVICE_EXTENSION),
                              &deviceNameUnicodeString,
                              FILE_DEVICE_UNKNOWN,
                              0,
                              FALSE,
                              DeviceObject);


   if (NT_SUCCESS(ntStatus))
   {

      // Initialize our device extension
      pdx = (PDEVICE_EXTENSION) ((*DeviceObject)->DeviceExtension);

      RtlCopyMemory(pdx->DeviceLinkNameBuffer,
                   deviceLinkBuffer,
                   sizeof(deviceLinkBuffer));

      pdx->OpenHandles = 0;
      pdx->ConfigurationHandle = NULL;
      pdx->DeviceDescriptor = NULL;
      pdx->NeedCleanup = FALSE;
      pdx->DataRingBuffer = NULL;
      pdx->DescriptorRingBuffer = NULL;
      pdx->Started = FALSE;

      // Initialize our interface
      pdx->Interface = NULL;

      RtlInitUnicodeString (&deviceLinkUnicodeString,
                           deviceLinkBuffer);

      Ezusb_KdPrint(("Create DosDevice name (%ws)\n", deviceLinkBuffer));

      ntStatus = IoCreateSymbolicLink (&deviceLinkUnicodeString,
                                      &deviceNameUnicodeString);

   }

   Ezusb_KdPrint(("exit Ezusb_CreateDeviceObject (%x)\n", ntStatus));

   return ntStatus;
}

NTSTATUS
Ezusb_CallUSBD(
    IN PDEVICE_OBJECT fdo,
    IN PURB Urb
    )
/*++

Routine Description:
   Passes a Usb Request Block (URB) to the USB class driver (USBD)

   Note that we create our own IRP here and use it to send the request to
   the USB software subsystem.  This means that this routine is essentially
   independent of the IRP that caused this driver to be called in the first
   place.  The IRP for this transfer is created, used, and then destroyed
   in this routine.

   However, note that this routine uses the Usb Request Block (urb) passed
   in by the caller as the request block for the USB software stack.

   Implementation of this routine may be changed depending on the specific
   requirements of your driver.  For example, while this routine issues a
   synchronous request to the USB stack, you may wish to implement this as an
   asynchronous request in which you set an IoCompletionRoutine to be called
   when the request is complete.

Arguments:
   fdo - pointer to the device object for this instance of an Ezusb Device
   Urb          - pointer to Urb request block

Return Value:
   STATUS_SUCCESS if successful,
   STATUS_UNSUCCESSFUL otherwise

--*/
{
   NTSTATUS ntStatus, status = STATUS_SUCCESS;
   PDEVICE_EXTENSION pdx;
   PIRP irp;
   KEVENT event;
   IO_STATUS_BLOCK ioStatus;
   PIO_STACK_LOCATION nextStack;

   Ezusb_KdPrint (("enter Ezusb_CallUSBD\n"));

   pdx = fdo->DeviceExtension;

   // issue a synchronous request (see notes above)
   KeInitializeEvent(&event, NotificationEvent, FALSE);

   irp = IoBuildDeviceIoControlRequest(
             IOCTL_INTERNAL_USB_SUBMIT_URB,
             pdx->StackDeviceObject,
             NULL,
             0,
             NULL,
             0,
             TRUE, /* INTERNAL */
             &event,
             &ioStatus);

   // Prepare for calling the USB driver stack
   nextStack = IoGetNextIrpStackLocation(irp);
   ASSERT(nextStack != NULL);

   // Set up the URB ptr to pass to the USB driver stack
   nextStack->Parameters.Others.Argument1 = Urb;

   Ezusb_KdPrint (("Calling USB Driver Stack\n"));

   //
   // Call the USB class driver to perform the operation.  If the returned status
   // is PENDING, wait for the request to complete.
   //
   ntStatus = IoCallDriver(pdx->StackDeviceObject,
                         irp);

   Ezusb_KdPrint (("return from IoCallDriver USBD %x\n", ntStatus));

   if (ntStatus == STATUS_PENDING)
   {
      Ezusb_KdPrint (("Wait for single object\n"));

      status = KeWaitForSingleObject(
                    &event,
                    Suspended,
                    KernelMode,
                    FALSE,
                    NULL);

      Ezusb_KdPrint (("Wait for single object, returned %x\n", status));
   }
   else
   {
      ioStatus.Status = ntStatus;
   }

   Ezusb_KdPrint (("URB status = %x status = %x irp status %x\n",
     Urb->UrbHeader.Status, status, ioStatus.Status));

   //
   // USBD maps the error code for us.  USBD uses error codes in its URB
   // structure that are more insightful into USB behavior. In order to
   // match the NT Status codes, USBD maps its error codes into more general NT
   // error categories so higher level drivers can decipher the error codes
   // based on standard NT error code definitions.
   //
   ntStatus = ioStatus.Status;

   //
   // If the URB status was not USBD_STATUS_SUCCESS, we save a copy of the
   // URB status in the device extension.  After a failure, another IOCTL,
   // IOCTL_EZUSB_GET_LAST_ERROR can be used to retrieve the URB status
   // for the most recently failed URB.  Of course, this status gets
   // overwritten by subsequent failures, but it's better than nothing.
   //
   if (!(USBD_SUCCESS(Urb->UrbHeader.Status)))
      pdx->LastFailedUrbStatus = Urb->UrbHeader.Status;

   //
   // if ioStatus.Status indicates an error (ie. the IRP failed) then return that.
   // If ioStatus.Status indicates success, it is still possible that what we were
   // trying to do failed.  For example, if the IRP is cancelled, the status returned
   // by the I/O manager for the IRP will not indicate an error.  In that case, we
   // should check the URB status.  If it indicates anything other than
   // USBD_SUCCESS, then we should return STATUS_UNSUCCESSFUL.
   //
   if (NT_SUCCESS(ntStatus))
   {
      if (!(USBD_SUCCESS(Urb->UrbHeader.Status)))
         ntStatus = STATUS_UNSUCCESSFUL;
   }

   Ezusb_KdPrint(("exit Ezusb_CallUSBD (%x)\n", ntStatus));

   return ntStatus;
}

NTSTATUS
Ezusb_ConfigureDevice(
    IN  PDEVICE_OBJECT fdo
    )
/*++
Routine Description:
   Configures the USB device via USB-specific device requests and interaction
   with the USB software subsystem.

Arguments:
   fdo - pointer to the device object for this instance of the Ezusb Device

Return Value:
   NT status code
--*/
{
   PDEVICE_EXTENSION pdx;
   NTSTATUS ntStatus;
   PURB urb = NULL;
   ULONG siz;
   PUSB_CONFIGURATION_DESCRIPTOR configurationDescriptor = NULL;

   Ezusb_KdPrint (("enter Ezusb_ConfigureDevice\n"));

   pdx = fdo->DeviceExtension;

   //
   // Get memory for the USB Request Block (urb).
   //
   urb = ExAllocatePool(NonPagedPool,
                      sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));

   if (urb != NULL)
   {
      //
      // Set size of the data buffer.  Note we add padding to cover hardware faults
      // that may cause the device to go past the end of the data buffer
      //
        siz = sizeof(USB_CONFIGURATION_DESCRIPTOR) + 16;

        // Get the nonpaged pool memory for the data buffer
        configurationDescriptor = ExAllocatePool(NonPagedPool, siz);

        if (configurationDescriptor != NULL) {

            UsbBuildGetDescriptorRequest(urb,
                                         (USHORT) sizeof (struct _URB_CONTROL_DESCRIPTOR_REQUEST),
                                         USB_CONFIGURATION_DESCRIPTOR_TYPE,
                                         0,
                                         0,
                                         configurationDescriptor,
                                         NULL,
                                         sizeof (USB_CONFIGURATION_DESCRIPTOR),/* Get only the configuration descriptor */
                                         NULL);

            ntStatus = Ezusb_CallUSBD(fdo, urb);

            if (NT_SUCCESS(ntStatus)) {
                Ezusb_KdPrint (("Configuration Descriptor is at %x, bytes txferred: %d\n\
                                  Configuration Descriptor Actual Length: %d\n",
                                  configurationDescriptor,
                                  urb->UrbControlDescriptorRequest.TransferBufferLength,
                                  configurationDescriptor->wTotalLength));
            }//if

        } else {
            ntStatus = STATUS_NO_MEMORY;
            goto Exit_EzusbConfigureDevice;
        }//if-else

        // Determine how much data is in the entire configuration descriptor
        // and add extra room to protect against accidental overrun
        siz = configurationDescriptor->wTotalLength + 16;

        //  Free up the data buffer memory just used
        ExFreePool(configurationDescriptor);
        configurationDescriptor = NULL;

        // Get nonpaged pool memory for the data buffer
        configurationDescriptor = ExAllocatePool(NonPagedPool, siz);

        // Now get the entire Configuration Descriptor
        if (configurationDescriptor != NULL) {
            UsbBuildGetDescriptorRequest(urb,
                                         (USHORT) sizeof (struct _URB_CONTROL_DESCRIPTOR_REQUEST),
                                         USB_CONFIGURATION_DESCRIPTOR_TYPE,
                                         0,
                                         0,
                                         configurationDescriptor,
                                         NULL,
                                         siz,  // Get all the descriptor data
                                         NULL);

            ntStatus = Ezusb_CallUSBD(fdo, urb);

            if (NT_SUCCESS(ntStatus)) {
                Ezusb_KdPrint (("Entire Configuration Descriptor is at %x, bytes txferred: %d\n",
                                  configurationDescriptor,
                                  urb->UrbControlDescriptorRequest.TransferBufferLength));
            } else {
                //Error in getting configuration descriptor
                goto Exit_EzusbConfigureDevice;
            }//else

        } else {
            // Failed getting data buffer (configurationDescriptor) memory
            ntStatus = STATUS_NO_MEMORY;
            goto Exit_EzusbConfigureDevice;
        }//if-else

    } else {
        // failed getting urb memory
        ntStatus = STATUS_NO_MEMORY;
        goto Exit_EzusbConfigureDevice;
    }//if-else

    /*
    // We have the configuration descriptor for the configuration
    // we want.
    //
    // Now we issue the SelectConfiguration command to get
    // the  pipes associated with this configuration.
    */
    if (configurationDescriptor) {
        // Get our pipes
        ntStatus = Ezusb_SelectInterfaces(fdo,
                                           configurationDescriptor,
                                           NULL // Device not yet configured
                                           );
    } //if

Exit_EzusbConfigureDevice:

    // Clean up and exit this routine
    if (urb != NULL) {
        ExFreePool(urb);                    // Free urb memory
    }//if

    if (configurationDescriptor != NULL) {
        ExFreePool(configurationDescriptor);// Free data buffer
    }//if

    Ezusb_KdPrint (("exit Ezusb_ConfigureDevice (%x)\n", ntStatus));
    return ntStatus;
}//Ezusb_ConfigureDevice


NTSTATUS
Ezusb_SelectInterfaces(
    IN PDEVICE_OBJECT fdo,
    IN PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor,
    IN PUSBD_INTERFACE_INFORMATION Interface
    )
/*++

Routine Description:
    Initializes an Ezusb Device with multiple interfaces

Arguments:
    fdo            - pointer to the device object for this instance of the Ezusb Device
    ConfigurationDescriptor - pointer to the USB configuration descriptor containing the interface and endpoint
                              descriptors.
    Interface               - pointer to a USBD Interface Information Object
                            - If this is NULL, then this driver must choose its interface based on driver-specific
                              criteria, and the driver must also CONFIGURE the device.
                            - If it is NOT NULL, then the driver has already been given an interface and
                              the device has already been configured by the parent of this device driver.

Return Value:
    NT status code
--*/
{
   PDEVICE_EXTENSION pdx;
   NTSTATUS ntStatus;
   PURB urb;
   ULONG j;
   UCHAR alternateSetting, MyInterfaceNumber;
   PUSBD_INTERFACE_INFORMATION interfaceObject;
   USBD_INTERFACE_LIST_ENTRY interfaceList[2];

   Ezusb_KdPrint (("enter Ezusb_SelectInterfaces\n"));

   pdx = fdo->DeviceExtension;
   MyInterfaceNumber = SAMPLE_INTERFACE_NBR;

	// Search the configuration descriptor for the first interface/alternate setting

   interfaceList[0].InterfaceDescriptor =
   USBD_ParseConfigurationDescriptorEx(ConfigurationDescriptor,
                                       ConfigurationDescriptor,
                                       -1,         // Interface - don't care
                                       -1,         // Alternate Setting - don't care
                                       -1,         // Class - don't care
                                       -1,         // SubClass - don't care
                                       -1);        // Protocol - don't care

   ASSERT(interfaceList[0].InterfaceDescriptor != NULL);

   interfaceList[1].InterfaceDescriptor = NULL;
   interfaceList[1].Interface = NULL;

   urb = USBD_CreateConfigurationRequestEx(ConfigurationDescriptor,&interfaceList[0]);

   if (!urb)
   {
      Ezusb_KdPrint ((" USBD_CreateConfigurationRequestEx failed\n"));            
   }

   DumpBuffer(urb, urb->UrbHeader.Length);

   interfaceObject = (PUSBD_INTERFACE_INFORMATION) (&(urb->UrbSelectConfiguration.Interface));

   /*
   // We set up a default max transfer size for the endpoints.  Your driver will
   // need to change this to reflect the capabilities of your device's endpoints.
   */
   for (j=0; j<interfaceList[0].InterfaceDescriptor->bNumEndpoints; j++)
      interfaceObject->Pipes[j].MaximumTransferSize = (64 * 1024) - 1;


   ntStatus = Ezusb_CallUSBD(fdo, urb);

   DumpBuffer(urb, urb->UrbHeader.Length);

   if (NT_SUCCESS(ntStatus) && USBD_SUCCESS(urb->UrbHeader.Status))
   {

      // Save the configuration handle for this device
      pdx->ConfigurationHandle =
         urb->UrbSelectConfiguration.ConfigurationHandle;

      pdx->Interface = ExAllocatePool(NonPagedPool,
                                                interfaceObject->Length);

      // save a copy of the interfaceObject information returned
      RtlCopyMemory(pdx->Interface, interfaceObject, interfaceObject->Length);

      // Dump the interfaceObject to the debugger
      Ezusb_KdPrint (("---------\n"));
      Ezusb_KdPrint (("NumberOfPipes 0x%x\n", pdx->Interface->NumberOfPipes));
      Ezusb_KdPrint (("Length 0x%x\n", pdx->Interface->Length));
      Ezusb_KdPrint (("Alt Setting 0x%x\n", pdx->Interface->AlternateSetting));
      Ezusb_KdPrint (("Interface Number 0x%x\n", pdx->Interface->InterfaceNumber));

      // Dump the pipe info
      for (j=0; j<interfaceObject->NumberOfPipes; j++)
      {
         PUSBD_PIPE_INFORMATION pipeInformation;

         pipeInformation = &pdx->Interface->Pipes[j];

         Ezusb_KdPrint (("---------\n"));
         Ezusb_KdPrint (("PipeType 0x%x\n", pipeInformation->PipeType));
         Ezusb_KdPrint (("EndpointAddress 0x%x\n", pipeInformation->EndpointAddress));
         Ezusb_KdPrint (("MaxPacketSize 0x%x\n", pipeInformation->MaximumPacketSize));
         Ezusb_KdPrint (("Interval 0x%x\n", pipeInformation->Interval));
         Ezusb_KdPrint (("Handle 0x%x\n", pipeInformation->PipeHandle));
         Ezusb_KdPrint (("MaximumTransferSize 0x%x\n", pipeInformation->MaximumTransferSize));
      }

      Ezusb_KdPrint (("---------\n"));
   }


   Ezusb_KdPrint (("exit Ezusb_SelectInterfaces (%x)\n", ntStatus));

   return ntStatus;

}/* Ezusb_SelectInterfaces */


NTSTATUS
Ezusb_Read_Write(
   IN  PDEVICE_OBJECT fdo,
   IN  PIRP Irp
   )
/*++
Routine Description:
    
Arguments:

Return Value:
    NT status code
        STATUS_SUCCESS:                 Read was done successfully
        STATUS_INVALID_PARAMETER_3:     The Endpoint Index does not specify an IN pipe 
        STATUS_NO_MEMORY:               Insufficient data memory was supplied to perform the READ

    This routine fills the status code into the Irp
    
--*/
{
   PDEVICE_EXTENSION          pdx = fdo->DeviceExtension;
   NTSTATUS                   ntStatus;
   PIO_STACK_LOCATION         irpStack = IoGetCurrentIrpStackLocation (Irp);
   PBULK_TRANSFER_CONTROL     bulkControl =
                              (PBULK_TRANSFER_CONTROL)Irp->AssociatedIrp.SystemBuffer;
   ULONG                      bufferLength =
                              irpStack->Parameters.DeviceIoControl.OutputBufferLength;
   PURB                       urb = NULL;
   ULONG                      urbSize = 0;
   ULONG                      transferFlags = 0;
   PUSBD_INTERFACE_INFORMATION interfaceInfo = NULL;
   PUSBD_PIPE_INFORMATION     pipeInfo = NULL;
   USBD_PIPE_HANDLE           pipeHandle = NULL;


   Ezusb_KdPrint(("enter Ezusb_Read_Write()\n"));
   
   //
   // verify that the selected pipe is valid, and get a handle to it. If anything
   // is wrong, return an error
   //
   interfaceInfo = pdx->Interface;

   if (!interfaceInfo)
   {
      Ezusb_KdPrint(("Ezusb_Read_Write() no interface info - Exiting\n"));
      return STATUS_UNSUCCESSFUL;
   }
   
   if (bulkControl->pipeNum > interfaceInfo->NumberOfPipes)
   {
      Ezusb_KdPrint(("Ezusb_Read_Write() invalid pipe - Exiting\n"));
      return STATUS_INVALID_PARAMETER;
   }

   pipeInfo = &(interfaceInfo->Pipes[bulkControl->pipeNum]);

   if (!((pipeInfo->PipeType == UsbdPipeTypeBulk) ||
         (pipeInfo->PipeType == UsbdPipeTypeInterrupt)))
   {
      Ezusb_KdPrint(("Ezusb_Read_Write() invalid pipe - Exiting\n"));
      return STATUS_INVALID_PARAMETER;
   }

   pipeHandle = pipeInfo->PipeHandle;

   if (!pipeHandle)
   {
      Ezusb_KdPrint(("Ezusb_Read_Write() invalid pipe - Exiting\n"));
      return STATUS_UNSUCCESSFUL;
   }

   if (bufferLength > pipeInfo->MaximumTransferSize)
   {
      Ezusb_KdPrint(("Ezusb_Read_Write() invalid transfer size - Exiting\n"));
      return STATUS_INVALID_PARAMETER;
   }

   //
   // allocate and fill in the Usb request (URB)
   //
   urbSize = sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER);

   urb = ExAllocatePool(NonPagedPool,urbSize);

   if (!urb)
   {
      Ezusb_KdPrint(("Ezusb_Read_Write() unable to alloc URB - Exiting\n"));
      return STATUS_NO_MEMORY;
   }
   

   transferFlags = USBD_SHORT_TRANSFER_OK;

   //
   // get direction info from the endpoint address
   //
   if (USB_ENDPOINT_DIRECTION_IN(pipeInfo->EndpointAddress))
      transferFlags |= USBD_TRANSFER_DIRECTION_IN;

   UsbBuildInterruptOrBulkTransferRequest(urb,        //ptr to urb
                        (USHORT) urbSize,             //size of urb
							   pipeHandle,                   //usbd pipe handle
							   NULL,                         //TransferBuffer
							   Irp->MdlAddress,              //mdl
							   bufferLength,                 //bufferlength
							   transferFlags,                //flags
							   NULL);                        //link

   //
   // Call the USB Stack.
   //
	ntStatus = Ezusb_CallUSBD(fdo, urb);

   //
   // If the transfer was successful, report the length of the transfer to the
   // caller by setting IoStatus.Information
   //
   if (NT_SUCCESS(ntStatus))
   {
      Irp->IoStatus.Information = urb->UrbBulkOrInterruptTransfer.TransferBufferLength;
      Ezusb_KdPrint(("Successfully transfered 0x%x bytes\n",Irp->IoStatus.Information));
   }

   //
   // free the URB
   //
   ExFreePool(urb);

   return ntStatus;
}


NTSTATUS
Ezusb_Create(
    IN PDEVICE_OBJECT fdo,
    IN PIRP Irp
    )
/*++
Routine Description:
     This is the Entry point for CreateFile calls from user mode apps (apps may open "\\.\Ezusb-x\yyzz"
     where yy is the interface number and zz is the endpoint address).

     Here is where you would add code to create symbolic links between endpoints
     (i.e., pipes in USB software terminology) and User Mode file names.  You are
     free to use any convention you wish to create these links, although the above
     convention offers a way to identify resources on a device by familiar file and
     directory structure nomenclature.

Arguments:
    fdo - pointer to the device object for this instance of the Ezusb device

Return Value:
    NT status code
--*/
{
   NTSTATUS ntStatus;
   PDEVICE_EXTENSION pdx = (PDEVICE_EXTENSION )fdo->DeviceExtension;

   Ezusb_KdPrint(("Enter Ezusb_Create()\n"));

   if (!pdx->Started)
   {
      return STATUS_UNSUCCESSFUL;
   }

   // increment the open handle counter
   pdx->OpenHandles++;

   Irp->IoStatus.Status = STATUS_SUCCESS;
   Irp->IoStatus.Information = 0;

   // Create all the symbolic links here
   ntStatus = Irp->IoStatus.Status;

   IoCompleteRequest (Irp,
                    IO_NO_INCREMENT
                    );

   return ntStatus;

}//Ezusb_Create


NTSTATUS
Ezusb_Close(
    IN PDEVICE_OBJECT fdo,
    IN PIRP Irp
    )
/*++
Routine Description:
    Entry point for CloseHandle calls from user mode apps to close handles they have opened

Arguments:
    fdo - pointer to the device object for this instance of the Ezusb device
    Irp          - pointer to an irp

Return Value:
    NT status code

--*/
{
   NTSTATUS ntStatus;
   PDEVICE_EXTENSION pdx = (PDEVICE_EXTENSION )fdo->DeviceExtension;

   Ezusb_KdPrint(("Enter Ezusb_Close()\n"));

   // decrement the open handle counter
   pdx->OpenHandles--;

   Irp->IoStatus.Status = STATUS_SUCCESS;
   Irp->IoStatus.Information = 0;

   ntStatus = Irp->IoStatus.Status;

   IoCompleteRequest (Irp,
                    IO_NO_INCREMENT
                    );

   return ntStatus;

}//Ezusb_Close


NTSTATUS
Ezusb_ProcessIOCTL(
    IN PDEVICE_OBJECT fdo,
    IN PIRP Irp
    )
/*++

Routine Description:
   This where all the DeviceIoControl codes are handled.  You can add more code
   here to handle IOCTL codes that are specific to your device driver.

Arguments:
   fdo - pointer to the device object for this instance of the Ezusb device.

Return Value:
   NT status code
--*/
{
   PIO_STACK_LOCATION irpStack;
   PVOID ioBuffer;
   ULONG inputBufferLength;
   ULONG outputBufferLength;
   PDEVICE_EXTENSION pdx = (PDEVICE_EXTENSION )fdo->DeviceExtension;
   ULONG ioControlCode;
   NTSTATUS ntStatus;
   ULONG length;
   PUCHAR pch;

   Ezusb_KdPrint (("IRP_MJ_DEVICE_CONTROL\n"));

   if (!LockDevice(fdo))
      return CompleteRequest(Irp, STATUS_DELETE_PENDING, 0);

   //
   // Get a pointer to the current location in the Irp. This is where
   //     the function codes and parameters are located.
   //
   irpStack = IoGetCurrentIrpStackLocation (Irp);

   Irp->IoStatus.Status = STATUS_SUCCESS;
   Irp->IoStatus.Information = 0;

   ioBuffer           = Irp->AssociatedIrp.SystemBuffer;
   inputBufferLength  = irpStack->Parameters.DeviceIoControl.InputBufferLength;
   outputBufferLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;

   ioControlCode = irpStack->Parameters.DeviceIoControl.IoControlCode;

   //
   // Handle Ioctls from User mode
   //
   switch (ioControlCode)
   {

      case IOCTL_Ezusb_VENDOR_REQUEST:
         length = Ezusb_VendorRequest (fdo, (PVENDOR_REQUEST_IN) ioBuffer);

         if (length)
         {
            Irp->IoStatus.Information = length;
            Irp->IoStatus.Status = STATUS_SUCCESS;
         }
         else
         {
            Irp->IoStatus.Status = STATUS_SUCCESS;
         }
   
         Ezusb_KdPrint(("Vendor Request returned %d bytes\n", length));

         break;

      case IOCTL_Ezusb_ANCHOR_DOWNLOAD:
      {
#define CHUNKLENGTH 512

         PURB                urb             = NULL;
         int   chunkcount;
         int   chunklength = 1024;
         PVOID ioBufferPtr = NULL;
         int i;

         urb = ExAllocatePool(NonPagedPool, 
                             sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST));

         Ezusb_KdPrint (("IOCTL_Ezusb_ANCHOR_DOWNLOAD\n"));
                
         if (urb)
         {
            chunkcount = inputBufferLength / CHUNKLENGTH;
            ioBufferPtr = ioBuffer;

            for (i=0;i<chunkcount;i++)
            {
               RtlZeroMemory(urb,sizeof(struct  _URB_CONTROL_VENDOR_OR_CLASS_REQUEST));

               urb->UrbHeader.Length = sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);
               urb->UrbHeader.Function = URB_FUNCTION_VENDOR_DEVICE;

               Ezusb_KdPrint (("ioBuffer: %x | inputBufferLength: %d | outputBufferLength: %d\n",
                                 ioBuffer, inputBufferLength, outputBufferLength));

               urb->UrbControlVendorClassRequest.TransferBufferLength = CHUNKLENGTH;
               urb->UrbControlVendorClassRequest.TransferBuffer = ioBufferPtr;
               urb->UrbControlVendorClassRequest.TransferBufferMDL = NULL;
               urb->UrbControlVendorClassRequest.Request = 0xA0;
               urb->UrbControlVendorClassRequest.Value = (i * CHUNKLENGTH);
               urb->UrbControlVendorClassRequest.Index = 0;

               ntStatus = Ezusb_CallUSBD(fdo, urb);

               (char *) ioBufferPtr += CHUNKLENGTH;
            }

            ExFreePool(urb);
         }
      }

      break;

      case IOCTL_EZUSB_ANCHOR_DOWNLOAD:
      {
         PANCHOR_DOWNLOAD_CONTROL downloadControl = (PANCHOR_DOWNLOAD_CONTROL) ioBuffer;

         Ezusb_KdPrint (("IOCTL_EZUSB_ANCHOR_DOWNLOAD\n"));

         //
         // check the arguments
         //
         if (inputBufferLength != sizeof(ANCHOR_DOWNLOAD_CONTROL) ||
             outputBufferLength == 0)
         {
            Ezusb_KdPrint (("Error: Invalid Parameter\n"));
            ntStatus = STATUS_INVALID_PARAMETER;
         }
         else
         {
            ntStatus = Ezusb_AnchorDownload(fdo,
                                            downloadControl->Offset,
                                            (PUCHAR) MmGetSystemAddressForMdl(Irp->MdlAddress),
                                            outputBufferLength);
         }

         break;
      }
      case IOCTL_Ezusb_GET_CURRENT_CONFIG:

         Irp->IoStatus.Status = STATUS_SUCCESS;
         Irp->IoStatus.Information = 0;

         break;

      case IOCTL_EZUSB_GET_CURRENT_FRAME_NUMBER:
      {
         ULONG frameNumber = 0;

         //
         // make sure the output buffer is valid
         //
         if (outputBufferLength < sizeof(ULONG))
         {
            Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
            break;
         }
         
         frameNumber = Ezusb_GetCurrentFrameNumber (fdo);

         if (frameNumber)
         {
            *((PULONG)ioBuffer) = frameNumber;
            Irp->IoStatus.Information = sizeof(ULONG);
            Irp->IoStatus.Status = STATUS_SUCCESS;
         }
         else
            Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
      }

         break;

      case IOCTL_Ezusb_RESETPIPE:
      {
         ULONG pipenum = *((PULONG) ioBuffer);

         Irp->IoStatus.Status = Ezusb_ResetPipe(fdo,pipenum);
      }

      break;

      case IOCTL_Ezusb_ABORTPIPE:
      {
         int pipenum = *((PULONG) ioBuffer);

         Ezusb_AbortPipe(fdo,
                        (USBD_PIPE_HANDLE) pdx->Interface->Pipes[pipenum].PipeHandle);

         Irp->IoStatus.Information = 0;
         Irp->IoStatus.Status = STATUS_SUCCESS;
      }
      
         break;

      case IOCTL_Ezusb_GET_PIPE_INFO:
         //
         // inputs  - none
         // outputs - we copy the interface information structure that we have
         //           stored in our device extension area to the output buffer which
         //           will be reflected to the user mode application by the IOS.
         //
         length = 0;
         pch = (PUCHAR) ioBuffer;

         if (pdx->Interface)
         {
            RtlCopyMemory(pch+length,
                          (PUCHAR) pdx->Interface,
                          pdx->Interface->Length);

            length += pdx->Interface->Length;
         }


         Irp->IoStatus.Information = length;
         Irp->IoStatus.Status = STATUS_SUCCESS;

         break;

      case IOCTL_Ezusb_GET_DEVICE_DESCRIPTOR:
         //
         // inputs  - pointer to a buffer in which to place descriptor data
         // outputs - we put the device descriptor data, if any is returned by the device
         //           in the system buffer and then we set the length inthe Information field
         //           in the Irp, which will then cause the system to copy the buffer back
         //           to the user's buffer
         //
         length = Ezusb_GetDeviceDescriptor (fdo, ioBuffer);

         Ezusb_KdPrint(("Get Device Descriptor returned %d bytes\n", length));

         Irp->IoStatus.Information = length;
         Irp->IoStatus.Status = STATUS_SUCCESS;

         break;

      case IOCTL_Ezusb_GET_STRING_DESCRIPTOR:
         {
            PGET_STRING_DESCRIPTOR_IN Input = ioBuffer;

            if ((inputBufferLength = sizeof(GET_STRING_DESCRIPTOR_IN)) &&
                (outputBufferLength > 0))
            {
               length = Ezusb_GetStringDescriptor (fdo,
                                                   Input->Index,
                                                   Input->LanguageId,
                                                   ioBuffer,
                                                   outputBufferLength);

               if (length)
               {
                  Irp->IoStatus.Information = length;
                  Irp->IoStatus.Status = STATUS_SUCCESS;
               }
               else
               {
                  Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
               }
            }
            else
            {
               Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
            }
         }      
         break;

         case IOCTL_Ezusb_GET_CONFIGURATION_DESCRIPTOR:
            //
            // inputs  - pointer to a buffer in which to place descriptor data
            // outputs - we put the configuration descriptor data, if any is returned by the device
            //           in the system buffer and then we set the length in the Information field
            //           in the Irp, which will then cause the system to copy the buffer back
            //           to the user's buffer
            //
            length = Ezusb_GetConfigDescriptor (fdo, ioBuffer, outputBufferLength);

            Ezusb_KdPrint(("Get Config Descriptor returned %d bytes\n", length));

            Irp->IoStatus.Information = length;
            Irp->IoStatus.Status = STATUS_SUCCESS;

            break;

      case IOCTL_Ezusb_SETINTERFACE:
      {
         PSET_INTERFACE_IN input = ioBuffer;

         Irp->IoStatus.Status = SetInterface(fdo,
                                             input->interfaceNum,
                                             input->alternateSetting);
         Irp->IoStatus.Status = 0;
      }
      break;
    
      case IOCTL_Ezusb_RESET:

         Ezusb_ResetParentPort(fdo);

         break;

      case IOCTL_EZUSB_BULK_WRITE:
      case IOCTL_EZUSB_BULK_READ:

         Irp->IoStatus.Status = Ezusb_Read_Write(fdo,Irp);

         break;

      case IOCTL_EZUSB_VENDOR_OR_CLASS_REQUEST:

         Irp->IoStatus.Status = Ezusb_VendorRequest2(fdo,Irp);

         break;

      case IOCTL_EZUSB_GET_LAST_ERROR:

         //
         // make sure the output buffer is ok, and then copy the most recent
         // URB status from the device extension to it
         //
         if (outputBufferLength >= sizeof(ULONG))
         {
            *((PULONG)ioBuffer) = pdx->LastFailedUrbStatus;
            Irp->IoStatus.Status = STATUS_SUCCESS;
            Irp->IoStatus.Information = sizeof(ULONG);
         }
         else
         {
            Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
         }

         break;

         case IOCTL_EZUSB_ISO_READ:
         case IOCTL_EZUSB_ISO_WRITE:
            Irp->IoStatus.Status = Ezusb_StartIsoTransfer(fdo,Irp);
            Irp->IoStatus.Information = 0;
         break;

         case IOCTL_EZUSB_START_ISO_STREAM:
            Irp->IoStatus.Status = Ezusb_StartIsoStream(fdo,Irp);
            Irp->IoStatus.Information = 0;
         break;

         case IOCTL_EZUSB_STOP_ISO_STREAM:
            pdx->StopIsoStream = TRUE;
            Irp->IoStatus.Status = STATUS_SUCCESS;
            Irp->IoStatus.Information = 0;
         break;

         case IOCTL_EZUSB_READ_ISO_BUFFER:
         {
            ULONG dataBytesToRead;
            ULONG descriptorBytesToRead;
            ULONG dataBytesRead;
            ULONG descriptorBytesRead;

            PISO_TRANSFER_CONTROL isoControl =
               (PISO_TRANSFER_CONTROL)Irp->AssociatedIrp.SystemBuffer;

            //
            // verify the input and output params
            //
            if (inputBufferLength != sizeof(ISO_TRANSFER_CONTROL))
            {
               Irp->IoStatus.Information = 0; 
               Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
               break;
            }

            if (outputBufferLength !=
                isoControl->PacketCount * (isoControl->PacketSize + sizeof(USBD_ISO_PACKET_DESCRIPTOR)))
            {
               Irp->IoStatus.Information = 0; 
               Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
               break;
            }

            //
            // make sure the ring buffers exist
            //
            if (!(pdx->DataRingBuffer && pdx->DescriptorRingBuffer))
            {
               Irp->IoStatus.Information = 0; 
               Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
               break;
            }

            dataBytesToRead = isoControl->PacketCount * isoControl->PacketSize;
            dataBytesRead = ReadRingBuffer(pdx->DataRingBuffer,
                                           MmGetSystemAddressForMdl(Irp->MdlAddress),
                                           dataBytesToRead);
            Ezusb_KdPrint(("Copied %d bytes from the data ring buffer\n",dataBytesRead));

            descriptorBytesToRead = (dataBytesRead / isoControl->PacketSize) * sizeof(USBD_ISO_PACKET_DESCRIPTOR);
            descriptorBytesRead = ReadRingBuffer(pdx->DescriptorRingBuffer,
                                           ((PUCHAR) MmGetSystemAddressForMdl(Irp->MdlAddress)) + dataBytesRead,
                                           descriptorBytesToRead);
            Ezusb_KdPrint(("Copied %d bytes from the desc ring buffer\n",descriptorBytesRead));

            Irp->IoStatus.Information = dataBytesRead + descriptorBytesRead;
            Irp->IoStatus.Status = STATUS_SUCCESS;
         }
         break;

         case IOCTL_EZUSB_GET_DRIVER_VERSION:
         {
            PEZUSB_DRIVER_VERSION version = (PEZUSB_DRIVER_VERSION) ioBuffer;

            if (outputBufferLength >= sizeof(EZUSB_DRIVER_VERSION))
            {
               version->MajorVersion = EZUSB_MAJOR_VERSION;
               version->MinorVersion = EZUSB_MINOR_VERSION;
               version->BuildVersion = EZUSB_BUILD_VERSION;

               Irp->IoStatus.Status = STATUS_SUCCESS;
               Irp->IoStatus.Information = sizeof(EZUSB_DRIVER_VERSION);
            }
            else
            {
               Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
            }
         }
         break;

         case IOCTL_EZUSB_SET_FEATURE:
         {
            //
            // verify the input params
            //
            if (inputBufferLength != sizeof(SET_FEATURE_CONTROL))
            {
               Irp->IoStatus.Information = 0; 
               Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
               break;
            }

            Ezusb_SetFeature(fdo,(PSET_FEATURE_CONTROL)Irp->AssociatedIrp.SystemBuffer);
            Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
         }
         break;

      default:

      Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
   }

   UnlockDevice(fdo);

   ntStatus = Irp->IoStatus.Status;

   IoCompleteRequest (Irp,IO_NO_INCREMENT);

   return ntStatus;

}

ULONG
Ezusb_GetDescriptor(
    IN PDEVICE_OBJECT fdo,
    PVOID             DescriptorBuffer,
    ULONG             BufferLength,
    UCHAR             DescriptorType
    )
/*++
Routine Description:
    Gets a device descriptor from the given device object
    
Arguments:
    fdo - pointer to our device object
    DescriptorBuffer - buffer to accept the descriptor table
    BufferLength - size of the allocated Descriptor Buffer
    DescriptorType - Descriptor Type (per USB spec)

Return Value:
    Number of valid bytes in data buffer  

--*/
{
   NTSTATUS            ntStatus        = STATUS_SUCCESS;
   PURB                urb             = NULL;
   ULONG               length          = 0;
   PDEVICE_EXTENSION   pdx = NULL;

   Ezusb_KdPrint (("Enter Ezusb_GetDescriptor\n"));    

   pdx = fdo->DeviceExtension;

   urb = ExAllocatePool(NonPagedPool, 
                      sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));
                   
   if (urb)
   {
      if (DescriptorBuffer)
      {    
         UsbBuildGetDescriptorRequest(urb,
                                      (USHORT) sizeof (struct _URB_CONTROL_DESCRIPTOR_REQUEST),
                                      DescriptorType,                //descriptor type
                                      0,                             //index
                                      0,                             //language ID
                                      DescriptorBuffer,              //transfer buffer
                                      NULL,                          //MDL
                                      BufferLength,                  //buffer length
                                      NULL);                         //link
                                                                  
         ntStatus = Ezusb_CallUSBD(fdo, urb);

         // If successful, get the length from the Urb
         if (NT_SUCCESS(ntStatus))
         {
            length = urb->UrbControlDescriptorRequest.TransferBufferLength;
         }
      }
      else
      {
         ntStatus = STATUS_INVALID_PARAMETER;
      }    

      Ezusb_KdPrint (("%d bytes of descriptor received\n",length));
      ExFreePool(urb);
   }
   else
   {
      ntStatus = STATUS_NO_MEMORY;        
   }        

   // If unsuccessful, set the length to 0
   if (!(NT_SUCCESS(ntStatus)))
   {
      length = 0;
   }
    
   Ezusb_KdPrint (("Leaving Ezusb_GetDescriptor\n"));    

   return length;
}    

ULONG
Ezusb_GetDeviceDescriptor(
    IN PDEVICE_OBJECT fdo,
    PVOID             pvOutputBuffer
    )
/*++
Routine Description:
    Gets a device descriptor from the given device object
    
Arguments:
    fdo - pointer to the sample device object

Return Value:
    Number of valid bytes in data buffer  

--*/
{
   return (Ezusb_GetDescriptor(fdo,
                               pvOutputBuffer,
                               sizeof(USB_DEVICE_DESCRIPTOR),
                               USB_DEVICE_DESCRIPTOR_TYPE));
}    


ULONG
Ezusb_GetConfigDescriptor(
    IN PDEVICE_OBJECT fdo,
    PVOID             pvOutputBuffer,
    ULONG             ulLength
    )
/*++

Routine Description:
    Gets a configuration descriptor from the given device object
    
Arguments:
    fdo    - pointer to the sample device object
    pvOutputBuffer  - pointer to the buffer where the data is to be placed
    ulLength        - length of the buffer

Return Value:
    Number of valid bytes in data buffer  

--*/
{
   return (Ezusb_GetDescriptor(fdo,
                               pvOutputBuffer,
                               ulLength,
                               USB_CONFIGURATION_DESCRIPTOR_TYPE));
}    

ULONG
Ezusb_GetOtherSpeedConfigDescriptor(
    IN PDEVICE_OBJECT fdo,
    PVOID             pvOutputBuffer,
    ULONG             ulLength
    )
/*++

Routine Description:
    Gets a configuration descriptor from the given device object
    
Arguments:
    fdo    - pointer to the sample device object
    pvOutputBuffer  - pointer to the buffer where the data is to be placed
    ulLength        - length of the buffer

Return Value:
    Number of valid bytes in data buffer  

--*/
{
   return (Ezusb_GetDescriptor(fdo,
                               pvOutputBuffer,
                               ulLength,
                               7));
}    

ULONG
Ezusb_GetStringDescriptor(
    IN PDEVICE_OBJECT fdo,
    UCHAR             Index,
    USHORT            LanguageId,
    PVOID             pvOutputBuffer,
    ULONG             ulLength
    )
/*++

Routine Description:
    Gets the specified string descriptor from the given device object
    
Arguments:
    fdo    - pointer to the sample device object
    Index           - Index of the string descriptor
    LanguageId      - Language ID of the string descriptor
    pvOutputBuffer  - pointer to the buffer where the data is to be placed
    ulLength        - length of the buffer

Return Value:
    Number of valid bytes in data buffer  

--*/
{
   NTSTATUS            ntStatus        = STATUS_SUCCESS;
   PURB                urb             = NULL;
   ULONG               length          = 0;

   Ezusb_KdPrint (("Enter Ezusb_GetStringDescriptor\n"));    

   urb = ExAllocatePool(NonPagedPool, 
                      sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));
                         
   if (urb)
   {
      if (pvOutputBuffer)
      {    

         UsbBuildGetDescriptorRequest(urb,
                                      (USHORT) sizeof (struct _URB_CONTROL_DESCRIPTOR_REQUEST),
                                      USB_STRING_DESCRIPTOR_TYPE,    //descriptor type
                                      Index,                         //index
                                      LanguageId,                    //language ID
                                      pvOutputBuffer,                //transfer buffer
                                      NULL,                          //MDL
                                      ulLength,                      //buffer length
                                      NULL);                         //link
                                                                  
         ntStatus = Ezusb_CallUSBD(fdo, urb);

      }
      else
      {
         ntStatus = STATUS_INVALID_PARAMETER;
      }    

      // If successful, get the length from the Urb, otherwise set length to 0
      if (NT_SUCCESS(ntStatus))
      {
         length = urb->UrbControlDescriptorRequest.TransferBufferLength;
      }
      else
         length = 0;

      Ezusb_KdPrint (("%d bytes of string descriptor received\n",length));

      ExFreePool(urb);
        
   }
   else
   {
      ntStatus = STATUS_NO_MEMORY;        
   }        
    
   Ezusb_KdPrint (("Leaving Ezusb_GetStringDescriptor\n"));    

   return length;
}    

NTSTATUS
Ezusb_VendorRequest2(
   IN PDEVICE_OBJECT fdo,
   IN PIRP Irp
   )
{
   PDEVICE_EXTENSION          pdx = fdo->DeviceExtension;
   NTSTATUS                   ntStatus;
   PIO_STACK_LOCATION         irpStack = IoGetCurrentIrpStackLocation (Irp);
   PVENDOR_OR_CLASS_REQUEST_CONTROL  requestControl =
                              (PVENDOR_OR_CLASS_REQUEST_CONTROL) Irp->AssociatedIrp.SystemBuffer;
   ULONG                      bufferLength =
                              irpStack->Parameters.DeviceIoControl.OutputBufferLength;
   PURB                       urb = NULL;
   ULONG                      urbSize = 0;

   ULONG                      transferFlags = 0;
   USHORT                     urbFunction = 0;

   //
   // verify that the input parameter is correct (or at least that it's
   // the right size
   //
   if (irpStack->Parameters.DeviceIoControl.InputBufferLength !=
       sizeof(VENDOR_OR_CLASS_REQUEST_CONTROL))
   {
      ntStatus = STATUS_INVALID_PARAMETER;
      return ntStatus;
   }

   //
   // allocate and fill in the Usb request (URB)
   //
   urbSize = sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);

   urb = ExAllocatePool(NonPagedPool,urbSize);

   if (!urb)
   {
      return STATUS_NO_MEMORY;
   }

   RtlZeroMemory(urb,urbSize);

   transferFlags = USBD_SHORT_TRANSFER_OK;

   //
   // get direction info from the input parms
   //
   if (requestControl->direction)
      transferFlags |= USBD_TRANSFER_DIRECTION_IN;

   //
   // the type of request (class or vendor) and the recepient
   // (device, interface, endpoint, other) combine to determine the 
   // URB function.  The following ugly code transforms fields in
   // the input param into an URB function
   //
   switch ((requestControl->requestType << 2) | requestControl->recepient)
   {
      case 0x04:
         urbFunction = URB_FUNCTION_CLASS_DEVICE;
         break;
      case 0x05:
         urbFunction = URB_FUNCTION_CLASS_INTERFACE;
         break;
      case 0x06:
         urbFunction = URB_FUNCTION_CLASS_ENDPOINT;
         break;
      case 0x07:
         urbFunction = URB_FUNCTION_CLASS_OTHER;
         break;
      case 0x08:
         urbFunction = URB_FUNCTION_VENDOR_DEVICE;
         break;
      case 0x09:
         urbFunction = URB_FUNCTION_VENDOR_INTERFACE;
         break;
      case 0x0A:
         urbFunction = URB_FUNCTION_VENDOR_ENDPOINT;
         break;
      case 0x0B:
         urbFunction = URB_FUNCTION_VENDOR_OTHER;
         break;
      default:
         return STATUS_INVALID_PARAMETER;
   }

   urb->UrbHeader.Length = sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);
   urb->UrbHeader.Function = urbFunction;

   urb->UrbControlVendorClassRequest.TransferFlags = transferFlags;
   urb->UrbControlVendorClassRequest.TransferBufferLength = bufferLength;
   urb->UrbControlVendorClassRequest.TransferBufferMDL = Irp->MdlAddress;
   urb->UrbControlVendorClassRequest.Request = requestControl->request;
   urb->UrbControlVendorClassRequest.Value = requestControl->value;
   urb->UrbControlVendorClassRequest.Index = requestControl->index;

   //
   // Call the USB Stack.
   //
	ntStatus = Ezusb_CallUSBD(fdo, urb);

   //
   // If the transfer was successful, report the length of the transfer to the
   // caller by setting IoStatus.Information
   //
   if (NT_SUCCESS(ntStatus))
   {
      Irp->IoStatus.Information = urb->UrbControlVendorClassRequest.TransferBufferLength;
      Ezusb_KdPrint(("Successfully transfered 0x%x bytes\n",Irp->IoStatus.Information));
   }

   if (urb)
      ExFreePool(urb);

   return ntStatus;
}


ULONG
Ezusb_VendorRequest(
    IN PDEVICE_OBJECT fdo,
    IN PVENDOR_REQUEST_IN pVendorRequest
    )
{
    NTSTATUS            ntStatus        = STATUS_SUCCESS;
    PURB                urb             = NULL;
    ULONG               length          = 0;
    PUCHAR buffer = NULL;
    
    Ezusb_KdPrint (("Enter Ezusb_VendorRequest - yahoooo\n"));    

    urb = ExAllocatePool(NonPagedPool, 
                         sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST));
                         
    if (urb)
    {
        RtlZeroMemory(urb,sizeof(struct  _URB_CONTROL_VENDOR_OR_CLASS_REQUEST));

        //
        // fill in the URB
        //
        urb->UrbHeader.Length = sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);
        urb->UrbHeader.Function = URB_FUNCTION_VENDOR_DEVICE;

        urb->UrbControlVendorClassRequest.TransferBufferLength = pVendorRequest->wLength;
        urb->UrbControlVendorClassRequest.TransferBufferMDL = NULL;
        urb->UrbControlVendorClassRequest.Request = pVendorRequest->bRequest;
        urb->UrbControlVendorClassRequest.Value = pVendorRequest->wValue;
        urb->UrbControlVendorClassRequest.Index = pVendorRequest->wIndex;


        //
        // very kludgey.  The idea is: if its an IN then a buffer has been passed
        // in from user mode.  So, use the pointer to the system buffer as the transfer
        // buffer.  If the transfer is an out, then we need to allocate a transfer 
        // buffer.  If the length of the transfer is 1, then put pVendorRequest->bData
        // in the buffer.  Otherwise, fill the buffer with an incrementing byte pattern.
        // yuch
        //
        if (pVendorRequest->direction)
        {
            urb->UrbControlVendorClassRequest.TransferFlags |= USBD_TRANSFER_DIRECTION_IN;
            urb->UrbControlVendorClassRequest.TransferBuffer = pVendorRequest;

        }
        else
        {
            urb->UrbControlVendorClassRequest.TransferFlags = 0;
            buffer = ExAllocatePool(NonPagedPool, pVendorRequest->wLength);

            urb->UrbControlVendorClassRequest.TransferBuffer = buffer;

            if (pVendorRequest->wLength == 1)
            {
                buffer[0] = pVendorRequest->bData;
            }
            else
            {
                int i;
                PUCHAR ptr = buffer;

                for (i=0; i < pVendorRequest->wLength; i++)
                {
                    *ptr = (UCHAR) i;
                    ptr++;
                }
            }
        }

        ntStatus = Ezusb_CallUSBD(fdo, urb);

        //
        // only return a length if this was an IN transaction
        //
        if (pVendorRequest->direction)
        {
            length = urb->UrbControlVendorClassRequest.TransferBufferLength;
        }
        else
        {
            length = 0;
        }

        ExFreePool(urb);
        if (buffer)
            ExFreePool(buffer);
    }
    return length;
}

NTSTATUS
Ezusb_ResetPipe(
   IN PDEVICE_OBJECT fdo,
   ULONG PipeNum
   )
/*++

Routine Description:

   Reset a given USB pipe.
   NOTE: this will reset the data toggle on the host as well

Arguments:

Return Value:


--*/
{
   NTSTATUS ntStatus;
   PDEVICE_EXTENSION             pdx = fdo->DeviceExtension;
   PUSBD_INTERFACE_INFORMATION   interfaceInfo = NULL;
   USBD_PIPE_HANDLE              pipeHandle = NULL;
   PURB urb;
   USBD_VERSION_INFORMATION VersionInformation;

   Ezusb_KdPrint (("EZUSB.SYS: Reset Pipe \n"));

   //
   // verify that the selected pipe is valid, and get a handle to it. If anything
   // is wrong, return an error
   //
   interfaceInfo = pdx->Interface;

   if (!interfaceInfo)
   {
      Ezusb_KdPrint(("Ezusb_ResetPipe() no interface info - Exiting\n"));
      return STATUS_UNSUCCESSFUL;
   }
   
   if (PipeNum > interfaceInfo->NumberOfPipes)
   {
      Ezusb_KdPrint(("Ezusb_ResetPipe() invalid pipe - Exiting\n"));
      return STATUS_INVALID_PARAMETER;
   }

   pipeHandle = interfaceInfo->Pipes[PipeNum].PipeHandle;

   urb = ExAllocatePool(NonPagedPool,
                      sizeof(struct _URB_PIPE_REQUEST));

   if (urb)
   {
      urb->UrbHeader.Length = (USHORT) sizeof (struct _URB_PIPE_REQUEST);
      urb->UrbHeader.Function = URB_FUNCTION_RESET_PIPE;
      urb->UrbPipeRequest.PipeHandle = pipeHandle;

      //
      // kludge.  Win98 changed the size of the URB_PIPE_REQUEST.
      // Check the USBDI version.  If it is prior to win98 (0x101)
      // make the structure smaller.
      // 
      USBD_GetUSBDIVersion(&VersionInformation);
      if (VersionInformation.USBDI_Version < 0x101) 
      {
         Ezusb_KdPrint(("Ezusb_ResetPipe() Detected OSR2.1\n"));
         urb->UrbHeader.Length -= sizeof(ULONG);
      }

      ntStatus = Ezusb_CallUSBD(fdo, urb);

      ExFreePool(urb);
   }
   else
   {
      ntStatus = STATUS_INSUFFICIENT_RESOURCES;
   }

   return ntStatus;
}

NTSTATUS
Ezusb_AbortPipe(
    IN PDEVICE_OBJECT fdo,
    IN USBD_PIPE_HANDLE PipeHandle
    )
/*++

Routine Description:

   cancel pending transfers for a pipe

Arguments:

Return Value:


--*/
{
   NTSTATUS ntStatus;
   PURB urb;
   USBD_VERSION_INFORMATION VersionInformation;

   Ezusb_KdPrint (("'EZUSB.SYS: Reset Pipe \n"));

   urb = ExAllocatePool(NonPagedPool,
                      sizeof(struct _URB_PIPE_REQUEST));

   if (urb)
   {
      RtlZeroMemory(urb,sizeof(struct _URB_PIPE_REQUEST));
      urb->UrbHeader.Length = (USHORT) sizeof (struct _URB_PIPE_REQUEST);
      urb->UrbHeader.Function = URB_FUNCTION_ABORT_PIPE;
      urb->UrbPipeRequest.PipeHandle = PipeHandle;

      //
      // kludge.  Win98 changed the size of the URB_PIPE_REQUEST.
      // Check the USBDI version.  If it is prior to win98 (0x101)
      // make the structure smaller.
      // 
      USBD_GetUSBDIVersion(&VersionInformation);
      if (VersionInformation.USBDI_Version < 0x101) 
      {
         Ezusb_KdPrint(("Ezusb_ResetPipe() Detected OSR2.1\n"));
         urb->UrbHeader.Length -= sizeof(ULONG);
      }

      ntStatus = Ezusb_CallUSBD(fdo, urb);

      ExFreePool(urb);
   }
   else
   {
      ntStatus = STATUS_INSUFFICIENT_RESOURCES;
   }


   return ntStatus;
}


ULONG
Ezusb_GetCurrentFrameNumber(
    IN PDEVICE_OBJECT fdo
    )
{
   PURB     urb = NULL;
   PDEVICE_EXTENSION   pdx = NULL;
   NTSTATUS            ntStatus        = STATUS_SUCCESS;
   ULONG               frameNumber = 0;
    
   Ezusb_KdPrint (("Enter Ezusb_GetCurrentFrameNumber\n"));    

   pdx = fdo->DeviceExtension;

   urb = ExAllocatePool(NonPagedPool,sizeof(struct _URB_GET_CURRENT_FRAME_NUMBER));

   if (urb == NULL)
      return 0;

   RtlZeroMemory(urb,sizeof(struct _URB_GET_CURRENT_FRAME_NUMBER));

   urb->UrbHeader.Length = sizeof(struct _URB_GET_CURRENT_FRAME_NUMBER);
   urb->UrbHeader.Function = URB_FUNCTION_GET_CURRENT_FRAME_NUMBER;

   ntStatus = Ezusb_CallUSBD(fdo, urb);

   if (NT_SUCCESS(ntStatus))
   {
      frameNumber = urb->UrbGetCurrentFrameNumber.FrameNumber;
   }

   ExFreePool(urb);

   return frameNumber;
}

NTSTATUS
Ezusb_ResetParentPort(
    IN IN PDEVICE_OBJECT fdo
    )
/*++

Routine Description:

    Reset the our parent port

Arguments:

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise

--*/
{
    NTSTATUS ntStatus, status = STATUS_SUCCESS;
    PIRP irp;
    KEVENT event;
    IO_STATUS_BLOCK ioStatus;
    PIO_STACK_LOCATION nextStack;
    PDEVICE_EXTENSION pdx;

    Ezusb_KdPrint (("EZUSB.SYS: enter Ezusb_ResetPort\n"));

    pdx = fdo->DeviceExtension;

    //
    // issue a synchronous request
    //

    KeInitializeEvent(&event, NotificationEvent, FALSE);

    irp = IoBuildDeviceIoControlRequest(
                IOCTL_INTERNAL_USB_RESET_PORT,
                pdx->StackDeviceObject,
//                pdx->TopOfStackDeviceObject,
                NULL,
                0,
                NULL,
                0,
                TRUE, /* INTERNAL */
                &event,
                &ioStatus);

    //
    // Call the class driver to perform the operation.  If the returned status
    // is PENDING, wait for the request to complete.
    //

    nextStack = IoGetNextIrpStackLocation(irp);
    ASSERT(nextStack != NULL);

    Ezusb_KdPrint (("EZUSB.SYS: calling USBD enable port api\n"));

    ntStatus = IoCallDriver(pdx->StackDeviceObject,
                            irp);
                            
    Ezusb_KdPrint (("EZUSB.SYS: return from IoCallDriver USBD %x\n", ntStatus));

    if (ntStatus == STATUS_PENDING) {

        Ezusb_KdPrint (("EZUSB.SYS: Wait for single object\n"));

        status = KeWaitForSingleObject(
                       &event,
                       Suspended,
                       KernelMode,
                       FALSE,
                       NULL);

        Ezusb_KdPrint (("EZUSB.SYS: Wait for single object, returned %x\n", status));
        
    } else {
        ioStatus.Status = ntStatus;
    }

    //
    // USBD maps the error code for us
    //
    ntStatus = ioStatus.Status;

    Ezusb_KdPrint (("EZUSB.SYS: Ezusb_ResetPort (%x)\n", ntStatus));

    return ntStatus;
}

ULONG
Ezusb_DownloadTest(
    IN PDEVICE_OBJECT fdo,
    IN PVENDOR_REQUEST_IN pVendorRequest
    )
{
    NTSTATUS            ntStatus        = STATUS_SUCCESS;
    PURB                urb             = NULL;
    ULONG               length          = 0;
    ULONG i;
    PUCHAR  buffer1 = NULL;
    PUCHAR  buffer2 = NULL;
    ULONG CompareCount = 0;
    
    Ezusb_KdPrint (("Enter Ezusb_VendorRequest - yahoooo\n"));    

    urb = ExAllocatePool(NonPagedPool, 
                         sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST));

    buffer1 = ExAllocatePool(NonPagedPool, CHUNKLENGTH);
    buffer2 = ExAllocatePool(NonPagedPool, CHUNKLENGTH);

    for (i=0; i < CHUNKLENGTH; i++)
    {
        buffer1[i] = (UCHAR) i;
    }

    if (urb)
    {
        for (i=0; i < 5120 / CHUNKLENGTH; i++)
        {
            RtlZeroMemory(urb,sizeof(struct  _URB_CONTROL_VENDOR_OR_CLASS_REQUEST));

            //
            // fill in the URB
            //
            urb->UrbHeader.Length = sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);
            urb->UrbHeader.Function = URB_FUNCTION_VENDOR_DEVICE;

            urb->UrbControlVendorClassRequest.TransferBufferLength = CHUNKLENGTH;
            urb->UrbControlVendorClassRequest.TransferBuffer = buffer1;
            urb->UrbControlVendorClassRequest.TransferBufferMDL = NULL;
            urb->UrbControlVendorClassRequest.Request = 0xA0;
            urb->UrbControlVendorClassRequest.Value = (unsigned short) i * CHUNKLENGTH;
            urb->UrbControlVendorClassRequest.Index = 0;

            ntStatus = Ezusb_CallUSBD(fdo, urb);

        }

        for (i=0; i < 5120 / CHUNKLENGTH; i++)
        {
            RtlZeroMemory(urb,sizeof(struct  _URB_CONTROL_VENDOR_OR_CLASS_REQUEST));
            RtlZeroMemory(buffer2, CHUNKLENGTH);

            //
            // fill in the URB
            //
            urb->UrbHeader.Length = sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);
            urb->UrbHeader.Function = URB_FUNCTION_VENDOR_DEVICE;

            urb->UrbControlVendorClassRequest.TransferBufferLength = CHUNKLENGTH;
            urb->UrbControlVendorClassRequest.TransferBuffer = buffer2;
            urb->UrbControlVendorClassRequest.TransferBufferMDL = NULL;
            urb->UrbControlVendorClassRequest.Request = 0xA0;
            urb->UrbControlVendorClassRequest.Value = (unsigned short) i * CHUNKLENGTH;
            urb->UrbControlVendorClassRequest.Index = 0;
            urb->UrbControlVendorClassRequest.TransferFlags |= USBD_TRANSFER_DIRECTION_IN;

            ntStatus = Ezusb_CallUSBD(fdo, urb);

            CompareCount = RtlCompareMemory(buffer1,buffer2,CHUNKLENGTH);
            Ezusb_KdPrint (("%d matched\n",CompareCount))
            if (CompareCount != CHUNKLENGTH)
            {
                Ezusb_KdPrint (("**** Compare Error *****************************************************\n"));
            }


        }

    }
    return length;
}


PUSB_CONFIGURATION_DESCRIPTOR
GetConfigDescriptor(
    IN PDEVICE_OBJECT fdo
    )
/*++

Routine Description:

Arguments:



Return Value:

    NT status code

--*/
{
    PDEVICE_EXTENSION pdx;
    NTSTATUS ntStatus;
    PURB urb;
    ULONG siz;
    PUSB_CONFIGURATION_DESCRIPTOR configurationDescriptor = NULL;

    Ezusb_KdPrint (("Ezusb.SYS: enter Ezusb_GetConfigDescriptor\n"));

    pdx = fdo->DeviceExtension;

    urb = ExAllocatePool(NonPagedPool,
                         sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));

    if (urb) {


        siz = sizeof(USB_CONFIGURATION_DESCRIPTOR)+256;

get_config_descriptor_retry2:

        configurationDescriptor = ExAllocatePool(NonPagedPool,
                                                 siz);

        if (configurationDescriptor) {

            UsbBuildGetDescriptorRequest(urb,
                                         (USHORT) sizeof (struct _URB_CONTROL_DESCRIPTOR_REQUEST),
                                         USB_CONFIGURATION_DESCRIPTOR_TYPE,
                                         0,
                                         0,
                                         configurationDescriptor,
                                         NULL,
                                         siz,
                                         NULL);

            ntStatus = Ezusb_CallUSBD(fdo, urb);

            Ezusb_KdPrint (("Ezusb.SYS: Configuration Descriptor = %x, len %x\n",
                            configurationDescriptor,
                            urb->UrbControlDescriptorRequest.TransferBufferLength));
        } else {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        }

        if (NT_SUCCESS(ntStatus) &&
            (urb->UrbControlDescriptorRequest.TransferBufferLength >= 
             sizeof(USB_CONFIGURATION_DESCRIPTOR)) &&
            (configurationDescriptor->wTotalLength >=
             sizeof(USB_CONFIGURATION_DESCRIPTOR)))
        {
            //
            // The Get Config Descriptor request did not return an error
            // AND at least enough data was transferred to fill a Config
            // Descriptor AND the Config Descriptor wLength is at least the
            // size of a Config Descriptor
            //
            if (configurationDescriptor->wTotalLength > siz)
            {
                //
                // The request buffer is not big enough to hold the
                // entire set of descriptors.  Free the current buffer
                // and retry with a buffer which should be big enough.
                //
                siz = configurationDescriptor->wTotalLength;
                ExFreePool(configurationDescriptor);
                configurationDescriptor = NULL;
                goto get_config_descriptor_retry2;
            }
            else if (configurationDescriptor->wTotalLength >
                     urb->UrbControlDescriptorRequest.TransferBufferLength)
            {
                //
                // The request buffer is greater than or equal to the
                // Config Descriptor wLength, but less data was transferred
                // than wLength.  Return NULL to indicate a device error.
                //
                ExFreePool(configurationDescriptor);
                configurationDescriptor = NULL;
            }
            //
            // else  everything is OK with the Config Descriptor, return it.
            //
        }
        else
        {
            //
            // The Get Config Descriptor request returned an error OR
            // not enough data was transferred to fill a Config Descriptor
            // OR the Config Descriptor wLength is less than the size of
            // a Config Descriptor.  Return NULL to indicate a device error.
            //
            ExFreePool(configurationDescriptor);
            configurationDescriptor = NULL;
        }

        ExFreePool(urb);

    } else {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    Ezusb_KdPrint (("Ezusb.SYS: exit Ezusb_GetConfigDescriptor\n"));

    return configurationDescriptor;
}

NTSTATUS
ConfigureDevice(
    IN  PDEVICE_OBJECT fdo
    )
/*++
Routine Description:
    Configures the USB device via USB-specific device requests and interaction
    with the USB software subsystem.

Arguments:
    fdo - pointer to the device object for this instance of the Ezusb Device

Return Value:
    NT status code
--*/
{
   PDEVICE_EXTENSION pdx = fdo->DeviceExtension;
   NTSTATUS ntStatus;
   PURB urb = NULL;
   ULONG numberOfPipes;
   PUSB_CONFIGURATION_DESCRIPTOR configurationDescriptor = NULL;
   PUSB_INTERFACE_DESCRIPTOR interfaceDescriptor = NULL;

   Ezusb_KdPrint (("enter ConfigureDevice\n"));

   //
   // Get the configuration Descriptor
   //
   configurationDescriptor = GetConfigDescriptor(fdo);

   if (!configurationDescriptor)
   {
      Ezusb_KdPrint (("Get Configuration Descriptor Failed\n"));
      ntStatus = STATUS_UNSUCCESSFUL;
      goto CleanupConfigureDevice;
   }

#define INTERFACENUMBER 0
#define ALTERNATESETTING 1
   //
   // Get the interface Descriptor for the interface we want
   //
   interfaceDescriptor = USBD_ParseConfigurationDescriptorEx(
                           configurationDescriptor,
                           configurationDescriptor,
                           INTERFACENUMBER,
                           ALTERNATESETTING,
                           -1,
                           -1,
                           -1);
   if (!interfaceDescriptor)
   {
      ntStatus = STATUS_UNSUCCESSFUL;
      goto CleanupConfigureDevice;
   }

   numberOfPipes = interfaceDescriptor->bNumEndpoints;
   
   //
   // Configure the Device, but don't select any interfaces
   //
   urb = USBD_CreateConfigurationRequestEx(configurationDescriptor, NULL);

   if (!urb)
   {
      Ezusb_KdPrint (("USBD_CreateConfigurationRequestEx Failed\n"));
      ntStatus = STATUS_UNSUCCESSFUL;
      goto CleanupConfigureDevice;
   }

   ntStatus = Ezusb_CallUSBD(fdo, urb);

   if (NT_SUCCESS(ntStatus))
   {
      pdx->ConfigurationHandle = urb->UrbSelectConfiguration.ConfigurationHandle;
   }
   else
   {
      Ezusb_KdPrint (("Configuration Request Failed\n"));
      ntStatus = STATUS_UNSUCCESSFUL;
      goto CleanupConfigureDevice;
   }

CleanupConfigureDevice:

   // Clean up and exit this routine
   if (urb != NULL)
   {
      ExFreePool(urb);
   }

   if (configurationDescriptor != NULL)
   {
      ExFreePool(configurationDescriptor);
   }

    Ezusb_KdPrint (("exit Ezusb_ConfigureDevice (%x)\n", ntStatus));
    return ntStatus;
}


NTSTATUS
SetInterface(
   IN PDEVICE_OBJECT fdo,
   IN UCHAR InterfaceNumber,
   IN UCHAR AlternateSetting
   )
{
   PUSBD_INTERFACE_INFORMATION interfaceInformation = NULL;
   PUSB_INTERFACE_DESCRIPTOR interfaceDescriptor = NULL;
   PUSB_CONFIGURATION_DESCRIPTOR configurationDescriptor = NULL;
   PDEVICE_EXTENSION pdx = fdo->DeviceExtension;
   PURB urb = NULL;
   USHORT urbSize;
   ULONG numberOfPipes;
   ULONG i,j;
   NTSTATUS ntStatus;

   //
   // Get the configuration Descriptor
   //
   configurationDescriptor = GetConfigDescriptor(fdo);
   if (!configurationDescriptor)
   {
      Ezusb_KdPrint (("Get Configuration Descriptor Failed\n"));
      ntStatus = STATUS_UNSUCCESSFUL;
      goto CleanupSetInterface;
   }

   //
   // Get the interface Descriptor for the interface we want
   //
   interfaceDescriptor = USBD_ParseConfigurationDescriptorEx(
                           configurationDescriptor,
                           configurationDescriptor,
                           InterfaceNumber,
                           AlternateSetting,
                           -1,
                           -1,
                           -1);

   if (!interfaceDescriptor)
   {
      ntStatus = STATUS_UNSUCCESSFUL;
      goto CleanupSetInterface;
   }

   numberOfPipes = interfaceDescriptor->bNumEndpoints;
   Ezusb_KdPrint (("numberOfPipes = %d\n", numberOfPipes));


   urbSize = GET_SELECT_INTERFACE_REQUEST_SIZE(numberOfPipes);
   Ezusb_KdPrint (("urbSize = %d\n", urbSize));
   urb = ExAllocatePool(NonPagedPool,urbSize);

   if (!urb)
   {
      ntStatus = STATUS_NO_MEMORY;
      goto CleanupSetInterface;
   }
   
   RtlZeroMemory(urb,urbSize);

   UsbBuildSelectInterfaceRequest(urb,
                                  urbSize,
                                  pdx->ConfigurationHandle,
                                  InterfaceNumber,
                                  AlternateSetting);


   interfaceInformation = &(urb->UrbSelectInterface.Interface);
   interfaceInformation->Length = GET_USBD_INTERFACE_SIZE(numberOfPipes);

   for (i = 0 ;i < numberOfPipes ;i++ )
   {
      interfaceInformation->Pipes[i].MaximumTransferSize = (64*1024) -1;
   }

   ntStatus = Ezusb_CallUSBD(fdo, urb);

   //
   // If that succeeded, then update the Interface structure
   // in the device extension.
   //
   if (NT_SUCCESS(ntStatus))
   {
      for (j=0; j<interfaceInformation->NumberOfPipes; j++)
      {
         PUSBD_PIPE_INFORMATION pipeInformation;

         pipeInformation = &interfaceInformation->Pipes[j];

         Ezusb_KdPrint (("---------\n"));
         Ezusb_KdPrint (("PipeType 0x%x\n", pipeInformation->PipeType));
         Ezusb_KdPrint (("EndpointAddress 0x%x\n", pipeInformation->EndpointAddress));
         Ezusb_KdPrint (("MaxPacketSize 0x%x\n", pipeInformation->MaximumPacketSize));
         Ezusb_KdPrint (("Interval 0x%x\n", pipeInformation->Interval));
         Ezusb_KdPrint (("Handle 0x%x\n", pipeInformation->PipeHandle));
         Ezusb_KdPrint (("MaximumTransferSize 0x%x\n", pipeInformation->MaximumTransferSize));
      }

      if (pdx->Interface)
      {
         ExFreePool(pdx->Interface);
      }

      pdx->Interface = NULL;
      pdx->Interface = ExAllocatePool(NonPagedPool,interfaceInformation->Length);
      if (!pdx->Interface)
      {
         ntStatus = STATUS_NO_MEMORY;
         goto CleanupSetInterface;
      }
      RtlCopyMemory(pdx->Interface, interfaceInformation, interfaceInformation->Length);
   }

CleanupSetInterface:

   // Clean up and exit this routine
   if (urb != NULL)
   {
      ExFreePool(urb);
   }

   if (configurationDescriptor != NULL)
   {
      ExFreePool(configurationDescriptor);
   }

   return(ntStatus);
   
}   

#define BYTES_PER_LINE 0x10

void
DumpBuffer(PVOID pvBuffer, ULONG length)
{
    int                    nItems    = 0;
    char                   temp[64]  = "";
    char                   temp2[64]  = "";
	ULONG	i;
	ULONG   j;
	PUCHAR	ptr;


	ptr = (PUCHAR) pvBuffer;

	for (i = 0; i < ((length + BYTES_PER_LINE - 1) / BYTES_PER_LINE); i++)
	{
		sprintf(temp,"%04X ",(i*BYTES_PER_LINE));
		for (j = 0; j < BYTES_PER_LINE; j++)
		{
			if (((i * BYTES_PER_LINE) + j) < length)
			{
				sprintf(temp2,"%02X ",*ptr++);
				strcat(temp,temp2);
			}
		}
//	    SendMessage (hOutputBox, LB_ADDSTRING, 0, (LPARAM)temp);
         Ezusb_KdPrint (("%s\n",temp));
	}
}



///////////////////////////////////////////////////////////////////////////////
// @func Lock a SIMPLE device object
// @parm Address of our device extension
// @rdesc TRUE if it was possible to lock the device, FALSE otherwise.
// @comm A FALSE return value indicates that we're in the process of deleting
// the device object, so all new requests should be failed

BOOLEAN LockDevice(
   IN PDEVICE_OBJECT fdo
   )
{
   PDEVICE_EXTENSION pdx = (PDEVICE_EXTENSION) fdo->DeviceExtension;

   // Increment use count on our device object
   LONG usage = InterlockedIncrement(&pdx->usage);

   // AddDevice initialized the use count to 1, so it ought to be bigger than
   // one now. HandleRemoveDevice sets the "removing" flag and decrements the
   // use count, possibly to zero. So if we find a use count of "1" now, we
   // should also find the "removing" flag set.

   ASSERT(usage > 1 || pdx->removing);

   // If device is about to be removed, restore the use count and return FALSE.
   // If we're in a race with HandleRemoveDevice (maybe running on another CPU),
   // the sequence we've followed is guaranteed to avoid a mistaken deletion of
   // the device object. If we test "removing" after HandleRemoveDevice sets it,
   // we'll restore the use count and return FALSE. In the meantime, if
   // HandleRemoveDevice decremented the count to 0 before we did our increment,
   // its thread will have set the remove event. Otherwise, we'll decrement to 0
   // and set the event. Either way, HandleRemoveDevice will wake up to finish
   // removing the device, and we'll return FALSE to our caller.
   // 
   // If, on the other hand, we test "removing" before HandleRemoveDevice sets it,
   // we'll have already incremented the use count past 1 and will return TRUE.
   // Our caller will eventually call UnlockDevice, which will decrement the use
   // count and might set the event HandleRemoveDevice is waiting on at that point.

   if (pdx->removing)
	{
	   if (InterlockedDecrement(&pdx->usage) == 0)
		   KeSetEvent(&pdx->evRemove, 0, FALSE);
	   return FALSE;
	}

   return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// @func Unlock a SIMPLE device object
// @parm Address of our device extension
// @comm If the use count drops to zero, set the evRemove event because we're
// about to remove this device object.

void UnlockDevice(
   PDEVICE_OBJECT fdo
   )
{
   PDEVICE_EXTENSION pdx = (PDEVICE_EXTENSION) fdo->DeviceExtension;
   LONG usage = InterlockedDecrement(&pdx->usage);

   ASSERT(usage >= 0);

   if (usage == 0)
   {						// removing device
      ASSERT(pdx->removing);	// HandleRemoveDevice should already have set this
      KeSetEvent(&pdx->evRemove, 0, FALSE);
   }						// removing device
}

NTSTATUS IsoTransferComplete(
   IN PDEVICE_OBJECT bunkfdo,
   IN PIRP Irp,
   IN PVOID Context
   )
{
   NTSTATUS ntStatus,status;
   PISO_TRANSFER_OBJECT transferObject = (PISO_TRANSFER_OBJECT) Context;
   PISO_STREAM_OBJECT streamObject = transferObject->StreamObject;
   PDEVICE_OBJECT fdo = streamObject->DeviceObject;
   PDEVICE_EXTENSION          pdx = fdo->DeviceExtension;
   PIO_STACK_LOCATION nextStack;
   PURB urb = transferObject->Urb;
   USHORT urbSize = 0;
   ULONG i;
   

   Ezusb_KdPrint(("IsoTransferComplete Irp Status 0x%x\n",Irp->IoStatus.Status));

   //
   // copy the ISO transfer descriptors to the callers buffer.  The user mode caller passed
   // in a large buffer.  That buffer holds both the data and also provides space for the ISO
   // descriptors.  This way, the caller can verify the status of each ISO packet transmitted
   // or sent.
   //
   RtlCopyMemory((PUCHAR) streamObject->IsoDescriptorBuffer + (transferObject->Frame * sizeof(USBD_ISO_PACKET_DESCRIPTOR)),
                 urb->UrbIsochronousTransfer.IsoPacket,
                 (streamObject->FramesPerBuffer * sizeof(USBD_ISO_PACKET_DESCRIPTOR)));

   for (i=0;i<streamObject->FramesPerBuffer; i++)
   {
      Ezusb_KdPrint (("Packet %d length = %d status = %d\n",i,
         urb->UrbIsochronousTransfer.IsoPacket[i].Length,urb->UrbIsochronousTransfer.IsoPacket[i].Status));
   }
      
   
   transferObject->Frame += (streamObject->FramesPerBuffer * streamObject->BufferCount);

   if (transferObject->Frame < streamObject->NumPackets)
   {
      Ezusb_KdPrint(("IsoTransferComplete setting up the next transfer at frame %d\n",transferObject->Frame));

      //
      // reinitialize the URB for the next transfer.
      //
      urbSize = GET_ISO_URB_SIZE(streamObject->FramesPerBuffer);
      RtlZeroMemory(urb,urbSize);

      urb->UrbHeader.Length = urbSize;
      urb->UrbHeader.Function = URB_FUNCTION_ISOCH_TRANSFER;
      urb->UrbIsochronousTransfer.PipeHandle = streamObject->PipeInfo->PipeHandle;
      urb->UrbIsochronousTransfer.TransferFlags =
         USB_ENDPOINT_DIRECTION_IN(streamObject->PipeInfo->EndpointAddress) ? USBD_TRANSFER_DIRECTION_IN : 0;
      urb->UrbIsochronousTransfer.TransferFlags |=
         USBD_START_ISO_TRANSFER_ASAP;
      urb->UrbIsochronousTransfer.TransferFlags |=
         USBD_SHORT_TRANSFER_OK;
      urb->UrbIsochronousTransfer.TransferBufferLength =
         streamObject->PacketSize * streamObject->FramesPerBuffer;
      urb->UrbIsochronousTransfer.TransferBuffer =
         ((PUCHAR) streamObject->TransferBuffer) +  (transferObject->Frame * streamObject->PacketSize);
      urb->UrbIsochronousTransfer.NumberOfPackets = streamObject->FramesPerBuffer;

      for (i=0; i<streamObject->FramesPerBuffer; i++)
      {
         urb->UrbIsochronousTransfer.IsoPacket[i].Offset = i * streamObject->PacketSize;
         urb->UrbIsochronousTransfer.IsoPacket[i].Length = streamObject->PacketSize;
      }

      //
      // initialize the IRP for the next transfer
      // cuz lynn says I hafta
      //
      IoInitializeIrp(Irp,
                     IoSizeOfIrp((pdx->StackDeviceObject->StackSize + 1)),
                     (CCHAR)(pdx->StackDeviceObject->StackSize + 1));

      nextStack = IoGetNextIrpStackLocation(Irp);

      nextStack->Parameters.Others.Argument1 = transferObject->Urb;
      nextStack->Parameters.DeviceIoControl.IoControlCode = 
         IOCTL_INTERNAL_USB_SUBMIT_URB;                    
      nextStack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;

      IoSetCompletionRoutine(Irp,
             IsoTransferComplete,
             transferObject,
             TRUE,
             TRUE,
             TRUE);

      //
      // submit the request
      //
      status = IoCallDriver(pdx->StackDeviceObject,Irp);
      Ezusb_KdPrint(("IsoTransferComplete Resubmit Irp Status 0x%x\n",status));
      ntStatus = STATUS_MORE_PROCESSING_REQUIRED;
   }
   else
   {
      Ezusb_KdPrint(("IsoTransferComplete All done, setting event\n"));

      IoFreeIrp(Irp);
      ExFreePool(urb);

      KeSetEvent(&transferObject->Done,1,FALSE);

      ntStatus = STATUS_MORE_PROCESSING_REQUIRED;
   }

   return ntStatus;
}

NTSTATUS Ezusb_StartIsoTransfer(
   IN PDEVICE_OBJECT fdo,
   IN PIRP Irp
   )
{
   PDEVICE_EXTENSION          pdx = fdo->DeviceExtension;
   PIO_STACK_LOCATION         irpStack = IoGetCurrentIrpStackLocation (Irp);
   PISO_TRANSFER_CONTROL      isoControl =
                              (PISO_TRANSFER_CONTROL)Irp->AssociatedIrp.SystemBuffer;
   ULONG                      bufferLength =
                              irpStack->Parameters.DeviceIoControl.OutputBufferLength;
   ULONG                      packetSize = 0;
   PUSBD_INTERFACE_INFORMATION interfaceInfo = NULL;
   PUSBD_PIPE_INFORMATION     pipeInfo = NULL;
   USBD_PIPE_HANDLE           pipeHandle = NULL;
   PISO_STREAM_OBJECT         streamObject;
   ULONG                      i;

   //
   // verify that the selected pipe is valid, and get a handle to it. If anything
   // is wrong, return an error
   //
   interfaceInfo = pdx->Interface;

   if (!interfaceInfo)
   {
      Ezusb_KdPrint(("Ezusb_StartIsoTransfer() no interface info - Exiting\n"));
      return STATUS_UNSUCCESSFUL;
   }
   
   if (isoControl->PipeNum > interfaceInfo->NumberOfPipes)
   {
      Ezusb_KdPrint(("Ezusb_StartIsoTransfer() invalid pipe - Exiting\n"));
      return STATUS_INVALID_PARAMETER;
   }

   pipeInfo = &(interfaceInfo->Pipes[isoControl->PipeNum]);

   if (!(pipeInfo->PipeType == UsbdPipeTypeIsochronous))
   {
      Ezusb_KdPrint(("Ezusb_StartIsoTransfer() invalid pipe - Exiting\n"));
      return STATUS_INVALID_PARAMETER;
   }

   pipeHandle = pipeInfo->PipeHandle;

   if (!pipeHandle)
   {
      Ezusb_KdPrint(("Ezusb_StartIsoTransfer() invalid pipe - Exiting\n"));
      return STATUS_UNSUCCESSFUL;
   }

   //
   // verify that the FramesPerBuffer and BufferCount parameters are correct.
   // A "feature" of this function is that PacketCount must
   // be zero modulus (FramesPerBuffer * BufferCount).
   //
   if (isoControl->PacketCount % (isoControl->FramesPerBuffer * isoControl->BufferCount))
   {
      Ezusb_KdPrint(("Ezusb_StartIsoTransfer() Invalid parameter - Exiting\n"));
      return STATUS_INVALID_PARAMETER;
   }

   //
   // verify that the caller has provided a buffer large enough for
   // the requested transfer.  The buffer must have room for all of
   // the ISO data plus room for an array of ISO Packet Descriptor
   // objects (1 for each packet transferred).
   //
   packetSize = isoControl->PacketSize;

   if (bufferLength < (isoControl->PacketCount * (packetSize + sizeof(USBD_ISO_PACKET_DESCRIPTOR))))
   {
      Ezusb_KdPrint(("Ezusb_StartIsoTransfer() OutputBuffer too small - Exiting\n"));
      return STATUS_UNSUCCESSFUL;
   }

   //
   // Allocate the streamObject
   //
   streamObject = ExAllocatePool(NonPagedPool, sizeof(ISO_STREAM_OBJECT));

   if (!streamObject)
   {
      Ezusb_KdPrint(("Ezusb_StartIsoTransfer() unable to allocate stream object - Exiting\n"));
      return STATUS_NO_MEMORY;
   }

   //
   // FramesPerBuffer specifies how many frames of ISO data are transferred
   // by a single URB.  This field corresponds to the NumberOfPackets
   // field in the ISO transfer URB (_URB_ISOCH_TRANSFER)
   //
   streamObject->FramesPerBuffer = isoControl->FramesPerBuffer;

   //
   // BufferCount specifies how many IRP's are queued to carry out an ISO transfer.
   //
   streamObject->BufferCount = isoControl->BufferCount;

   streamObject->DeviceObject = fdo;
   streamObject->PipeInfo = pipeInfo;
   streamObject->PacketSize = packetSize;
   streamObject->NumPackets = isoControl->PacketCount;
   streamObject->TransferBuffer = MmGetSystemAddressForMdl(Irp->MdlAddress);
   streamObject->TransferBufferLength = streamObject->PacketSize * streamObject->NumPackets;
   streamObject->IsoDescriptorBuffer = (PUCHAR) streamObject->TransferBuffer + streamObject->TransferBufferLength;
   
   streamObject->TransferObject = ExAllocatePool(NonPagedPool,
                            sizeof(ISO_TRANSFER_OBJECT) * streamObject->BufferCount);

   if (!streamObject->TransferObject)
   {
      Ezusb_KdPrint(("Ezusb_StartIsoTransfer() unable to allocate transfer object - Exiting\n"));
      return STATUS_NO_MEMORY;
   }

   for (i=0; i < streamObject->BufferCount; i++)
   {
      InitTransferObject(streamObject,i);
   }

   for (i=0; i < streamObject->BufferCount; i++)
   {
      IoCallDriver(pdx->StackDeviceObject,
                  streamObject->TransferObject[i].Irp);
   }

   //
   // wait for those guys to finish
   //
   for (i=0; i < streamObject->BufferCount; i++)
   {
      KeWaitForSingleObject(
                    &streamObject->TransferObject[i].Done,
                    Suspended,
                    KernelMode,
                    FALSE,
                    NULL);
   }

   //
   // free the stream and transfer objects
   //
   ExFreePool(streamObject->TransferObject);
   ExFreePool(streamObject);

   return STATUS_SUCCESS;
}

NTSTATUS IsoStreamTransferComplete(
   IN PDEVICE_OBJECT bunkfdo,
   IN PIRP Irp,
   IN PVOID Context
   )
{
   NTSTATUS ntStatus,status;
   PISO_TRANSFER_OBJECT transferObject = (PISO_TRANSFER_OBJECT) Context;
   PISO_STREAM_OBJECT streamObject = transferObject->StreamObject;
   PDEVICE_OBJECT fdo = streamObject->DeviceObject;
   PDEVICE_EXTENSION          pdx = fdo->DeviceExtension;
   PIO_STACK_LOCATION nextStack;
   PURB urb = transferObject->Urb;
   USHORT urbSize = 0;
   ULONG i;
   PUSBD_PIPE_INFORMATION     pipeInfo = streamObject->PipeInfo;

   Ezusb_KdPrint(("IsoTransferComplete Irp Status 0x%x\n",Irp->IoStatus.Status));
   for (i=0;i<streamObject->FramesPerBuffer; i++)
   {
      Ezusb_KdPrint (("Packet %d length = %d status = %d\n",i,
         urb->UrbIsochronousTransfer.IsoPacket[i].Length,urb->UrbIsochronousTransfer.IsoPacket[i].Status));
   }

   //
   // write the transfer descriptors to the descriptor ring buffer
   //
   WriteRingBuffer(streamObject->DescriptorRingBuffer,
                   (PUCHAR) &urb->UrbIsochronousTransfer.IsoPacket[0],
                   (streamObject->FramesPerBuffer * sizeof(USBD_ISO_PACKET_DESCRIPTOR)));

   //
   // write the transfer data to the data ring buffer
   //
   WriteRingBuffer(streamObject->DataRingBuffer,
                   urb->UrbIsochronousTransfer.TransferBuffer,
                   (streamObject->FramesPerBuffer * streamObject->PacketSize));


   //
   // If no errors occured, re-initialize and re-submit the IRP/URB
   //
	if (NT_SUCCESS(Irp->IoStatus.Status) && !pdx->StopIsoStream)
   {

      urbSize = GET_ISO_URB_SIZE(streamObject->FramesPerBuffer);
      RtlZeroMemory(urb,urbSize);

      urb->UrbHeader.Length = urbSize;
      urb->UrbHeader.Function = URB_FUNCTION_ISOCH_TRANSFER;
      urb->UrbIsochronousTransfer.PipeHandle = streamObject->PipeInfo->PipeHandle;
      urb->UrbIsochronousTransfer.TransferFlags =
         USB_ENDPOINT_DIRECTION_IN(streamObject->PipeInfo->EndpointAddress) ? USBD_TRANSFER_DIRECTION_IN : 0;
      urb->UrbIsochronousTransfer.TransferFlags |=
         USBD_START_ISO_TRANSFER_ASAP;
      urb->UrbIsochronousTransfer.TransferFlags |=
         USBD_SHORT_TRANSFER_OK;
      urb->UrbIsochronousTransfer.TransferBufferLength =
         streamObject->PacketSize * streamObject->FramesPerBuffer;
      urb->UrbIsochronousTransfer.TransferBuffer =
         ((PUCHAR) streamObject->TransferBuffer) +  (transferObject->Frame * streamObject->PacketSize);
      urb->UrbIsochronousTransfer.NumberOfPackets = streamObject->FramesPerBuffer;

      for (i=0; i<streamObject->FramesPerBuffer; i++)
      {
         urb->UrbIsochronousTransfer.IsoPacket[i].Offset = i * streamObject->PacketSize;
         urb->UrbIsochronousTransfer.IsoPacket[i].Length = streamObject->PacketSize;
      }

      //
      // initialize the IRP for the next transfer
      // cuz lynn says I hafta
      //
      IoInitializeIrp(Irp,
                     IoSizeOfIrp((pdx->StackDeviceObject->StackSize + 1)),
                     (CCHAR)(pdx->StackDeviceObject->StackSize + 1));

      nextStack = IoGetNextIrpStackLocation(Irp);

      nextStack->Parameters.Others.Argument1 = transferObject->Urb;
      nextStack->Parameters.DeviceIoControl.IoControlCode = 
         IOCTL_INTERNAL_USB_SUBMIT_URB;                    
      nextStack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;

      IoSetCompletionRoutine(Irp,
             IsoStreamTransferComplete,
             transferObject,
             TRUE,
             TRUE,
             TRUE);

      //
      // submit the request
      //
      status = IoCallDriver(pdx->StackDeviceObject,Irp);
      //Ezusb_KdPrint(("IsoTransferComplete Resubmit Irp Status 0x%x\n",status));
      ntStatus = STATUS_MORE_PROCESSING_REQUIRED;
   }
   else
   {
      Ezusb_KdPrint(("IsoTransferComplete All done, setting event\n"));

      IoFreeIrp(Irp);
      ExFreePool(urb);

      streamObject->PendingTransfers--;
      if (streamObject->PendingTransfers == 0)
      {
         ExFreePool(streamObject->TransferBuffer);
         ExFreePool(streamObject->TransferObject);
         FreeRingBuffer(streamObject->DataRingBuffer);
         pdx->DataRingBuffer = NULL;
         FreeRingBuffer(streamObject->DescriptorRingBuffer);
         pdx->DescriptorRingBuffer = NULL;

         ExFreePool(streamObject);
      }

      ntStatus = STATUS_MORE_PROCESSING_REQUIRED;
   }

   return ntStatus;
}

NTSTATUS Ezusb_StartIsoStream(
   IN PDEVICE_OBJECT fdo,
   IN PIRP Irp
   )
{
   PDEVICE_EXTENSION          pdx = fdo->DeviceExtension;
   PIO_STACK_LOCATION         irpStack = IoGetCurrentIrpStackLocation (Irp);
   PISO_TRANSFER_CONTROL      isoControl =
                              (PISO_TRANSFER_CONTROL)Irp->AssociatedIrp.SystemBuffer;
   PUSBD_INTERFACE_INFORMATION interfaceInfo = NULL;
   PUSBD_PIPE_INFORMATION     pipeInfo = NULL;
   USBD_PIPE_HANDLE           pipeHandle = NULL;
   PISO_STREAM_OBJECT         streamObject;
   ULONG                      i;

   //
   // verify that the selected pipe is valid, and get a handle to it. If anything
   // is wrong, return an error
   //
   interfaceInfo = pdx->Interface;

   if (!interfaceInfo)
   {
      Ezusb_KdPrint(("Ezusb_StartIsoTransfer() no interface info - Exiting\n"));
      return STATUS_UNSUCCESSFUL;
   }
   
   if (isoControl->PipeNum > interfaceInfo->NumberOfPipes)
   {
      Ezusb_KdPrint(("Ezusb_StartIsoTransfer() invalid pipe - Exiting\n"));
      return STATUS_INVALID_PARAMETER;
   }

   pipeInfo = &(interfaceInfo->Pipes[isoControl->PipeNum]);

   if (!(pipeInfo->PipeType == UsbdPipeTypeIsochronous))
   {
      Ezusb_KdPrint(("Ezusb_StartIsoTransfer() invalid pipe - Exiting\n"));
      return STATUS_INVALID_PARAMETER;
   }

   pipeHandle = pipeInfo->PipeHandle;

   if (!pipeHandle)
   {
      Ezusb_KdPrint(("Ezusb_StartIsoTransfer() invalid pipe - Exiting\n"));
      return STATUS_UNSUCCESSFUL;
   }

   //
   // Allocate the streamObject
   //
   streamObject = ExAllocatePool(NonPagedPool, sizeof(ISO_STREAM_OBJECT));

   if (!streamObject)
   {
      Ezusb_KdPrint(("Ezusb_StartIsoTransfer() unable to allocate stream object - Exiting\n"));
      return STATUS_NO_MEMORY;
   }

   //
   // FramesPerBuffer specifies how many frames of ISO data are transferred
   // by a single URB.  This field corresponds to the NumberOfPackets
   // field in the ISO transfer URB (_URB_ISOCH_TRANSFER)
   //
// MDNISO   streamObject->PendingTransfers = 
   streamObject->FramesPerBuffer = isoControl->FramesPerBuffer;

   //
   // BufferCount specifies how many IRP's are queued to carry out an ISO transfer.
   //
   streamObject->PendingTransfers = 
   streamObject->BufferCount = isoControl->BufferCount;

   streamObject->DeviceObject = fdo;
   streamObject->PipeInfo = pipeInfo;
   streamObject->PacketSize = isoControl->PacketSize;
   streamObject->TransferBufferLength = streamObject->PacketSize * streamObject->FramesPerBuffer * streamObject->BufferCount;
   streamObject->TransferBuffer = ExAllocatePool(NonPagedPool, streamObject->TransferBufferLength);
   if (!streamObject->TransferBuffer)
   {
      Ezusb_KdPrint(("Ezusb_StartIsoTransfer() unable to allocate transfer buffer - Exiting\n"));
      return STATUS_NO_MEMORY;
   }

   streamObject->DataRingBuffer = AllocRingBuffer(isoControl->PacketCount * isoControl->PacketSize);
   if (!streamObject->DataRingBuffer)
   {
      Ezusb_KdPrint(("Ezusb_StartIsoTransfer() unable to allocate data ring buffer - Exiting\n"));
      return STATUS_NO_MEMORY;
   }
   pdx->DataRingBuffer = streamObject->DataRingBuffer;
   
   streamObject->DescriptorRingBuffer =
      AllocRingBuffer(isoControl->PacketCount * sizeof(USBD_ISO_PACKET_DESCRIPTOR));
   if (!streamObject->DescriptorRingBuffer)
   {
      Ezusb_KdPrint(("Ezusb_StartIsoTransfer() unable to allocate descriptor ring buffer - Exiting\n"));
      return STATUS_NO_MEMORY;
   }
   pdx->DescriptorRingBuffer = streamObject->DescriptorRingBuffer;

   streamObject->TransferObject = ExAllocatePool(NonPagedPool,
                            sizeof(ISO_TRANSFER_OBJECT) * streamObject->BufferCount);

   if (!streamObject->TransferObject)
   {
      Ezusb_KdPrint(("Ezusb_StartIsoTransfer() unable to allocate transfer object - Exiting\n"));
      return STATUS_NO_MEMORY;
   }

   for (i=0; i < streamObject->BufferCount; i++)
   {
      InitTransferObject(streamObject,i);

      //
      // override the completion routine that InitTransferObject registered
      //
      IoSetCompletionRoutine(streamObject->TransferObject[i].Irp,
                             IsoStreamTransferComplete,
                             &streamObject->TransferObject[i],
                             TRUE,
                             TRUE,
                             TRUE);


   }

   pdx->StopIsoStream = FALSE;

   for (i=0; i < streamObject->BufferCount; i++)
   {
      IoCallDriver(pdx->StackDeviceObject,
                  streamObject->TransferObject[i].Irp);
   }

   return STATUS_SUCCESS;
}

NTSTATUS InitTransferObject(
   IN OUT PISO_STREAM_OBJECT streamObject,
   IN ULONG index
   )
{
   PISO_TRANSFER_OBJECT transferObject = &streamObject->TransferObject[index];
   PUSBD_PIPE_INFORMATION pipeInfo = streamObject->PipeInfo;
   USHORT urbSize = 0;
   CCHAR stackSize;
   PIO_STACK_LOCATION nextStack = NULL;
   PURB urb = NULL;
   PIRP irp = NULL;
   PDEVICE_EXTENSION pdx =
      (PDEVICE_EXTENSION) streamObject->DeviceObject->DeviceExtension;
   ULONG i;


   //
   // allocate and prepare the URB
   //
   urbSize = GET_ISO_URB_SIZE(streamObject->FramesPerBuffer);

   urb = ExAllocatePool(NonPagedPool, urbSize);
   RtlZeroMemory(urb,urbSize);

   urb->UrbHeader.Length = urbSize;
   urb->UrbHeader.Function = URB_FUNCTION_ISOCH_TRANSFER;
   urb->UrbIsochronousTransfer.PipeHandle = pipeInfo->PipeHandle;
   urb->UrbIsochronousTransfer.TransferFlags =
      USB_ENDPOINT_DIRECTION_IN(pipeInfo->EndpointAddress) ? USBD_TRANSFER_DIRECTION_IN : 0;
   urb->UrbIsochronousTransfer.TransferFlags |=
      USBD_START_ISO_TRANSFER_ASAP;
   urb->UrbIsochronousTransfer.TransferFlags |=
      USBD_SHORT_TRANSFER_OK;
   urb->UrbIsochronousTransfer.TransferBufferLength =
      streamObject->PacketSize * streamObject->FramesPerBuffer;
   urb->UrbIsochronousTransfer.TransferBuffer =
      ((PUCHAR) streamObject->TransferBuffer) +  (index * streamObject->PacketSize * streamObject->FramesPerBuffer);

   urb->UrbIsochronousTransfer.NumberOfPackets = streamObject->FramesPerBuffer;

   //
   // setup the ISO packet descriptors
   //
   for (i=0; i<streamObject->FramesPerBuffer; i++)
   {
      urb->UrbIsochronousTransfer.IsoPacket[i].Offset = i * streamObject->PacketSize;
      urb->UrbIsochronousTransfer.IsoPacket[i].Length = streamObject->PacketSize;
   }

   stackSize = (CCHAR) (pdx->StackDeviceObject->StackSize + 1);

   //
   // allocate and prepare the IRP
   //
   irp = IoAllocateIrp(stackSize, FALSE);
   IoInitializeIrp(irp, irp->Size, stackSize);

   nextStack = IoGetNextIrpStackLocation(irp);

   nextStack->Parameters.Others.Argument1 = urb;
   nextStack->Parameters.DeviceIoControl.IoControlCode = 
      IOCTL_INTERNAL_USB_SUBMIT_URB;                    
   nextStack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;

   IoSetCompletionRoutine(irp,
                          IsoTransferComplete,
                          transferObject,
                          TRUE,
                          TRUE,
                          TRUE);

   transferObject->Frame = index * streamObject->FramesPerBuffer;
   transferObject->Urb = urb;
   transferObject->Irp = irp;
   transferObject->StreamObject = streamObject;
   KeInitializeEvent(&transferObject->Done, NotificationEvent, FALSE);

   return STATUS_SUCCESS;
}

NTSTATUS Ezusb_8051Reset(
   PDEVICE_OBJECT fdo,
   UCHAR resetBit
   )
/*++

Routine Description:
   Uses the ANCHOR LOAD vendor specific command to either set or release the
   8051 reset bit in the EZ-USB chip.

Arguments:
   fdo - pointer to the device object for this instance of an Ezusb Device
   resetBit - 1 sets the 8051 reset bit (holds the 8051 in reset)
              0 clears the 8051 reset bit (8051 starts running)
              
Return Value:
   STATUS_SUCCESS if successful,
   STATUS_UNSUCCESSFUL otherwise

--*/
{
   NTSTATUS ntStatus;
   PURB urb = NULL;

   urb = ExAllocatePool(NonPagedPool, 
                       sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST));

   if (urb)
   {

      // toggle the EZ-USB reset bit (harmless on FX2)
      RtlZeroMemory(urb,sizeof(struct  _URB_CONTROL_VENDOR_OR_CLASS_REQUEST));

      urb->UrbHeader.Length = sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);
      urb->UrbHeader.Function = URB_FUNCTION_VENDOR_DEVICE;

      urb->UrbControlVendorClassRequest.TransferBufferLength = 1;
      urb->UrbControlVendorClassRequest.TransferBuffer = &resetBit;
      urb->UrbControlVendorClassRequest.TransferBufferMDL = NULL;
      urb->UrbControlVendorClassRequest.Request = ANCHOR_LOAD_INTERNAL;
      urb->UrbControlVendorClassRequest.Value = CPUCS_REG_EZUSB;
      urb->UrbControlVendorClassRequest.Index = 0;

      ntStatus = Ezusb_CallUSBD(fdo, urb);

      // toggle the FX2 reset bit (harmless on EZ-USB)
      RtlZeroMemory(urb,sizeof(struct  _URB_CONTROL_VENDOR_OR_CLASS_REQUEST));

      urb->UrbHeader.Length = sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);
      urb->UrbHeader.Function = URB_FUNCTION_VENDOR_DEVICE;

      urb->UrbControlVendorClassRequest.TransferBufferLength = 1;
      urb->UrbControlVendorClassRequest.TransferBuffer = &resetBit;
      urb->UrbControlVendorClassRequest.TransferBufferMDL = NULL;
      urb->UrbControlVendorClassRequest.Request = ANCHOR_LOAD_INTERNAL;
      urb->UrbControlVendorClassRequest.Value = CPUCS_REG_FX2;
      urb->UrbControlVendorClassRequest.Index = 0;

      ntStatus = Ezusb_CallUSBD(fdo, urb);
   }
   else
   {
      ntStatus = STATUS_NO_MEMORY;
   }

   if (urb)
      ExFreePool(urb);

   return ntStatus;
}

//
// this is the number of bytes of firmware to download per setup transfer.
// 
#define CHUNK_SIZE 64

NTSTATUS Ezusb_AnchorDownload(
   PDEVICE_OBJECT fdo,
   WORD offset,
   PUCHAR downloadBuffer,
   ULONG downloadSize
   )
/*++

Routine Description:
   Uses the ANCHOR LOAD vendor specific command to download code to the EZ-USB
   device.  The actual code is stored as data within the driver binary in the
   global 'firmware' which is an EZUSB_FIRMWARE struct included in the file
   firmware.c.

Arguments:
   fdo - pointer to the device object for this instance of an Ezusb Device
   downloadBuffer - pointer to the firmware image
   downloadSize - total size (bytes) of the firmware image to download
   
Return Value:
   STATUS_SUCCESS if successful,
   STATUS_UNSUCCESSFUL otherwise

--*/
{
   NTSTATUS ntStatus;
   PURB urb = NULL;
   int i;
   int chunkCount;
   PUCHAR ptr = downloadBuffer;

   urb = ExAllocatePool(NonPagedPool, 
                       sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST));

   if (urb)
   {
      chunkCount = ((downloadSize + CHUNK_SIZE - 1) / CHUNK_SIZE);
      //
      // The download will be split into CHUNK_SIZE pieces and
      // downloaded with multiple setup transfers.  For the Rev B parts
      // CHUNK_SIZE should not exceed 64 bytes, as larger transfers can
      // result in data corruption when other USB devices are present.
      //
      for (i = 0; i < chunkCount; i++)
      {
         RtlZeroMemory(urb,sizeof(struct  _URB_CONTROL_VENDOR_OR_CLASS_REQUEST));

         urb->UrbHeader.Length = sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);
         urb->UrbHeader.Function = URB_FUNCTION_VENDOR_DEVICE;

         urb->UrbControlVendorClassRequest.TransferBufferLength =
            ((i == (chunkCount - 1)) &&  (downloadSize % CHUNK_SIZE)) ?
            (downloadSize % CHUNK_SIZE) :
            CHUNK_SIZE;

         urb->UrbControlVendorClassRequest.TransferBuffer = ptr;
         urb->UrbControlVendorClassRequest.TransferBufferMDL = NULL;
         urb->UrbControlVendorClassRequest.Request = ANCHOR_LOAD_INTERNAL;
         urb->UrbControlVendorClassRequest.Value = (i * CHUNK_SIZE) + offset;
         urb->UrbControlVendorClassRequest.Index = 0;

         ntStatus = Ezusb_CallUSBD(fdo, urb);

         if (!NT_SUCCESS(ntStatus))
            break;

         ptr += CHUNK_SIZE;
      }
   }
   else
   {
      ntStatus = STATUS_NO_MEMORY;
   }

   if (urb)
      ExFreePool(urb);

   return ntStatus;
}

NTSTATUS Ezusb_DownloadIntelHex(
   PDEVICE_OBJECT fdo,
   PINTEL_HEX_RECORD hexRecord
   )
/*++

Routine Description:
   This function downloads Intel Hex Records to the EZ-USB device.  If any of the hex records
   are destined for external RAM, then the caller must have previously downloaded firmware
   to the device that knows how to download to external RAM (ie. firmware that implements
   the ANCHOR_LOAD_EXTERNAL vendor specific command).

Arguments:
   fdo - pointer to the device object for this instance of an Ezusb Device
   hexRecord - pointer to an array of INTEL_HEX_RECORD structures.  This array
               is terminated by an Intel Hex End record (Type = 1).

Return Value:
   STATUS_SUCCESS if successful,
   STATUS_UNSUCCESSFUL otherwise

--*/
{
   NTSTATUS ntStatus;
   PURB urb = NULL;
   PINTEL_HEX_RECORD ptr = hexRecord;

   urb = ExAllocatePool(NonPagedPool, 
                       sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST));

   if (urb)
   {
      //
      // The download must be performed in two passes.  The first pass loads all of the
      // external addresses, and the 2nd pass loads to all of the internal addresses.
      // why?  because downloading to the internal addresses will probably wipe out the firmware
      // running on the device that knows how to receive external ram downloads.
      //
      //
      // First download all the records that go in external ram
      //
      while (ptr->Type == 0)
      {
         if (!INTERNAL_RAM(ptr->Address))
         {
            RtlZeroMemory(urb,sizeof(struct  _URB_CONTROL_VENDOR_OR_CLASS_REQUEST));

            urb->UrbHeader.Length = sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);
            urb->UrbHeader.Function = URB_FUNCTION_VENDOR_DEVICE;
            urb->UrbControlVendorClassRequest.TransferBufferLength = ptr->Length;
            urb->UrbControlVendorClassRequest.TransferBuffer = ptr->Data;
            urb->UrbControlVendorClassRequest.Request = ANCHOR_LOAD_EXTERNAL;
            urb->UrbControlVendorClassRequest.Value = ptr->Address;
            urb->UrbControlVendorClassRequest.Index = 0;

            ntStatus = Ezusb_CallUSBD(fdo, urb);

            if (!NT_SUCCESS(ntStatus))
               break;
         }
         ptr++;
      }

      //
      // Now download all of the records that are in internal RAM.  Before starting
      // the download, stop the 8051.
      //
      Ezusb_8051Reset(fdo,1);
      ptr = hexRecord;
      while (ptr->Type == 0)
      {
         if (INTERNAL_RAM(ptr->Address))
         {
            RtlZeroMemory(urb,sizeof(struct  _URB_CONTROL_VENDOR_OR_CLASS_REQUEST));

            urb->UrbHeader.Length = sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);
            urb->UrbHeader.Function = URB_FUNCTION_VENDOR_DEVICE;
            urb->UrbControlVendorClassRequest.TransferBufferLength = ptr->Length;
            urb->UrbControlVendorClassRequest.TransferBuffer = ptr->Data;
            urb->UrbControlVendorClassRequest.Request = ANCHOR_LOAD_INTERNAL;
            urb->UrbControlVendorClassRequest.Value = ptr->Address;
            urb->UrbControlVendorClassRequest.Index = 0;

            ntStatus = Ezusb_CallUSBD(fdo, urb);

            if (!NT_SUCCESS(ntStatus))
               break;
         }
         ptr++;
      }

   }
   else
   {
      ntStatus = STATUS_NO_MEMORY;
   }

   if (urb)
      ExFreePool(urb);

   return ntStatus;
}

PRING_BUFFER
AllocRingBuffer(
   ULONG    Size
   )
{
   PRING_BUFFER   ringBuffer = NULL;

   ringBuffer = ExAllocatePool(NonPagedPool, sizeof(RING_BUFFER));

   if (!ringBuffer)
      return NULL;

   ringBuffer->buffer = ExAllocatePool(NonPagedPool, Size);

   if (!ringBuffer->buffer)
   {
      ExFreePool(ringBuffer);
      return NULL;
   }

   ringBuffer->inPtr = ringBuffer->buffer;
   ringBuffer->outPtr = ringBuffer->buffer;
   ringBuffer->totalSize = Size;
   ringBuffer->currentSize = 0;

   KeInitializeSpinLock(&ringBuffer->spinLock);

   return ringBuffer;
}

VOID
FreeRingBuffer(
   PRING_BUFFER   ringBuffer
   )
{
   ExFreePool(ringBuffer->buffer);
   ExFreePool(ringBuffer);
}

ULONG
ReadRingBuffer(
   PRING_BUFFER   ringBuffer,
   PUCHAR         readBuffer,
   ULONG          numberOfBytesToRead
   )
/*
   Routine Description:
   This routine reads data from a ring buffer.

   Arguments:
   ringBuffer - pointer to a ring buffer structure
   readBuffer - pointer to a user supplied buffer to transfer data into
   numberOfBytesToRead - number of bytes to read from the ring buffer

   Return Value:
   ULONG - number of bytes read.  May be smaller than requested number of bytes.
*/
{
   ULONG    byteCount;
   KIRQL    oldIrql;

   Ezusb_KdPrint(( "ReadRingBuffer() enter\n"));

   if (numberOfBytesToRead > ringBuffer->totalSize)
      return 0;

   if (ringBuffer->currentSize == 0)
      return 0;

   if ( numberOfBytesToRead > ringBuffer->currentSize )
      byteCount = ringBuffer->currentSize;
   else
      byteCount = numberOfBytesToRead;

   //
   // two cases.  Read either wraps or it doesn't.
   // Handle the non-wrapped case first
   //
   if ((ringBuffer->outPtr + byteCount - 1) < 
       (ringBuffer->buffer + ringBuffer->totalSize))
   {
      Ezusb_KdPrint(( "ReadRingBuffer() about to copy a\n"));
      RtlCopyMemory(readBuffer, ringBuffer->outPtr, byteCount);
      ringBuffer->outPtr += byteCount;
      if (ringBuffer->outPtr == ringBuffer->buffer + ringBuffer->totalSize)
         ringBuffer->outPtr = ringBuffer->buffer;
   }
   // now handle the wrapped case
   else
   {
      ULONG    fragSize;

      Ezusb_KdPrint(( "ReadRingBuffer() about to copy b\n"));
      // get the first half of the read
      fragSize = ringBuffer->buffer + ringBuffer->totalSize - ringBuffer->outPtr;
      RtlCopyMemory(readBuffer, ringBuffer->outPtr, fragSize);

      // now get the rest
      RtlCopyMemory(readBuffer + fragSize, ringBuffer->buffer, byteCount - fragSize);

      ringBuffer->outPtr = ringBuffer->buffer + byteCount - fragSize;
   }
 
   // 
   // update the current size of the ring buffer.  Use spinlock to insure
   // atomic operation.
   //
   KeAcquireSpinLock(&ringBuffer->spinLock, &oldIrql);
   ringBuffer->currentSize -= byteCount;
   KeReleaseSpinLock(&ringBuffer->spinLock, oldIrql);

   Ezusb_KdPrint(( "ReadRingBuffer() exit\n"));

   return byteCount;
}

ULONG
WriteRingBuffer(
   PRING_BUFFER   ringBuffer,
   PUCHAR         writeBuffer,
   ULONG          numberOfBytesToWrite
   )
/*
   Routine Description:
   This routine writes data to a ring buffer.  If the requested write size exceeds
   available space in the ring buffer, then the write is rejected.

   Arguments:
   ringBuffer - pointer to a ring buffer structure
   readBuffer - pointer to a user supplied buffer of data to copy to the ring buffer
   numberOfBytesToRead - number of bytes to write to the ring buffer

   Return Value:
   ULONG - number of bytes written.
*/
{
   ULONG    byteCount;
   KIRQL    oldIrql;

   if ( numberOfBytesToWrite >
        (ringBuffer->totalSize - ringBuffer->currentSize) )
   {
      Ezusb_KdPrint(( "WriteRingBuffer() OVERFLOW\n"));
      return 0;
   }

   byteCount = numberOfBytesToWrite;

   //
   // two cases.  Write either wraps or it doesn't.
   // Handle the non-wrapped case first
   //
   if ((ringBuffer->inPtr + byteCount - 1) < 
       (ringBuffer->buffer + ringBuffer->totalSize))
   {
      RtlCopyMemory(ringBuffer->inPtr, writeBuffer, byteCount);
      ringBuffer->inPtr += byteCount;
      if (ringBuffer->inPtr == ringBuffer->buffer + ringBuffer->totalSize)
         ringBuffer->inPtr = ringBuffer->buffer;
   }
   // now handle the wrapped case
   else
   {
      ULONG    fragSize;

      // write the first fragment
      fragSize = ringBuffer->buffer + ringBuffer->totalSize - ringBuffer->inPtr;
      RtlCopyMemory(ringBuffer->inPtr, writeBuffer, fragSize);

      // now write the rest
      RtlCopyMemory(ringBuffer->buffer, writeBuffer + fragSize, byteCount - fragSize);

      ringBuffer->inPtr = ringBuffer->buffer + byteCount - fragSize;
   }

   // 
   // update the current size of the ring buffer.  Use spinlock to insure
   // atomic operation.
   //
   KeAcquireSpinLock(&ringBuffer->spinLock, &oldIrql);
   ringBuffer->currentSize += byteCount;
   KeReleaseSpinLock(&ringBuffer->spinLock, oldIrql);

   return byteCount;
}

BOOLEAN IsFx2(
   IN  PDEVICE_OBJECT fdo
   )
/*
   Routine Description:
   This routine queries the device via a SETUP command to determine if it is
   an FX2 chip.  In order for this to work, the device must implement the vendor
   specific IN SETUP command 0xAC, which returns 1 if the chip is FX2 and 0
   otherwise.

   Arguments:
   fdo - our device object

   Return Value:
   BOOL - TRUE if FX2, FALSE otherwise
*/
{
   NTSTATUS ntStatus;
   PURB urb = NULL;
   UCHAR transferBuffer[64];

   urb = ExAllocatePool(NonPagedPool, 
                       sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST));

   if (urb)
   {
      RtlZeroMemory(urb,sizeof(struct  _URB_CONTROL_VENDOR_OR_CLASS_REQUEST));

      urb->UrbHeader.Length = sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);
      urb->UrbHeader.Function = URB_FUNCTION_VENDOR_DEVICE;

      urb->UrbControlVendorClassRequest.TransferFlags = USBD_TRANSFER_DIRECTION_IN ;
      urb->UrbControlVendorClassRequest.TransferBufferLength = 1;
      urb->UrbControlVendorClassRequest.TransferBuffer = transferBuffer;
      urb->UrbControlVendorClassRequest.TransferBufferMDL = NULL;
      urb->UrbControlVendorClassRequest.Request = ANCHOR_ISFX2;
      urb->UrbControlVendorClassRequest.Value = 0;
      urb->UrbControlVendorClassRequest.Index = 0;

      Ezusb_KdPrint (("**** About to query ISFX2\n"));

      ntStatus = Ezusb_CallUSBD(fdo, urb);
   }
   else
   {
      ntStatus = STATUS_NO_MEMORY;
   }

   if (urb)
      ExFreePool(urb);

   if (transferBuffer[0] == 1)
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}

NTSTATUS Ezusb_SetFeature(
   IN PDEVICE_OBJECT fdo,
   IN PSET_FEATURE_CONTROL setFeatureControl
   )
/*
   Routine Description:
   This routine performs a Set Feature control transfer

   Arguments:
   fdo - our device object
   setFeatureControl - a data structure that contains the arguments for the
   set featire command

   Return Value:
   NTSTATUS 
*/
{
   NTSTATUS            ntStatus        = STATUS_SUCCESS;
   PURB                urb             = NULL;
   ULONG               length          = 0;
   PDEVICE_EXTENSION   pdx = fdo->DeviceExtension;

   Ezusb_KdPrint (("Enter Ezusb_SetFeature\n"));    

   urb = ExAllocatePool(NonPagedPool, 
                      sizeof(struct _URB_CONTROL_FEATURE_REQUEST));
                   
   if (urb)
   {
      RtlZeroMemory(urb,sizeof(struct _URB_CONTROL_FEATURE_REQUEST));

      urb->UrbHeader.Length = sizeof(struct _URB_CONTROL_FEATURE_REQUEST);
      urb->UrbHeader.Function = URB_FUNCTION_SET_FEATURE_TO_DEVICE;

      urb->UrbControlFeatureRequest.FeatureSelector = setFeatureControl->FeatureSelector;
      urb->UrbControlFeatureRequest.Index = setFeatureControl->Index;
                                                                  
      ntStatus = Ezusb_CallUSBD(fdo, urb);

   }
   else
   {
      ntStatus = STATUS_NO_MEMORY;        
   }        

   Ezusb_KdPrint (("Leaving Ezusb_SetFeature\n"));    

   return ntStatus;
}