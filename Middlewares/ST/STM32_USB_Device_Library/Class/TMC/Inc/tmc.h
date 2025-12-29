/* 
 * File:   tmc.h
 * Author: karlp
 *
 * USB TMC definitions
 */

#ifndef TMC_H
#define TMC_H

#ifdef __cplusplus
extern "C" {
#endif

    /* Definitions of Test & Measurement Class from
     * "Universal Serial Bus Test and Measurement Class
     * Revision 1.0"
     */

#define USBTMC_VERSION      0x0100
#define USB488_VERSION      0x0100

    /* Table 43: TMC Class Code */
    /*
     * Application-Class” class code, assigned by USB-IF. The Host must 
     * not load a USBTMC driver based on just the bInterfaceClass field.
     */
#define USB_IFACE_CLASS               0xFE
#define USB_IFACE_SUBCLASS_TMC        0x03

    /* Table 44 */
#define USB_TMC_PROTOCOL_NONE               0
#define USB_TMC_PROTOCOL_USB488             1

    /* USB TMC Class Specific Requests, Table 15 sec 4.2.1 */
    /* These are all required */
#define USB_TMC_REQ_INITIATE_ABORT_BULK_OUT         1u
#define USB_TMC_REQ_CHECK_ABORT_BULK_OUT_STATUS     2u
#define USB_TMC_REQ_INITIATE_ABORT_BULK_IN          3u
#define USB_TMC_REQ_CHECK_ABORT_BULK_IN_STATUS      4u
#define USB_TMC_REQ_INITIATE_CLEAR                  5u
#define USB_TMC_REQ_CHECK_CLEAR_STATUS              6u
#define USB_TMC_REQ_GET_CAPABILITIES                7u
#define USB_TMC_REQ_INDICATOR_PULSE                 64u /* optional */

    /* USB488 */
    /* 4.3 USB488 subclass specific requests */
#define USB_488_REQ_READ_STATUS_BYTE                128u
#define USB_488_REQ_REN_CONTROL                     160u
#define USB_488_REQ_GO_TO_LOCAL                     161u
#define USB_488_REQ_LOCAL_LOCKOUT                   162u


    /* USB TMC status values Table 16 */
#define USB_TMC_STATUS_SUCCESS                      1
#define USB_TMC_STATUS_PENDING                      2
#define USB_TMC_STATUS_FAILED                       0x80
#define USB_TMC_STATUS_TRANSFER_NOT_IN_PROGRESS     0x81
#define USB_TMC_STATUS_SPLIT_NOT_IN_PROGRESS        0x82
#define USB_TMC_STATUS_SPLIT_IN_PROGRESS            0x83

#define USB_488_STATUS_INTERRUPT_IN_BUSY            0x20

    /* Table 1 -- USBTMC message Bulk-OUT Header */
    /* Table 8 -- USBTMC Bulk-IN Header */
    struct usb_tmc_bulk_header {
        uint8_t MsgID;
        uint8_t bTag;
        uint8_t bTagInverse;
        uint8_t reserved;

        union {
            /** 3.2 Bulk-OUT endpoint */
            /* 3.2.1 Bulk-OUT USBTMC command messages */
            /* 3.2.1.1 MsgID = DEV_DEP_MSG_OUT */
            struct _dev_dep_msg_out {
                uint32_t TransferSize;
                uint8_t bmTransferAttributes;       // D0: EOM
                uint8_t reserved[3];
            } dev_dep_msg_out;
            /* 3.2.1.2 MsgID = REQUEST_DEV_DEP_MSG_IN */
            struct _req_dev_dep_msg_in {
                uint32_t TransferSize;
                uint8_t bmTransferAttributes;       // D0: Must be 0; D1: TermCharEnabled
                uint8_t TermChar;
                uint8_t reserved[2];
            } req_dev_dep_msg_in;
            /* 3.2.1.3 MsgID = VENDOR_SPECIFIC_OUT */
            struct _vendor_specific_out {
                uint32_t TransferSize;
                uint8_t reserved[4];
            } vendor_specific_out;
            /* 3.2.1.4 MsgID = REQUEST_VENDOR_SPECIFIC_IN */
            struct _req_vendor_specific_in {
                uint32_t TransferSize;
                uint8_t reserved[4];
            } req_vendor_specific_in;

            /** 3.3 Bulk-IN endpoint */
            /** 3.3.1 Bulk-IN USBTMC response messages */
            /* 3.3.1.1 MsgID = DEV_DEP_MSG_IN */
            struct _dev_dep_msg_in {
                uint32_t TransferSize;
                uint8_t bmTransferAttributes;       // D0: EOM; D1: TermChar
                uint8_t reserved[3];
            } dev_dep_msg_in;
            /* 3.3.1.2 MsgID = VENDOR_SPECIFIC_IN */
            struct _vendor_specific_in {
                uint32_t TransferSize;
                uint8_t reserved[4];
            } vendor_specific_in;
            uint8_t raw[8];
        } command_specific;

    } __attribute__((packed));

    /* Table 2, MsgId values */
#define USB_TMC_MSGID_OUT_DEV_DEP_MSG_OUT               1
#define USB_TMC_MSGID_OUT_REQUEST_DEV_DEP_MSG_IN        2
#define USB_TMC_MSGID_IN_DEV_DEP_MSG_IN                 2
#define USB_TMC_MSGID_OUT_VENDOR_SPECIFIC_OUT           126
#define USB_TMC_MSGID_OUT_REQUEST_VENDOR_SPECIFIC_IN    127
#define USB_TMC_MSGID_IN_VENDOR_SPECIFIC_IN             127

    /* USB488 */
    /* Table 1 -- USB488 defined MsgID values */
#define USB_488_MSGID_OUT_TRIGGER_MSG_OUT               128

    /* Table 4 -- REQUEST_DEV_DEP_MSG_IN Bulk-OUT Header with command specific content */
    /* Table 9 -- DEV_DEP_MSG_IN Bulk-IN Header with response specific content */
#define USB_TMC_BULK_HEADER_BMTRANSFER_ATTRIB_EOM       (1<<0)
#define USB_TMC_BULK_HEADER_BMTRANSFER_ATTRIB_TERMCHAR  (1<<1)

    /* 4.2.1.8 GET_CAPABILITIES */
    /* Table 37 -- GET_CAPABILITIES response format */
#define USB_TMC_INTERFACE_CAPABILITY_D2_INDICATOR_PULSE (1<<2)
#define USB_TMC_INTERFACE_CAPABILITY_D1_TALK_ONLY       (1<<1)
#define USB_TMC_INTERFACE_CAPABILITY_D0_LISTEN_ONLY     (1<<0)
#define USB_TMC_DEVICE_CAPABILITY_D0_TERMCHAR           (1<<0)

#define USB_488_INTERFACE_CAPABILITY_D2_USB488_2        (1<<2)
#define USB_488_INTERFACE_CAPABILITY_D1_REN_CONTROL     (1<<1)
#define USB_488_INTERFACE_CAPABILITY_D0_TRIGGER         (1<<0)
#define USB_488_DEVICE_CAPABILITY_D3_ALLSCPI            (1<<3)
#define USB_488_DEVICE_CAPABILITY_D2_SR                 (1<<2)
#define USB_488_DEVICE_CAPABILITY_D1_RL                 (1<<1)
#define USB_488_DEVICE_CAPABILITY_D0_DT                 (1<<0)

    /* USBTMC */
    /* Table 37 -- GET_CAPABILITIES response format */
struct  __attribute__ ((packed))  usbtmc_capabilities {
    uint8_t USBTMC_status;
    uint8_t reserved1;
    uint16_t bcdUSBTMC;
    uint8_t bmIfCapabilities;  /* bitmap! */
    uint8_t bmDevCapabilities;  /* bitmap! */
    uint8_t reserved2[6];
    /* USB488 */
    /* Table 8 -- GET_CAPABILITIES response packet */
    uint16_t bcdUSB488;
    uint8_t bmIfCapabilities488;  /* bitmap! */
    uint8_t bmDevCapabilities488;  /* bitmap! */
    uint8_t reserved3[8];
};

// Used for check clear status
typedef struct
{
  uint8_t USBTMC_status;
  uint8_t bmClear;
} usbtmc_check_clear_status_rsp_t;

// Used for both abort bulk IN and bulk OUT
typedef struct
{
  uint8_t USBTMC_status;
  uint8_t bTag;
} usbtmc_initiate_abort_rsp_t;

// Used for both check_abort_bulk_in_status and check_abort_bulk_out_status
typedef struct
{
  uint8_t USBTMC_status;
  uint8_t bmAbortBulkIn;
  uint8_t _reserved[2];               ///< Must be zero
  uint32_t NBYTES_RXD_TXD;
} usbtmc_check_abort_bulk_rsp_t;

// Used for both abort bulk IN and bulk OUT
typedef struct
{
  uint8_t USBTMC_status;
  uint8_t bTag;
  uint8_t Constant;
} usbtmc_read_status_byte_rsp_t;

#ifdef    __cplusplus
}
#endif

#endif    /* TMC_H */

