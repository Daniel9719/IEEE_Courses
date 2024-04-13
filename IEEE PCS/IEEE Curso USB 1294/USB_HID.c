#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "driverlib/usb.h"
#include "usblib/usblib.h"
#include "usblib/usbhid.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdhid.h"
#include "DAP.h"
#include "DAP_config.h"

#define USB_VID_YOUR_VENDOR_ID 0x0912
#define USB_PID_YOUR_PRODUCT_ID 0x50C5
#define POWER_CONSUMPTION_MA 0x64

//Prototypes 
static uint32_t USBReceiveEventCallback(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgParam, void *pvMsgData);
static uint32_t USBTransmitEventCallback(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgParam, void *pvMsgData);

//*****************************************************************************
//
// The languages supported by this device.
//
//*****************************************************************************
const uint8_t g_pui8LangDescriptor[] =
{
    4,
    USB_DTYPE_STRING,
    USBShort(USB_LANG_EN_US)
};

//*****************************************************************************
//
// The manufacturer string.
//
//*****************************************************************************
const uint8_t g_pui8ManufacturerString[] =
{
    (23 + 1) * 2,
    USB_DTYPE_STRING,
    'I', 0, 'E', 0, 'E', 0, 'E', 0, ' ', 0, 'C', 0, 'A', 0, 'S', 0, ' ', 0,
    'S', 0, 't', 0, 'u', 0, 'd', 0, 'e', 0, 'n', 0, 't', 0, ' ', 0, 'B', 0,
    'r', 0, 'a', 0, 'n', 0, 'c', 0, 'h', 0
};

//*****************************************************************************
//
// The product string.
//
//*****************************************************************************
const uint8_t g_pui8ProductString[] =
{
    (17 + 1) * 2,
    USB_DTYPE_STRING,
    'D', 0, 'e', 0, 'b', 0, 'u', 0, 'g', 0, ' ', 0, 'A', 0, 'c', 0, 'c', 0,
    'e', 0, 's', 0, 's', 0, ' ', 0, 'P', 0, 'o', 0, 'r', 0, 't', 0
};

//*****************************************************************************
//
// The serial number string.
//
//*****************************************************************************
const uint8_t g_pui8SerialNumberString[] =
{
    (8 + 1) * 2,
    USB_DTYPE_STRING,
    '1', 0, '2', 0, '3', 0, '4', 0, '5', 0, '6', 0, '7', 0, '8', 0
};

//*****************************************************************************
//
// The descriptor string table.
//
//*****************************************************************************
const uint8_t * const g_pui8StringDescriptors[] =
{
    g_pui8LangDescriptor,
    g_pui8ManufacturerString,
    g_pui8ProductString,
    g_pui8SerialNumberString,
};

#define NUM_STRING_DESCRIPTORS (sizeof(g_pui8StringDescriptors) /             \
                                sizeof(uint8_t *))

//*****************************************************************************
//
// The following is the custom HID report structure definition that is passed
// back to the host.  This structure shows the basics of overriding the default
// HID gamepad descriptor provided by the USB library.  Every entry in this
// report descriptor is mapped into the tCustomReport structure.
//
//*****************************************************************************
static const uint8_t g_pui8VendorReportDescriptor[] =
{
    UsagePageVendor(USB_HID_VENDOR_DEFINED),
            Usage(HID_VEND_USAGE_1),
        Collection(USB_HID_APPLICATION),

            UsageMinimum(1),
            UsageMaximum(64),
    LogicalMinimum(0),
    LogicalMaximum(64),      
    ReportSize(8),
    ReportCount(64),
        Input(USB_HID_INPUT_DATA | USB_HID_INPUT_ARRAY | USB_HID_INPUT_ABS),
            UsageMinimum(1),
            UsageMaximum(64),
        Output(USB_HID_INPUT_DATA | USB_HID_INPUT_ARRAY | USB_HID_INPUT_ABS),

        EndCollection
};

//*****************************************************************************
//
// The HID class descriptor table. For the mouse class, we have only a single
// report descriptor.
//
//*****************************************************************************
static const uint8_t * const g_pDAPClassDescriptors[] =
{
    g_pui8VendorReportDescriptor
};

//*****************************************************************************
//
// The HID descriptor for the mouse device.
//
//*****************************************************************************
static const tHIDDescriptor g_sDAPHIDDescriptor =
{
    .bLength = 9, // bLength
    .bDescriptorType = USB_HID_DTYPE_HID, // bDescriptorType
    .bcdHID = 0x111, // bcdHID (version 1.11 compliant)
    .bCountryCode = 0, // bCountryCode (not localized)
    .bNumDescriptors = 1, // bNumDescriptors
    .sClassDescriptor = USB_HID_DTYPE_REPORT, // Report descriptor
    .sizeof(g_pui8VendorReportDescriptor) // Size of report descriptor
};

tHIDReportIdle g_psReportIdle[1] =
{
    { 125, 1, 0, 0 } // Report 1 polled every 500mS (4 * 125).
    // { 0, 2, 0, 0} // Report 2 is not polled (0mS timeout)
};

const tUSBDHIDDevice g_sHIDDAPDevice =
{
    //
    // The Vendor ID you have been assigned by USB-IF.
    //
    .ui16VID = USB_VID_YOUR_VENDOR_ID,
    //
    // The product ID you have assigned for this device.
    //
    .ui16PID = USB_PID_YOUR_PRODUCT_ID,
    //
    // The power consumption of your device in milliamps.
    //
    .ui16MaxPowermA = POWER_CONSUMPTION_MA,
    //
    // The value to be passed to the host in the USB configuration descriptor’s
    // bmAttributes field.
    //
    .ui8PwrAttributes = USB_CONF_ATTR_BUS_PWR,
    //
    // This mouse supports the boot subclass.
    //
    .ui8Subclass = USB_HID_SCLASS_NONE,
    //
    // This device supports the BIOS mouse report protocol.
    //
    .ui8Protocol = USB_HID_PROTOCOL_NONE,
    //
    // The device has a single input report.
    //
    .ui8NumInputReports = 1,
    //
    // A pointer to our array of tHIDReportIdle structures. For this device,
    // the array must have 1 element (matching the value of the previous field).
    //
    .psReportIdle = g_psReportIdle,
    //
    // A pointer to your receive callback event handler.
    //
    .pfnRxCallback = &USBReceiveEventCallback,
    //
    // A value that you want passed to the receive callback alongside every
    // event.
    //
    .pvRxCBData = NULL,
    //
    // A pointer to your transmit callback event handler.
    //
    .pfnTxCallback = &USBTransmitEventCallback,
    //
    // A value that you want passed to the transmit callback alongside every
    // event.
    //
    .pvTxCBData = NULL,
    //
    // This device does not want to use a dedicated interrupt OUT endpoint
    // since there are no output or feature reports required.
    //
    .bUseOutEndpoint = false,
    //
    // A pointer to the HID descriptor for the device.
    //
    .psHIDDescriptor = &g_sDAPHIDDescriptor,
    //
    // A pointer to the array of HID class descriptor pointers for this device.
    // The number of elements in this array and their order must match the
    // information in the HID descriptor provided above.
    //
    .ppui8ClassDescriptors = g_pDAPClassDescriptors,
    //
    // A pointer to your string table.
    //
    .ppui8StringDescriptors = g_pui8StringDescriptors,
    //
    // The number of entries in your string table. This must equal
    // (1 + (5 + (num HID strings)) * (num languages)).
    //
    .ui32NumStringDescriptors = NUM_STRING_DESCRIPTORS
};

static volatile uint16_t USB_RequestIndexI;     // Request  Index In
static volatile uint16_t USB_RequestIndexO;     // Request  Index Out
static volatile uint16_t USB_RequestCountI;     // Request  Count In
static volatile uint16_t USB_RequestCountO;     // Request  Count Out
static volatile uint8_t  USB_RequestIdle;       // Request  Idle  Flag

static volatile uint16_t USB_ResponseIndexI;    // Response Index In
static volatile uint16_t USB_ResponseIndexO;    // Response Index Out
static volatile uint16_t USB_ResponseCountI;    // Response Count In
static volatile uint16_t USB_ResponseCountO;    // Response Count Out
static volatile uint8_t  USB_ResponseIdle;      // Response Idle  Flag

static uint8_t  USB_Request [DAP_PACKET_COUNT][DAP_PACKET_SIZE];  // Request  Buffer
static uint8_t  USB_Response[DAP_PACKET_COUNT][DAP_PACKET_SIZE];  // Response Buffer
static uint16_t USB_RespSize[DAP_PACKET_COUNT];                                                           // Response Size


static volatile bool isThreadRdy = false; 

static uint32_t USBReceiveEventCallback(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgParam, void *pvMsgData){
    uint32_t n;

    switch(ui32Event){
        /* The device is now attached to a USB host and ready to begin sending and receiving data */
        case USB_EVENT_CONNECTED:
            break;
        /* The device has been disconnected from the USB host */
        case USB_EVENT_DISCONNECTED:
            break;
        /* Data has been received and is in the buffer provided or is ready to be read from the FIFO. */
        case USB_EVENT_RX_AVAILABLE:
            n = USBDHIDPacketRead(&g_sHIDDAPDevice, &USB_Request[USB_RequestIndexI], DAP_PACKET_SIZE, false);
            if (n != 0U) {
                if (USB_Request[USB_RequestIndexI][0] == ID_DAP_TransferAbort) {
                    DAP_TransferAbort = 1U;
                } else {
                USB_RequestIndexI++;
                if (USB_RequestIndexI == DAP_PACKET_COUNT) {
                    USB_RequestIndexI = 0U;
                }
                USB_RequestCountI++;
                isThreadRdy = true;
                }
            }
            // Start reception of next request packet
            if ((uint16_t)(USB_RequestCountI - USB_RequestCountO) != DAP_PACKET_COUNT) {
                USBDHIDPacketRead(&g_sHIDDAPDevice, &USB_Request[USB_RequestIndexI], DAP_PACKET_SIZE, false);
            } else {
                USB_RequestIdle = 1U;
            }
            break;
        /* This event indicates to an application that a report idle timeout has occurred and requests a 
        pointer to the report that must be sent back to the host. */
        case USBD_HID_EVENT_IDLE_TIMEOUT:
            break;
        /* This event indicates that the host has sent a Set_Report request to the device and requests that
        the device provide a buffer into which the report can be written */
        case USBD_HID_EVENT_GET_REPORT_BUFFER:
            break;
        /* This event indicates that the host is requesting a particular report be returned via endpoint
        0, the control endpoint. */
        case USBD_HID_EVENT_GET_REPORT:
            break;
        /* This event is sent in response to a Set_Protocol request from the host. */
        case USBD_HID_EVENT_SET_PROTOCOL:
            break;
        /* This event is sent in respon se to a Get_Protocol request from the host. */
        case USBD_HID_EVENT_GET_PROTOCOL:
            break;
        /* This event indicates that the host has sent the device a report via endpoint 0, the control
        endpoint. */
        case USBD_HID_EVENT_SET_REPORT:
            break;
    }
    /* Events missing: 
    USB_EVENT_ERROR
    USB_EVENT_SUSPEND
    USB_EVENT_RESUME
    USBD_HID_EVENT_REPORT_SENT */
    return(0);
}

static uint32_t USBTransmitEventCallback(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgParam, void *pvMsgData){
        switch(ui32Event){
        /* Data has been sent and acknowledged. */
        case USB_EVENT_TX_COMPLETE:
            if (USB_ResponseCountI != USB_ResponseCountO) {
                // Load data from response buffer to be sent back
                USBDHIDReportWrite(&g_sHIDDAPDevice, &USB_Response[USB_ResponseIndexO], USB_RespSize[USB_ResponseIndexO], false);
                USB_ResponseIndexO++;
                if (USB_ResponseIndexO == DAP_PACKET_COUNT) {
                    USB_ResponseIndexO = 0U;
                }
                USB_ResponseCountO++;
            } else {
                USB_ResponseIdle = 1U;
            }
            break;
    }
}

// DAP Thread.
void DAP_Thread (void) {
    uint32_t flags;
    uint32_t n;

    for (;;) {
        while(!isThreadRdy){}
        isThreadRdy = false;

        // Process pending requests
        while (USB_RequestCountI != USB_RequestCountO) {

            // Handle Queue Commands
            n = USB_RequestIndexO;
            while (USB_Request[n][0] == ID_DAP_QueueCommands) {
                USB_Request[n][0] = ID_DAP_ExecuteCommands;
                n++;
                if (n == DAP_PACKET_COUNT) {
                    n = 0U;
                }
                if (n == USB_RequestIndexI) {
                    while(!isThreadRdy){}
                    isThreadRdy = false;
                    if (flags & 0x80U) {
                        break;
                    }
                }
            }

            // Execute DAP Command (process request and prepare response)
            USB_RespSize[USB_ResponseIndexI] =
                (uint16_t)DAP_ExecuteCommand(USB_Request[USB_RequestIndexO], USB_Response[USB_ResponseIndexI]);

            // Update Request Index and Count
            USB_RequestIndexO++;
            if (USB_RequestIndexO == DAP_PACKET_COUNT) {
                USB_RequestIndexO = 0U;
            }
            USB_RequestCountO++;

            if (USB_RequestIdle) {
                if ((uint16_t)(USB_RequestCountI - USB_RequestCountO) != DAP_PACKET_COUNT) {
                USB_RequestIdle = 0U;
                USBDHIDPacketRead(&g_sHIDDAPDevice, &USB_Request[USB_RequestIndexI], DAP_PACKET_SIZE, false);
                }
            }

            // Update Response Index and Count
            USB_ResponseIndexI++;
            if (USB_ResponseIndexI == DAP_PACKET_COUNT) {
                USB_ResponseIndexI = 0U;
            }
            USB_ResponseCountI++;

            if (USB_ResponseIdle) {
                if (USB_ResponseCountI != USB_ResponseCountO) {
                    // Load data from response buffer to be sent back
                    n = USB_ResponseIndexO++;
                    if (USB_ResponseIndexO == DAP_PACKET_COUNT) {
                        USB_ResponseIndexO = 0U;
                    }
                    USB_ResponseCountO++;
                    USB_ResponseIdle = 0U;
                    USBDHIDReportWrite(&g_sHIDDAPDevice, &USB_Response[n], USB_RespSize[n], false);
                }
            }
        }
    }
}

void USB_init(void){
    uint32_t* pvRetcode = NULL;
    pvRetcode = USBDHIDInit(ui32Index, &g_sHIDDAPDevice);
}
