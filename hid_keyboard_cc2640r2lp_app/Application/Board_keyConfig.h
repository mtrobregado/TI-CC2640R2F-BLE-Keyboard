/*******************************************************************************
  Filename:       board_key.h
  Revised:        $Date: 2014-02-28 14:18:14 -0800 (Fri, 28 Feb 2014) $
  Revision:       $Revision: 37461 $

  Description:    This file contains the Key Service definitions
                  and prototypes.

  Copyright 2014 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, 
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE, 
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com. 
*******************************************************************************/

#ifndef BOARD_KEY_H
#define BOARD_KEY_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
 * INCLUDES
 */
#include "Board.h"

/*********************************************************************
*  EXTERNAL VARIABLES
*/

/*********************************************************************
 * CONSTANTS
 */
#define MAX_KEY_LIST            10
#define MAX_NUM_OUTPUTS         8

/*********************************************************************
 * TYPEDEFS
 */
//Input Pin Index Enum
typedef enum KeyBoard_inputLines {
    BOARD_INPUT0 = 0,
    BOARD_INPUT1,
    BOARD_INPUT2,
    BOARD_INPUT3,
    BOARD_INPUT4,
    BOARD_INPUT5,
    BOARD_INPUT6,
    BOARD_INPUT7,
    BOARD_INPUT8,
    BOARD_INPUT9,
    BOARD_INPUT10,
    BOARD_INPUT11,
    BOARD_INPUT12,
    BOARD_INPUT13,
    BOARD_INPUT14,
    BOARD_INPUT15,
    BOARD_INPUTCOUNT
}KeyBoard_inputLines;

//Output Pin Index Enum
typedef enum KeyBoard_outputLines {
    BOARD_OUTPUT0 = 0,
    BOARD_OUTPUT1,
    BOARD_OUTPUT2,
    BOARD_OUTPUT3,
    BOARD_OUTPUT4,
    BOARD_OUTPUT5,
    BOARD_OUTPUT6,
    BOARD_OUTPUT7,
    BOARD_OUTPUTCOUNT
}KeyBoard_outputLines;

// Key Value Enum
typedef enum KeyBoard_keys {
    KEY_RESERVED = 0,
    KEY1,
    KEY2,
    KEY3,
    KEY4,
    KEY5,
    KEY6,
    KEY7,
    KEY8,
    KEY9,
    KEY10,
    KEY11,
    KEY12,
    KEY13,
    KEY14,
    KEY15,
    KEY16,
    KEY17,
    KEY18,
    KEY19,
    KEY20,
    KEY21,
    KEY22,
    KEY23,
    KEY24,
    KEY25,
    KEY26,
    KEY27,
    KEY28,
    KEY29,
    KEY30,
    KEY31,
    KEY32,
    KEY33,
    KEY34,
    KEY35,
    KEY36,
    KEY37,
    KEY38,
    KEY39,
    KEY40,
    KEY41,
    KEY42,
    KEY43,
    KEY44,
    KEY45,
    KEY46,
    KEY47,
    KEY48,
    KEY49,
    KEY50,
    KEY51,
    KEY52,
    KEY53,
    KEY54,
    KEY55,
    KEY56,
    KEY57,
    KEY58,
    KEY59,
    KEY60,
    KEY61,
    KEY62,
    KEY63,
    KEY64,
    KEY65,
    KEY66,
    KEY67,
    KEY68,
    KEY69,
    KEY70,
    KEY71,
    KEY72,
    KEY73,
    KEY74,
    KEY75,
    KEY76,
    KEY77,
    KEY78,
    KEY79,
    KEY80,
    KEY81,
    KEY82,
    KEY83,
    KEY84,
    KEY85,
    KEY86,
    KEY87,
    KEY88,
    KEY89,
    KEY90,
    KEY91,
    KEY92,
    KEY93,
    KEY94,
    KEY95,
    KEY96,
    KEY97,
    KEY98,
    KEY99,
    KEY100,
    KEY101,
    KEY102,
    KEY103,
    KEY104,
    KEY105,
    KEY106,
    KEY107,
    KEY108,
    KEY109,
    KEY110,
    KEY111,
    KEY112,
    KEY113,
    KEY114,
    KEY115,
    KEY116,
    KEY117,
    KEY118,
    KEY119,
    KEY120,
    KEY121,
    KEY122,
    KEY123,
    KEY124,
    KEY125,
    KEY126,
    KEY127,
    KEY128,
    KEY129,
    KEY130,
    KEY131,
    KEY132,
    KEY133,
    KEY134,
    KEY135,
    KEY136,
    KEY137,
    KEY138,
    KEY139,
    KEY140,
    KEY141,
    KEY142,
    KEY143,
    KEY144,
    KEY145,
    KEY146,
    KEY147,
    KEY148,
    KEY149,
    KEY150,
    KEY151,
    KEY152,
    KEY153,
    KEY154,
    KEY155,
    KEY156,
    KEY157,
    KEY158,
    KEY159,
    KEY160,
    KEY161,
    KEY162,
    KEY163,
    KEYCOUNT
}KeyBoard_keys;

// fn Callback to Application 
typedef void (*keyDetectNotifyCallback)(uint8_t iPinIndex);

// Structure containing output pins and keys 
// connected to each Input pin Index
typedef struct 
{
	uint8_t  numOutPins;
	uint32_t outPins[BOARD_OUTPUTCOUNT];
	uint8_t  keys[BOARD_OUTPUTCOUNT];

}outPinMap_t;

// Structure containing 
// input Pin Index, Input Pin(GPIO),
// Output pin Index, Output pins(GPIO) 
// connected to each key
typedef struct 
{
    uint8_t  inPinCnt;
    uint8_t  outPinCnt;
	uint32_t inPin;
	uint32_t outPin;
}keyPinMap_t;


/*
 *
 * Structure holding all key press Status Information.
 *
 */

typedef struct 
{
	// Stores column pin which generated ISR on Key Press
	uint8_t isrPinIndex;

	// Array Containing List of all Keys Currently Pressed. 
	// Invalid entries should contain value of 255(0xFF).
	uint8_t keysDetected[MAX_KEY_LIST];
	
	// Array representing the report status of Keys 
	// TRUE -  Report of Key Send to Host.
	// FALSE - Not yet send to host.
	// Valid for only valid entries in keysDetected array.
	bool    keysSendtoHost[MAX_KEY_LIST]; 
    
 	// Array representing the key detection status of Keys 
	// TRUE - Positive Detection of keys.
	// FALSE - Yet to Confirm the key detection.
	// Valid for only valid entries in keysDetected array.   
    bool    keyDetectDebounce[MAX_KEY_LIST];
    
    
    bool    keyReleaseDebounce[MAX_KEY_LIST];

	// Flag to check if new Key was detected on same Input line.
	bool    newKeyDetected;
	
	// Last key sent to host.
	// Contains all Alphanumetric and special character keys
	// Modifier and Function keys are not considered as last key.
	// Only on release of this key, HID_RESERVED_KEY will be send to host. 
	uint8_t lastKey;
	
	// Variable representing status of modifier key positions.
	// Status of this byte is used while preparing modifier byte status when sending report.
	uint8_t modifierKeys;

	// Function Key position Indicator
	// TRUE - Function Key is activated
	// FALSE - Function key not activated
	bool funcKey;

	// Consumer key Play/Pause Status bit.
	bool play;

	// Caps Lock Status bit
	bool capsLockState;

	// Num Lock Status bit
	bool numLockState;
	
} keyInfo_t;



/**
 * Callback structure - must be set-up by the application.
 */
typedef struct
{
  keyDetectNotifyCallback    pfnKeyChange; 
  keyDetectNotifyCallback    pfnNextKeyChange;
} keyDetectCBs_t;
  

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * API FUNCTIONS
 */

/*********************************************************************
 * @fn      Board_initKeys
 *
 * @brief   Enable interrupts for keys on GPIOs.
 *
 * @param   none
 *
 * @return  none
 */
void Board_initKeys(void);


/*********************************************************************
 * @fn      Board_scanKeys
 *
 * @brief   Check which Combination of Input Pin and Output Pin caused Interrupt on GPIOs.
 *
 * @param   iPinIndex - Input Pin Number
 *
 * @param   pKeyDetectList - Pointer to Key Press List
 *
 * @param   pNewKey - Pointer to New key detected Flag
 *
 * @return  none
 */
void Board_scanKeys( uint8_t* pKeyDetectList, bool* pNewKey );




/*********************************************************************
 * @fn      Board_enablePinInt
 *
 * @brief   Enable Interrupt on Negative Edge
 *
 * @param   inputPinIndex - Input Pin Index
 *
 * @return  none
 */

void Board_enablePinInt(uint8_t inputPinIndex);

/*********************************************************************
 * @fn      Board_disablePinInt
 *
 * @brief   Disable Interrupt
 *
 * @param   inputPinIndex - Input Pin Index
 *
 * @return  none
 */

void Board_disablePinInt(uint8_t inputPinIndex);

/*********************************************************************
 * @fn      Board_setCapsLockLed
 *
 * @brief   Control CAPS lock LED
 *
 * @param   state - Enable/Disable
 *
 * @return  none
 */

void Board_setCapsLockLed(bool state);

/*********************************************************************
 * @fn      Board_setBLEAdvLed
 *
 * @brief   Blink LED when Advertising Turned on.
 *
 * @param   state - Enable/Disable
 *
 * @return  none
 */

void Board_setBLEAdvLed(bool state);

/******************************************************************************
 *
 * @fn      Board_isGhostKey
 *
 * @brief   Checks for ghost keys -
 *  		checks if current column+row press is valid or it can be caused by potential
 *  		ghost keys
 *
 * @param   key being checked
 *
 * @param   pKeyDetectList - Pointer to key Press List.
 * 
 *
 * @return  TRUE Key can be caused by a ghost key, FALSE key is valid
 *****************************************************************************/
bool Board_isGhostKey(uint8_t keyIndex,  uint8_t* pKeyDetectList);


/*********************************************************************
 * @fn      Board_RegisterKeyCallBack
 *
 * @brief   Register Application Callback to Notify application of Keypresses.
 *
 * @param   pAppCallBack - Application Callback Handler
 *
 * @return  none
 */
void Board_RegisterKeyCallBack( keyDetectCBs_t *pAppCallBack);

/*********************************************************************
*********************************************************************/  

void Board_SystemReset(void);
#ifdef __cplusplus
}
#endif

#endif /* BOARD_KEY_H */
