/******************************************************************************

 @file       hid_keyboard.c

 @brief This file contains the HID Keyboard sample application for use in
        TI CC2640R2F.

 Target Device: CC2640R2

 Maker/Author - Markel T. Robregado
 Date: January 4, 2018
 
 *****************************************************************************/

/*********************************************************************
 * INCLUDES
 */

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Queue.h>

#include "gatt.h"
#include "gapgattserver.h"
#include "gattservapp.h"
#include "gatt_profile_uuid.h"
#include "devinfoservice.h"
#include "battservice.h"
#include "hidkbdservice.h"
#include "hiddev.h"
#include "ll_common.h"

#include "peripheral.h"
#include "gapbondmgr.h"

#include "osal_snv.h"
#include "icall_apimsg.h"

#include "util.h"

#ifdef USE_CORE_SDK
  #include <ti/display/Display.h>
#else // !USE_CORE_SDK
  #include <ti/mw/display/Display.h>
#endif // USE_CORE_SDK

#include "Board_keyConfig.h"
#include "Board.h"
#include "hid_keyboard.h"
#include "Board_HIDUsageKey.h"

#include "icall_api.h"
/*********************************************************************
 * MACROS
 */


/*********************************************************************
 * TYPEDEFS
 */

// App event passed from profiles.
typedef struct
{
  appEvtHdr_t hdr; // Event header
} hidEmuKbdEvt_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Display Interface
//Display_Handle dispHandle = NULL;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

// Entity ID globally used to check for source and/or destination of messages
static ICall_EntityID selfEntity;

// Event globally used to post local events and pend on system and
// local events.
static ICall_SyncHandle syncEvent;

// Clock instances for internal periodic events.
static Clock_Struct keyRelease;
static Clock_Struct keyDetect;

// Queue object used for app messages
static Queue_Struct appMsg;
static Queue_Handle appMsgQueue;

uint16_t HidKbdConnHdl;

uint8_t gInputPasscode;
uint8_t gDisplayPasscode;

// Task configuration
Task_Struct HidKeyboardTask;
Char HidKeyboardTaskStack[HIDKEYBOARD_TASK_STACK_SIZE];

keyInfo_t gKeyInfo;
uint8_t gKeysDetected[MAX_KEY_LIST];
bool gNewKeyDetected = FALSE;

uint8 gSpecialKey[MAX_KEY_LIST];
uint8 gSpecialKeyDetected = FALSE;

uint8 gPasscodeEntryStart = FALSE;
uint8 gPasscodeEntryStop  = FALSE;
uint32 gPasscode = 0;
uint8 gPasscodeDigitCount = 0;

uint8 gSystemState = 0xFF;

// GAP Profile - Name attribute for SCAN RSP data
static uint8_t scanData[] =
{
    0x0D,                             // length of this data
    GAP_ADTYPE_LOCAL_NAME_COMPLETE,   // AD Type = Complete local name
    'H',
    'I',
    'D',
    ' ',
    'K',
    'e',
    'y',
    'b',
    'o',
    'a',
    'r',
    'd'
};

// Advertising data
static uint8_t advData[] =
{
    // flags
    0x02,   // length of this data
    GAP_ADTYPE_FLAGS,
    GAP_ADTYPE_FLAGS_LIMITED | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

    // appearance
    0x03,   // length of this data
    GAP_ADTYPE_APPEARANCE,
    LO_UINT16(GAP_APPEARE_HID_KEYBOARD),
    HI_UINT16(GAP_APPEARE_HID_KEYBOARD),

    // service UUIDs
    0x05,   // length of this data
    GAP_ADTYPE_16BIT_MORE,
    LO_UINT16(HID_SERV_UUID),
    HI_UINT16(HID_SERV_UUID),
    LO_UINT16(BATT_SERV_UUID),
    HI_UINT16(BATT_SERV_UUID)
};

// Device name attribute value
static CONST uint8_t attDeviceName[GAP_DEVICE_NAME_LEN] = "HID Keyboard";

// HID Dev configuration
static hidDevCfg_t HidKeyBoardCfg =
{
    DEFAULT_HID_IDLE_TIMEOUT,   // Idle timeout
    HID_KBD_FLAGS               // HID feature flags
};


/*********************************************************************
 * LOCAL FUNCTIONS
 */

// Application task and event processing.
static void HidKeyboard_Init(void);
static void HidKeyboard_TaskFxn(UArg a0, UArg a1);
static void HidKeyboard_ProcessAppMsg(hidEmuKbdEvt_t *pMsg);
static void HidKeyboard_ProcessStackMsg(ICall_Hdr *pMsg);
static void HidKeyboard_ProcessGattMsg(gattMsgEvent_t *pMsg);

//not going to be used
#if 0
static uint8_t HidKeyboard_EnqueueMsg(uint16_t event, uint8_t state);
#endif
//from hidkeyboard.c reference design added by markel
static void HidKeyBoard_InitKeyInfo(void);
// Key press.
static void HidKeyBoard_KeyDetectHandler(UArg a0);
static void HidKeyBoard_KeyReleaseHandler(UArg a0);

static void HidKeyBoard_HandleKeys(uint8_t keys);
static void HidKeyBoard_ProcessFuncKey(uint8_t keycode);
static void HidKeyBoard_CheckModifierByteStatus(void);
// Key Press ISR Call Back Handler.
void HidKeyBoard_KeyDetected(uint8_t iPinIndex);

// HID reports.
static void HidKeyBoard_SendReport( uint8_t keycode );
static uint8_t HidKeyboard_ReceiveReport(uint8_t len, uint8_t *pData);
static uint8_t HidKeyboard_ReportCB(uint8_t id, uint8_t type, uint16_t uuid,
                                  uint8_t oper, uint16_t *pLen, uint8_t *pData);

static void HidKeyboard_HidEventCB(uint8_t evt);

static void HidKeyboard_ProcessPasscode(uint16_t connHandle, uint8_t uiInputs, uint8_t uiOutputs);

static void HidKeyboard_PassCodeCB(uint8_t *deviceAddr,uint16_t connHandle,
                                   uint8_t uiInputs, uint8_t uiOutputs );

static uint8 HidKeyboard_CheckKeyReleased(uint8_t key);

/*********************************************************************
 * PROFILE CALLBACKS
 */

static hidDevCB_t HidKeyBoardHidCBs =
{
    HidKeyboard_ReportCB,
    HidKeyboard_HidEventCB,
    HidKeyboard_PassCodeCB
};

static keyDetectCBs_t HidKeyBoard_keyDetectCB =
{
    HidKeyBoard_KeyDetected,
    NULL
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      HidEmuKbd_createTask
 *
 * @brief   Task creation function for the HID emulated keyboard.
 *
 * @param   none
 *
 * @return  none
 */
void HIDKeyboard_CreateTask(void)
{
    Task_Params taskParams;

    // Configure task
    Task_Params_init(&taskParams);
    taskParams.stack = HidKeyboardTaskStack;
    taskParams.stackSize = HIDKEYBOARD_TASK_STACK_SIZE;
    taskParams.priority = HIDKEYBOARD_TASK_PRIORITY;

    Task_construct(&HidKeyboardTask, HidKeyboard_TaskFxn, &taskParams, NULL);
}

/*********************************************************************
 * @fn      HidEmuKbd_init
 *
 * @brief   Initialization function for the HidEmuKbd App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notification ...).
 *
 * @param   none
 *
 * @return  none
 */
static void HidKeyboard_Init(void)
{
    // ******************************************************************
    // N0 STACK API CALLS CAN OCCUR BEFORE THIS CALL TO ICall_registerApp
    // ******************************************************************
    // Register the current thread as an ICall dispatcher application
    // so that the application can send and receive messages.
    ICall_registerApp(&selfEntity, &syncEvent);

    Board_shutDownExtFlash();

    // Initialize the KeyInfo Structure
    HidKeyBoard_InitKeyInfo();

    Board_RegisterKeyCallBack(&HidKeyBoard_keyDetectCB);

    // Create an RTOS queue for message from profile to be sent to app.
    appMsgQueue = Util_constructQueue(&appMsg);

    // Setup keycallback for key input release
    Util_constructClock(&keyRelease,
                        HidKeyBoard_KeyReleaseHandler,
                        KEY_RELEASE_TIMEOUT,
                        0,
                        false,
                        HIDKBD_GPIO_KEY_SCAN_EVT);

    // Setup keycallback for key input re detection
    Util_constructClock(&keyDetect,
                        HidKeyBoard_KeyDetectHandler,
                        KEY_DEBOUNCE_TIMEOUT,
                        0,
                        false,
                        HIDKBD_GPIO_KEY_SCAN_EVT);

    // Setup the GAP
    VOID GAP_SetParamValue(TGAP_CONN_PAUSE_PERIPHERAL,
                         DEFAULT_CONN_PAUSE_PERIPHERAL);

    // Setup the GAP Peripheral Role Profile
    {
        uint8_t initial_advertising_enable = TRUE;

        // By setting this to zero, the device will go into the waiting state after
        // being discoverable for 30.72 second, and will not being advertising again
        // until the enabler is set back to TRUE
        uint16_t gapRole_AdvertOffTime = 0;
#if 0
        uint8_t advertChannel = GAP_ADVCHAN_ALL; // review if needed
#endif

        uint8_t enable_update_request = DEFAULT_ENABLE_UPDATE_REQUEST;
        uint16_t desired_min_interval = DEFAULT_DESIRED_MIN_CONN_INTERVAL;
        uint16_t desired_max_interval = DEFAULT_DESIRED_MAX_CONN_INTERVAL;
        uint16_t desired_slave_latency = DEFAULT_DESIRED_SLAVE_LATENCY;
        uint16_t desired_conn_timeout = DEFAULT_DESIRED_CONN_TIMEOUT;

        // Set the GAP Role Parameters
        GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t),
                             &initial_advertising_enable);
        GAPRole_SetParameter(GAPROLE_ADVERT_OFF_TIME, sizeof(uint16_t),
                             &gapRole_AdvertOffTime);

        GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advData), advData);
        GAPRole_SetParameter(GAPROLE_SCAN_RSP_DATA, sizeof(scanData), scanData);
#if 0
        GAPRole_SetParameter(GAPROLE_ADV_CHANNEL_MAP, sizeof(uint8_t),
                             &advertChannel);
#endif

        GAPRole_SetParameter(GAPROLE_PARAM_UPDATE_ENABLE, sizeof(uint8_t),
                             &enable_update_request);
        GAPRole_SetParameter(GAPROLE_MIN_CONN_INTERVAL, sizeof(uint16_t),
                             &desired_min_interval);
        GAPRole_SetParameter(GAPROLE_MAX_CONN_INTERVAL, sizeof(uint16_t),
                             &desired_max_interval);
        GAPRole_SetParameter(GAPROLE_SLAVE_LATENCY, sizeof(uint16_t),
                             &desired_slave_latency);
        GAPRole_SetParameter(GAPROLE_TIMEOUT_MULTIPLIER, sizeof(uint16_t),
                             &desired_conn_timeout);
    }

    // Set the GAP Characteristics
    GGS_SetParameter(GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN,
                   (void *)attDeviceName);

    // Setup the GAP Bond Manager
    {
        uint32_t passkey = DEFAULT_PASSCODE;
        uint8_t pairMode = DEFAULT_PAIRING_MODE;
        uint8_t mitm = DEFAULT_MITM_MODE;
        uint8_t ioCap = DEFAULT_IO_CAPABILITIES;
        uint8_t bonding = DEFAULT_BONDING_MODE;

        GAPBondMgr_SetParameter(GAPBOND_DEFAULT_PASSCODE, sizeof(uint32_t),
                                &passkey);
        GAPBondMgr_SetParameter(GAPBOND_PAIRING_MODE, sizeof(uint8_t), &pairMode);
        GAPBondMgr_SetParameter(GAPBOND_MITM_PROTECTION, sizeof(uint8_t), &mitm);
        GAPBondMgr_SetParameter(GAPBOND_IO_CAPABILITIES, sizeof(uint8_t), &ioCap);
        GAPBondMgr_SetParameter(GAPBOND_BONDING_ENABLED, sizeof(uint8_t), &bonding);
    }

    // Setup Battery Characteristic Values
    {
        uint8_t critical = DEFAULT_BATT_CRITICAL_LEVEL;

        Batt_SetParameter(BATT_PARAM_CRITICAL_LEVEL, sizeof (uint8_t), &critical);
    }

    // Set up HID keyboard service
    HidKbd_AddService();

    // Register for HID Dev callback
    HidDev_Register(&HidKeyBoardCfg, &HidKeyBoardHidCBs);

    // Start the GAP Role and Register the Bond Manager.
    HidDev_StartDevice();

    // Initialize keys on the Key Board
    Board_initKeys();

    // Register with GAP for HCI/Host messages
    GAP_RegisterForMsgs(selfEntity);
  
#if !defined (USE_LL_CONN_PARAM_UPDATE)
    // Get the currently set local supported LE features
    // The HCI will generate an HCI event that will get received in the main
    // loop
    HCI_LE_ReadLocalSupportedFeaturesCmd();
#endif // !defined (USE_LL_CONN_PARAM_UPDATE)
  
}

/*********************************************************************
 * @fn      HidKeyboard_TaskFxn
 *
 * @brief   HidKeyboard Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   a0, a1 - not used.
 *
 * @return  none
 */
void HidKeyboard_TaskFxn(UArg a0, UArg a1)
{
    uint8_t i, iCnt, iClockRestart, iFound = 0;

    // Initialize the application.
    HidKeyboard_Init();

    // Application main loop.
    for (;;)
    {
        uint32_t events;

        events = Event_pend(syncEvent, Event_Id_NONE, HIDKEYBOARD_ALL_EVENTS,
                            ICALL_TIMEOUT_FOREVER);

        if (events)
        {
            ICall_EntityID dest;
            ICall_ServiceEnum src;
            ICall_HciExtEvt *pMsg = NULL;

            if (ICall_fetchServiceMsg(&src, &dest,
                                    (void **)&pMsg) == ICALL_ERRNO_SUCCESS)
            {
                if ((src == ICALL_SERVICE_CLASS_BLE) && (dest == selfEntity))
                {
                    // Process inter-task message
                    HidKeyboard_ProcessStackMsg((ICall_Hdr *)pMsg);
                }

                if (pMsg)
                {
                    ICall_freeMsg(pMsg);
                }
            }

            // If RTOS queue is not empty, process app message.
            if (events & HIDKEYBOARD_QUEUE_EVT)
            {
                while (!Queue_empty(appMsgQueue))
                {
                    hidEmuKbdEvt_t *pMsg = (hidEmuKbdEvt_t *)Util_dequeueMsg(appMsgQueue);
                    if (pMsg)
                    {
                        // Process message.
                        HidKeyboard_ProcessAppMsg(pMsg);

                        // Free the space from the message.
                        ICall_free(pMsg);
                    }
                }
            }

            if (events & HIDKBD_PASSCODE_STATE_EVT)
            {
                HidKeyboard_ProcessPasscode(HidKbdConnHdl, gInputPasscode, gDisplayPasscode);
            }

            if (events & HIDKBD_GPIO_KEY_SCAN_EVT)
            {
                gKeyInfo.newKeyDetected = FALSE;

                for(i=0; i<MAX_KEY_LIST; i++)
                {
                    gKeysDetected[i] = KEY_NOT_VALID;
                }

                // Detect all key presses in the particular column
                // The Board_ScanKeys scans the particular column of the key, and updates the gKeysDetected
                // array with keys pressed in the column
                Board_scanKeys(gKeysDetected, &gNewKeyDetected);

                //Check Each Key in Key Detection Table and update Table
                for(i=0; i<MAX_KEY_LIST; i++)
                {
                    if(gKeyInfo.keysDetected[i] != KEY_NOT_VALID)
                    {
                        iFound = FALSE;

                        //Check if current key should to be removed from key detection list.
                        for(iCnt=0; iCnt<MAX_KEY_LIST; iCnt++)
                        {
                            if((gKeysDetected[iCnt] == gKeyInfo.keysDetected[i]) && (gKeyInfo.keysSendtoHost[i] == TRUE))
                            {
                                iFound = TRUE;
                                gKeysDetected[iCnt] = KEY_NOT_VALID;   //Remove already detected keys from list. Keep only new keys
                                break;
                            }
                        }

                        //Check for Key Release
                        if(iFound == FALSE)
                        {
                            if(gKeyInfo.keyReleaseDebounce[i] == TRUE)
                            {
                                // Send Release Key only for last pressed key
                                if(gKeyInfo.lastKey == gKeyInfo.keysDetected[i])
                                {
                                    gKeyInfo.lastKey = KEY_RESERVED;
                                    HidKeyBoard_HandleKeys(KEY_RESERVED);
                                }
                                // Send Release Key only for last pressed key
                                if(gKeyInfo.lastKey == gKeyInfo.keysDetected[i])
                                {
                                    gKeyInfo.lastKey = KEY_RESERVED;
                                    HidKeyBoard_HandleKeys(KEY_RESERVED);
                                }
                                //Remove from key detect list the released keys
                                gKeyInfo.keysSendtoHost[i] = FALSE;
                                gKeyInfo.keysDetected[i] = KEY_NOT_VALID;
                                gKeyInfo.keyReleaseDebounce[i] = FALSE;
                            }
                            else
                            {
                                //Check for Release Debounce
                                gKeyInfo.keyReleaseDebounce[i] = TRUE;
                            }
                        }
                    }
                }

                //Adding New keys to the Key detection Table
                for(iCnt=0; iCnt<MAX_KEY_LIST; iCnt++)
                {
                    if(gKeysDetected[iCnt] != KEY_NOT_VALID)
                    {
                        if(Board_isGhostKey(gKeysDetected[iCnt], gKeyInfo.keysDetected) == TRUE)
                        {
                            gKeysDetected[iCnt] = KEY_NOT_VALID;
                        }
                        else
                        {
                              //Add New keys to Key Detect List after validating the debounce.
                            for(i=0; i<MAX_KEY_LIST; i++)
                            {
                                if((gKeyInfo.keysDetected[i] == gKeysDetected[iCnt]) && (gKeyInfo.keysSendtoHost[i] == FALSE))
                                {
                                    gKeyInfo.keysSendtoHost[i] = TRUE;
                                    gKeysDetected[iCnt] = KEY_NOT_VALID;
                                    HidKeyBoard_HandleKeys(gKeyInfo.keysDetected[i]);
                                    break;
                                }
                            }

                            if(gKeysDetected[iCnt] != KEY_NOT_VALID)
                            {
                                for(i=0; i<MAX_KEY_LIST; i++)
                                {
                                    if(gKeyInfo.keysDetected[i] == KEY_NOT_VALID)
                                    {
                                        gKeyInfo.keysDetected[i] = gKeysDetected[iCnt];
                                        gKeyInfo.keysSendtoHost[i] = FALSE;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }

                //Restart Release Clock if Any key is still left in Detect List.
                iClockRestart = FALSE;

                for(i=0; i<MAX_KEY_LIST; i++)
                {
                    if(gKeyInfo.keysDetected[i] != KEY_NOT_VALID)
                    {
                        iClockRestart = TRUE;
                        break;
                    }
                }

                if(iClockRestart == TRUE)
                {
                    Util_startClock(&keyRelease);
                }
            }
        }
    }
}

/*********************************************************************
 * @fn      HidKeyboard_ProcessStackMsg
 *
 * @brief   Process an incoming stack message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void HidKeyboard_ProcessStackMsg(ICall_Hdr *pMsg)
{
    switch (pMsg->event)
    {
        case GATT_MSG_EVENT:
            HidKeyboard_ProcessGattMsg((gattMsgEvent_t *) pMsg);
        break;

        case HCI_GAP_EVENT_EVENT:
        {
            // Process HCI message
            switch(pMsg->status)
            {
                case HCI_COMMAND_COMPLETE_EVENT_CODE:
                // Process HCI Command Complete Event
                {

#if !defined (USE_LL_CONN_PARAM_UPDATE)
                    // This code will disable the use of the LL_CONNECTION_PARAM_REQ
                    // control procedure (for connection parameter updates, the
                    // L2CAP Connection Parameter Update procedure will be used
                    // instead). To re-enable the LL_CONNECTION_PARAM_REQ control
                    // procedures, define the symbol USE_LL_CONN_PARAM_UPDATE

                    // Parse Command Complete Event for opcode and status
                    hciEvt_CmdComplete_t* command_complete = (hciEvt_CmdComplete_t*) pMsg;
                    uint8_t   pktStatus = command_complete->pReturnParam[0];
              
                    //find which command this command complete is for
                    switch (command_complete->cmdOpcode)
                    {
                        case HCI_LE_READ_LOCAL_SUPPORTED_FEATURES:
                        {
                            if (pktStatus == SUCCESS)
                            {
                                uint8_t featSet[8];

                                // get current feature set from received event (bits 1-9 of
                                // the returned data
                                memcpy( featSet, &command_complete->pReturnParam[1], 8 );

                                // Clear bit 1 of byte 0 of feature set to disable LL
                                // Connection Parameter Updates
                                CLR_FEATURE_FLAG( featSet[0], LL_FEATURE_CONN_PARAMS_REQ );

                                // Update controller with modified features
                                HCI_EXT_SetLocalSupportedFeaturesCmd( featSet );
                            }
                        }
                        break;

                        default:
                            //do nothing
                        break;
                    }
#endif // !defined (USE_LL_CONN_PARAM_UPDATE)
              
                }
                break;

                default:
                break;
            }
        }
        break;
      
        default:
        // Do nothing
        break;
    }
}

/*********************************************************************
 * @fn      HidKeyboard_ProcessGattMsg
 *
 * @brief   Process GATT messages
 *
 * @return  none
 */
static void HidKeyboard_ProcessGattMsg(gattMsgEvent_t *pMsg)
{
    GATT_bm_free(&pMsg->msg, pMsg->method);
}

/*********************************************************************
 * @fn      HidKeyboard_ProcessAppMsg
 *
 * @brief   Process an incoming task message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void HidKeyboard_ProcessAppMsg(hidEmuKbdEvt_t *pMsg)
{
    switch (pMsg->hdr.event)
    {
        default:
        //Do nothing.
        break;
    }
}


/*********************************************************************
 * @fn      HidKeyboard_ReportCB
 *
 * @brief   HID Dev report callback.
 *
 * @param   id - HID report ID.
 * @param   type - HID report type.
 * @param   uuid - attribute uuid.
 * @param   oper - operation:  read, write, etc.
 * @param   len - Length of report.
 * @param   pData - Report data.
 *
 * @return  GATT status code.
 */
static uint8_t HidKeyboard_ReportCB(uint8_t id, uint8_t type, uint16_t uuid,
                                    uint8_t oper, uint16_t *pLen, uint8_t *pData)
{
    uint8_t status = SUCCESS;

    // Write
    if (oper == HID_DEV_OPER_WRITE)
    {
        if (uuid == REPORT_UUID)
        {
            // Process write to LED output report; ignore others
            if (type == HID_REPORT_TYPE_OUTPUT)
            {
                status = HidKeyboard_ReceiveReport(*pLen, pData);
            }
        }

        if (status == SUCCESS)
        {
            status = HidKbd_SetParameter(id, type, uuid, *pLen, pData);
        }
    }
    // Read
    else if (oper == HID_DEV_OPER_READ)
    {
        uint8_t len;

        status = HidKbd_GetParameter(id, type, uuid, &len, pData);

        if (status == SUCCESS)
        {
            *pLen = len;
        }
    }
    // Notifications enabled
    else if (oper == HID_DEV_OPER_ENABLE)
    {
        if (id == HID_RPT_ID_MOUSE_IN && type == HID_REPORT_TYPE_INPUT)
        {

        }
    }
    // Notifications disabled
    else if (oper == HID_DEV_OPER_DISABLE)
    {
        if (id == HID_RPT_ID_MOUSE_IN && type == HID_REPORT_TYPE_INPUT)
        {

        }
    }

    return status;
}

/*********************************************************************
 * @fn      HidKeyboard_HidEventCB
 *
 * @brief   HID Dev event callback.
 *
 * @param   evt - event ID.
 *
 * @return  HID response code.
 */
static void HidKeyboard_HidEventCB(uint8_t evt)
{
    // Process enter/exit suspend or enter/exit boot mode
    return;
}

/*********************************************************************
 * @fn      HidKeyboard_EnqueueMsg
 *
 * @brief   Creates a message and puts the message in RTOS queue.
 *
 * @param   event  - message event.
 * @param   state  - message state.
 *
 * @return  TRUE or FALSE
 */
#if 0
static uint8_t HidKeyboard_EnqueueMsg(uint16_t event, uint8_t state)
{
    hidEmuKbdEvt_t *pMsg;

    // Create dynamic pointer to message.
    if (pMsg = ICall_malloc(sizeof(hidEmuKbdEvt_t)))
    {
        pMsg->hdr.event = event;
        pMsg->hdr.state = state;

        // Enqueue the message.
        return Util_enqueueMsg(appMsgQueue, syncEvent, (uint8_t *)pMsg);
    }

    return FALSE;
}
#endif

// from hidkeyboard reference design

/*********************************************************************
 * @fn      HidKeyBoard_InitKeyInfo
 *
 * @brief   Initialization function used to set default values in gKeyInfo
 *
 * @param   none
 *
 * @return  none
 */
static void HidKeyBoard_InitKeyInfo(void)
{
    uint8_t i = 0;

    gKeyInfo.isrPinIndex    = KEY_NOT_VALID;
    gKeyInfo.lastKey        = 0x00;
    gKeyInfo.modifierKeys   = 0x00;
    gKeyInfo.newKeyDetected = FALSE;
    gKeyInfo.play           = FALSE;
    gKeyInfo.funcKey        = FALSE;
    gKeyInfo.numLockState   = FALSE;
    gKeyInfo.capsLockState  = FALSE;

    for(i=0;i<MAX_KEY_LIST;i++)
    {
        gKeysDetected[i]               = KEY_NOT_VALID;
        gKeyInfo.keysDetected[i]       = KEY_NOT_VALID;
        gKeyInfo.keysSendtoHost[i]     = FALSE;
        gKeyInfo.keyDetectDebounce[i]  = FALSE;
        gKeyInfo.keyReleaseDebounce[i] = FALSE;
        gSpecialKey[i]                 = KEY_NOT_VALID;

    }
}


/*********************************************************************
 * @fn      HidKeyboard_CheckKeyReleased
 *
 * @brief   Check Key if Released
 *
 * @param   key
 *
 * @return  bStatus
 */
static uint8 HidKeyboard_CheckKeyReleased(uint8_t key)
{
    uint8_t i = 0;
    uint8_t bStatus = TRUE;

    for(i=0; i<MAX_KEY_LIST; i++)
    {
        if(gKeyInfo.keysDetected[i] == key)
        {
            bStatus = FALSE;
            break;
        }
    }

    return bStatus;
}

/*********************************************************************
 * @fn      HidKeyBoard_HandleKeys
 *
 * @brief   Handles all key events for this device.
 *          Prepares HID report depending upon Key classification.
 *
 * @param   keys - Key used to generate report.
 *
 *
 * @return  none
 */
static void HidKeyBoard_HandleKeys(uint8_t keys)
{
    // Save last key Press except the modifier keys(shift, Ctrl, Alt and GUI keys) or Function Key & Release Key
    if((keys != leftShift) && (keys != rightShift) && (keys != leftCtrl) && (keys != rightCtrl) && (keys != leftAlt) && (keys != rightAlt) && (keys != fnKey) && (keys != releaseKey))
    {
        gKeyInfo.lastKey = keys;
    }

    if (keys == releaseKey)  // Reserved Key
    {
        HidKeyBoard_SendReport( HID_KEYBOARD_RESERVED);
        return;
    }

    if (keys == leftShift)  // Left Shift
    {
        gKeyInfo.modifierKeys |= HIDKBD_LEFTSHIFT_BM_EN;
    }

    if (keys == rightShift)  // Right Shift
    {
        gKeyInfo.modifierKeys |= HIDKBD_RIGHTSHIFT_BM_EN;
    }

    if (keys == leftCtrl)  // Left Ctrl
    {
        gKeyInfo.modifierKeys |= HIDKBD_LEFTCTRL_BM_EN;
    }

    if (keys == fnKey)  // Function Key
    {
        gKeyInfo.funcKey = TRUE;
    }

    if (keys == leftAlt)  // Left Alt
    {
        gKeyInfo.modifierKeys |= HIDKBD_LEFTALT_BM_EN;
    }

    if (keys == rightAlt)  // Right Alt
    {
        gKeyInfo.modifierKeys |= HIDKBD_RIGHTALT_BM_EN;
    }

    if (keys == rightCtrl)  // Right Ctrl
    {
        gKeyInfo.modifierKeys |= HIDKBD_RIGHTCTRL_BM_EN;
    }

    if (keys == capsLock)  // Caps Lock Key
    {
        if(gKeyInfo.capsLockState == TRUE)
        {
           gKeyInfo.capsLockState = FALSE;
           Board_setCapsLockLed(gKeyInfo.capsLockState);
        }
        else
        {
           gKeyInfo.capsLockState = TRUE;
           Board_setCapsLockLed(gKeyInfo.capsLockState);
        }
    }

    //Arrow Keys
    if( (keys == leftArrow) || (keys == upArrow) || (keys == rightArrow) || (keys == downArrow) )
    {
        if(gKeyInfo.funcKey == FALSE)
        {
            HidKeyBoard_SendReport( keyMap[keys]);
            return;
        }
        else
        {
            if(HidKeyboard_CheckKeyReleased(fnKey))
            {
                gKeyInfo.funcKey = FALSE;
                HidKeyBoard_SendReport( keyMap[keys]);
                return;
            }

            HidKeyBoard_ProcessFuncKey(keys);
            return;
        }
    }


    if((gPasscodeEntryStart == TRUE) && (gPasscodeEntryStop == FALSE))
    {
        if((keys > KEY1) && (keys < KEY12))
        {
            uint32 passcodeDigit = 0;
            switch(keys)
            {
                case KEY2:
                    passcodeDigit = 1;
                break;

                case KEY3:
                    passcodeDigit = 2;
                break;

                case KEY4:
                    passcodeDigit = 3;
                break;

                case KEY5:
                    passcodeDigit = 4;
                break;

                case KEY6:
                    passcodeDigit = 5;
                break;

                case KEY7:
                    passcodeDigit = 6;
                break;

                case KEY8:
                    passcodeDigit = 7;
                break;

                case KEY9:
                    passcodeDigit = 8;
                break;

                case KEY10:
                    passcodeDigit = 9;
                break;

                case KEY11:
                    passcodeDigit = 0;
                break;

                default:
                    passcodeDigit = 0;
                break;
            }

            gPasscode = gPasscode*10 + passcodeDigit;
            passcodeDigit = 0;
            gPasscodeDigitCount++;

            if(gPasscodeDigitCount == 6)
            {
                gPasscodeDigitCount = 0;
                gPasscodeEntryStop = TRUE;
            }

            if(gPasscodeEntryStop == TRUE)
            {
                gPasscodeEntryStop = FALSE;
                gPasscodeEntryStart = FALSE;
                // Send passcode response
                GAPBondMgr_PasscodeRsp(HidKbdConnHdl, SUCCESS, gPasscode);
            }
        }

      return;

    }

    //Normal Key Range
    {
      HidKeyBoard_SendReport(keyMap[keys]);
      return;
    }
}

/*********************************************************************
 * @fn      HidKeyBoard_ProcessFuncKey
 *
 * @brief   Prepares HID report with Function key active.
 *
 * @param   keycode - Key used to generate report.
 *
 *
 * @return  none
 */
static void HidKeyBoard_ProcessFuncKey(uint8_t keycode)
{
    if(keycode == rightArrow)
            HidKeyBoard_SendReport(HID_KEYBOARD_END);
    else if(keycode == leftArrow)
            HidKeyBoard_SendReport(HID_KEYBOARD_HOME);
    else if(keycode == upArrow)
            HidKeyBoard_SendReport(HID_KEYBOARD_PAGE_UP);
    else if(keycode == downArrow)
            HidKeyBoard_SendReport(HID_KEYBOARD_PAGE_DOWN);
}

/*********************************************************************
 * @fn      HidKeyBoard_CheckModifierByteStatus
 *
 * @brief   Check Modifier Byte Status.
 *          Update Modifier Byte according to the key press status of modifier keys
 *
 * @param   none
 *
 *
 * @return  none
 */
static void HidKeyBoard_CheckModifierByteStatus(void)
{
    if(gKeyInfo.modifierKeys & HIDKBD_LEFTSHIFT_BM_EN)
    {
        if(HidKeyboard_CheckKeyReleased(leftShift))
        {
            gKeyInfo.modifierKeys &= (~HIDKBD_LEFTSHIFT_BM_EN);
        }
    }

    if(gKeyInfo.modifierKeys & HIDKBD_RIGHTSHIFT_BM_EN)
    {
        if(HidKeyboard_CheckKeyReleased(rightShift))
        {
            gKeyInfo.modifierKeys &= (~HIDKBD_RIGHTSHIFT_BM_EN);
        }
    }
    if(gKeyInfo.modifierKeys & HIDKBD_LEFTCTRL_BM_EN)
    {
        if(HidKeyboard_CheckKeyReleased(leftCtrl))
        {
            gKeyInfo.modifierKeys &= (~HIDKBD_LEFTCTRL_BM_EN);
        }
    }
    if(gKeyInfo.modifierKeys & HIDKBD_RIGHTCTRL_BM_EN)
    {
        if(HidKeyboard_CheckKeyReleased(rightCtrl))
        {
            gKeyInfo.modifierKeys &= (~HIDKBD_RIGHTCTRL_BM_EN);
        }
    }

    if(gKeyInfo.modifierKeys & HIDKBD_LEFTALT_BM_EN)
    {
        if(HidKeyboard_CheckKeyReleased(leftAlt))
        {
            gKeyInfo.modifierKeys &= (~HIDKBD_LEFTALT_BM_EN);
        }
    }

    if(gKeyInfo.modifierKeys & HIDKBD_RIGHTALT_BM_EN)
    {
        if(HidKeyboard_CheckKeyReleased(rightAlt))
        {
            gKeyInfo.modifierKeys &= (~HIDKBD_RIGHTALT_BM_EN);
        }
    }
}

/*********************************************************************
 * @fn      HidKeyBoard_SendReport
 *
 * @brief   Build and send a HID keyboard report.
 *
 * @param   keycode - HID keycode.
 *
 * @return  none
 */
static void HidKeyBoard_SendReport( uint8 keycode )
{
    uint8_t buf[HID_KEYBOARD_IN_RPT_LEN];

    HidKeyBoard_CheckModifierByteStatus();

    buf[0] = gKeyInfo.modifierKeys;   // Modifier keys
    buf[1] = 0;                       // Reserved
    buf[2] = keycode;                 // Keycode 1
    buf[3] = 0;                       // Keycode 2
    buf[4] = 0;                       // Keycode 3
    buf[5] = 0;                       // Keycode 4
    buf[6] = 0;                       // Keycode 5
    buf[7] = 0;                       // Keycode 6

    HidDev_Report(HID_RPT_ID_KEY_IN, HID_REPORT_TYPE_INPUT,
                  HID_KEYBOARD_IN_RPT_LEN, buf );
}

/*********************************************************************
 * @fn      HidKeyBoard_KeyDetected
 *
 * @brief   Store the Column pin which generated Interrupt.
 *          Start the Debounce clock and check for key input after clock timeout.
 *
 * @param   iPinIndex - Column Pin which generated ISR
 *
 * @return  none
 */
void HidKeyBoard_KeyDetected(uint8_t iPinIndex)
{
    gKeyInfo.isrPinIndex = iPinIndex;
    Util_startClock(&keyDetect);
}

/*********************************************************************
 * @fn      HidKeyBoard_KeyDetectHandler
 *
 * @brief   Key Debounce timeout expired.
 *          Invoke application to check for Key Input.
 *
 * @param   a0 - EVENT code (HIDKBD_GPIO_KEY_SCAN_EVT) generated after clock expired
 *
 * @return  none
 */
static void HidKeyBoard_KeyDetectHandler(UArg a0)
{
    // Wake up the application thread when it waits for clock event
    Event_post(syncEvent, a0);
}

/*********************************************************************
 * @fn      HidKeyBoard_KeyReleaseHandler
 *
 * @brief   Key Release Timeout expired.
 *          Invoke applciation to check for key release and new keys.
 *
 * @param   a0 - EVENT code (HIDKBD_GPIO_KEY_SCAN_EVT) generated after clock expired
 *
 * @return  none
 */
static void HidKeyBoard_KeyReleaseHandler(UArg a0)
{
    // Wake up the application thread when it waits for clock event
    Event_post(syncEvent, a0);
}

/*********************************************************************
 * @fn      HidKeyBoardRcvReport
 *
 * @brief   Process an incoming HID keyboard report.
 *
 * @param   len - Length of report.
 * @param   pData - Report data.
 *
 * @return  status
 */
static uint8 HidKeyboard_ReceiveReport( uint8 len, uint8 *pData )
{
    // verify data length
    if ( len == HID_LED_OUT_RPT_LEN )
    {
        // set keyfob LEDs
        if((*pData & LED_CAPS_LOCK) == LED_CAPS_LOCK)
        {
            gKeyInfo.capsLockState = TRUE;
            Board_setCapsLockLed(gKeyInfo.capsLockState);
        }
        else
        {
            gKeyInfo.capsLockState = FALSE;
            Board_setCapsLockLed(gKeyInfo.capsLockState);
        }

        if((*pData & LED_NUM_LOCK) == LED_NUM_LOCK)
        {
            gKeyInfo.numLockState = TRUE;
        }
        else
        {
            gKeyInfo.numLockState = FALSE;
        }

        return SUCCESS;
    }
    else
    {
        return ATT_ERR_INVALID_VALUE_SIZE;
    }
}


/*********************************************************************
 * @fn      HidKeyBoard_ProcessPasscode
 *
 * @brief   Passcode callback.
 *
 * @param   connHandle - connection handle
 * @param   uiInputs   - input passcode
 * @param   uiOutputs  - display passcode
 *
 * @return  none
 */
static void HidKeyboard_ProcessPasscode(uint16_t connHandle,
                                        uint8_t uiInputs,
                                        uint8_t uiOutputs)
{

    // Is the callback to get the passcode from or display it to the user?
    if (uiInputs != 0)
    {
        // Passcode must be entered by the user but use the default passcode for now
        gPasscodeEntryStart = TRUE;
        gPasscode = 0;
    }

}

/*********************************************************************
 * @fn      HidKeyboard_PassCodeCB
 *
 * @brief   HID Dev Passcode callback.
 *
 * @param   evt - event ID.
 *
 * @return  HID response code.
 */
static void HidKeyboard_PassCodeCB(uint8_t *deviceAddr,
                                   uint16_t connHandle,
                                   uint8_t uiInputs,
                                   uint8_t uiOutputs )
{
    HidKbdConnHdl = connHandle;
    gInputPasscode = uiInputs;
    gDisplayPasscode = uiOutputs;

    Event_post(syncEvent, HIDKBD_PASSCODE_STATE_EVT);

    return;
}



/*********************************************************************
*********************************************************************/
