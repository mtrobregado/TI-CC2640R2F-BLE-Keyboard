/******************************************************************************

 @file       hid_keyboard.h

 @brief This file contains the HID Keyboard sample application for use in
        TI CC2640R2F.

 Target Device: CC2640R2
 
 Maker/Author - Markel T. Robregado
 Date: January 4, 2018

 *****************************************************************************/

#ifndef HIDEMUKBD_H
#define HIDEMUKBD_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * MACROS
 */

#define KEY_NONE                    0x00

// Selected HID LED bitmaps
#define LED_NUM_LOCK                0x01
#define LED_CAPS_LOCK               0x02

// Selected HID mouse button values
#define MOUSE_BUTTON_1              0x01
#define MOUSE_BUTTON_NONE           0x00

// HID keyboard input report length
#define HID_KEYBOARD_IN_RPT_LEN     8

// HID LED output report length
#define HID_LED_OUT_RPT_LEN         1

// HID mouse input report length
#define HID_MOUSE_IN_RPT_LEN        5

/*********************************************************************
 * CONSTANTS
 */

// HID idle timeout in msec; set to zero to disable timeout
#define DEFAULT_HID_IDLE_TIMEOUT              60000

// Minimum connection interval (units of 1.25ms) if automatic parameter update
// request is enabled.
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL     8

// Maximum connection interval (units of 1.25ms) if automatic parameter update
// request is enabled.
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL     8

// Slave latency to use if automatic parameter update request is enabled
#define DEFAULT_DESIRED_SLAVE_LATENCY         50

// Supervision timeout value (units of 10ms) if automatic parameter update
// request is enabled.
#define DEFAULT_DESIRED_CONN_TIMEOUT          500

// Whether to enable automatic parameter update request when a connection is
// formed.
#define DEFAULT_ENABLE_UPDATE_REQUEST         GAPROLE_LINK_PARAM_UPDATE_INITIATE_BOTH_PARAMS

// Connection Pause Peripheral time value (in seconds)
#define DEFAULT_CONN_PAUSE_PERIPHERAL         10

// Default passcode
#define DEFAULT_PASSCODE                      0

// Default GAP pairing mode
#define DEFAULT_PAIRING_MODE                  GAPBOND_PAIRING_MODE_INITIATE

// Default MITM mode (TRUE to require passcode or OOB when pairing)
#define DEFAULT_MITM_MODE                     FALSE

// Default bonding mode, TRUE to bond
#define DEFAULT_BONDING_MODE                  TRUE

// Default GAP bonding I/O capabilities
#define DEFAULT_IO_CAPABILITIES               GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT

// Battery level is critical when it is less than this %
#define DEFAULT_BATT_CRITICAL_LEVEL           6

// Key bindings, can be modified to any HID value.
#ifndef CC2650_LAUNCHXL
#define KEY_UP_HID_BINDING                    HID_KEYBOARD_UP_ARROW
#define KEY_DOWN_HID_BINDING                  HID_KEYBOARD_DOWN_ARROW
#define KEY_SELECT_HID_BINDING                MOUSE_BUTTON_1
#define USE_HID_MOUSE
#endif // !CC2650_LAUNCHXL
#define KEY_LEFT_HID_BINDING                  HID_KEYBOARD_LEFT_ARROW
#define KEY_RIGHT_HID_BINDING                 HID_KEYBOARD_RIGHT_ARROW

// Task configuration
#define HIDKEYBOARD_TASK_PRIORITY               1

#ifndef HIDKEYBOARD_TASK_STACK_SIZE
#define HIDKEYBOARD_TASK_STACK_SIZE             644
#endif


// Task Events
#define HIDKEYBOARD_ICALL_EVT                   ICALL_MSG_EVENT_ID // Event_Id_31
#define HIDKEYBOARD_QUEUE_EVT                   UTIL_QUEUE_EVENT_ID // Event_Id_30
#define HIDKBD_GPIO_KEY_DETECT_EVT              Event_Id_00
#define HIDKBD_GPIO_KEY_SCAN_EVT                Event_Id_01
#define HIDKBD_PASSCODE_STATE_EVT               Event_Id_02


#define HIDKEYBOARD_ALL_EVENTS                  (HIDKEYBOARD_ICALL_EVT | \
                                                 HIDKBD_GPIO_KEY_DETECT_EVT | \
                                                 HIDKBD_GPIO_KEY_SCAN_EVT   | \
                                                 HIDKBD_PASSCODE_STATE_EVT  | \
                                                 HIDKEYBOARD_QUEUE_EVT)

// Release timeout in Milliseconds
#define KEY_RELEASE_TIMEOUT                         4

// Debounce timeout in Milliseconds
#define KEY_DEBOUNCE_TIMEOUT                        4

// Standard Bitmaps for Modifier keys.
// These are used to generate modifier byte for the keyboard Report.
#define HIDKBD_LEFTCTRL_BM_EN                       (0x01)
#define HIDKBD_LEFTSHIFT_BM_EN                      (0x01<<1)
#define HIDKBD_LEFTALT_BM_EN                        (0x01<<2)
#define HIDKBD_LEFTGUI_BM_EN                        (0x01<<3)
#define HIDKBD_RIGHTCTRL_BM_EN                      (0x01<<4)
#define HIDKBD_RIGHTSHIFT_BM_EN                     (0x01<<5)
#define HIDKBD_RIGHTALT_BM_EN                       (0x01<<6)
#define HIDKBD_RIGHTGUI_BM_EN                       (0x01<<7)

#define KEY_NOT_VALID       255
#define IS_VALID_KEY(i)     (gKeyInfo.keysDetected[i] != 255)
#define NEW_KEY(i)          (gKeyInfo.keysDetected[i] != KEY_NOT_VALID) && (gKeyInfo.keysSendtoHost[i] == FALSE)
/*********************************************************************
 * FUNCTIONS
 */
  
/*********************************************************************
 * GLOBAL VARIABLES
 */

/*
 * Task creation function for the HID emulated keyboard.
 */
extern void HIDKeyboard_CreateTask(void);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /*HIDEMUKBD_H */
