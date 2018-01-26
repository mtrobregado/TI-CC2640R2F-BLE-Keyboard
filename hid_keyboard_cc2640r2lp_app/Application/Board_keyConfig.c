/*******************************************************************************
  Filename:       board_key.c
  Revised:        $Date: 2014-03-10 07:29:12 -0700 (Mon, 10 Mar 2014) $
  Revision:       $Revision: 37597 $

  Description:    This file contains the interface to the Key Service.

  Copyright 2014 Texas Instruments Incorporated. All rights reserved.

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
  PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, 
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

  Modification Detail: Ported to CC2640R2F Platform

  Maker: Markel T. Robregado
  Date: January 4, 2018
*******************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include <stdbool.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>
   
#include "hal_mcu.h"
#include "hal_defs.h"
#include "hal_types.h"
#include "hal_assert.h"
#include <inc/hw_memmap.h>

#include <ICall.h>
#include <inc/hw_ints.h>
#include "bcomdef.h"

#include "util.h"
#include "board.h"
#include "Board_keyConfig.h"

/*********************************************************************
 * TYPEDEFS
 */

#define MAX_NUM_PINS 32
/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void Board_keyCallback(PIN_Handle hPin, PIN_Id pinId);

/*******************************************************************************
 * EXTERNAL VARIABLES
 */



/*********************************************************************
 * LOCAL VARIABLES
 */// Mapping of Input Pins(Coloumn GPIO) to Input Pin  Index.

static const uint32_t gInputPinMap[BOARD_INPUTCOUNT] = 
{
    BOARD_KEY_INPUT0,
    BOARD_KEY_INPUT1,
    BOARD_KEY_INPUT2,
    BOARD_KEY_INPUT3,
    BOARD_KEY_INPUT4,
    BOARD_KEY_INPUT5,
    BOARD_KEY_INPUT6,
    BOARD_KEY_INPUT7,
    BOARD_KEY_INPUT8,
    BOARD_KEY_INPUT9,
    BOARD_KEY_INPUT10,
    BOARD_KEY_INPUT11,
    BOARD_KEY_INPUT12,
    BOARD_KEY_INPUT13,
    BOARD_KEY_INPUT14,
    BOARD_KEY_INPUT15
};

// Mapping of Output Pins(ROw GPIO) to Output Pin  Index.

static const uint32_t gOutputPinMap[BOARD_OUTPUTCOUNT] = 
{
    BOARD_KEY_OUTPUT0,
    BOARD_KEY_OUTPUT1,
    BOARD_KEY_OUTPUT2,
    BOARD_KEY_OUTPUT3,
    BOARD_KEY_OUTPUT4,
    BOARD_KEY_OUTPUT5,
    BOARD_KEY_OUTPUT6,
    BOARD_KEY_OUTPUT7
};

// Mapping of Pin Index to Input Pin(Coloumn GPIO)
// Reverse mapping of gInputPinMap
// Invalid Input Pin Index(for pins used as output) are represented as BOARD_INPUTCOUNT.

static const uint8_t inPinId2inPinIndex[MAX_NUM_PINS] =  
{
    BOARD_INPUTCOUNT,
    BOARD_INPUT1,
    BOARD_INPUT4,
    BOARD_INPUT6,
    BOARD_INPUTCOUNT,
    BOARD_INPUT0,
    BOARD_INPUTCOUNT,
    BOARD_INPUTCOUNT,
    BOARD_INPUTCOUNT,
    BOARD_INPUT13,
    BOARD_INPUTCOUNT,
    BOARD_INPUT10,
    BOARD_INPUTCOUNT,
    BOARD_INPUT14,
    BOARD_INPUT15,
    BOARD_INPUTCOUNT,
    BOARD_INPUTCOUNT,
    BOARD_INPUTCOUNT,
    BOARD_INPUT12,
    BOARD_INPUT11,
    BOARD_INPUT9,
    BOARD_INPUTCOUNT,
    BOARD_INPUT2,
    BOARD_INPUT8,
    BOARD_INPUTCOUNT,
    BOARD_INPUT7,
    BOARD_INPUT5,
    BOARD_INPUT3,
    BOARD_INPUTCOUNT,
    BOARD_INPUTCOUNT,
    BOARD_INPUTCOUNT
};
// Mapping of Output Pins(Row GPIOs) and Keys for each Input Pin Index.
// Output Pin  & Key Map for each Input Pin Index
static const outPinMap_t gOutpinMap[BOARD_INPUTCOUNT] = 
{
    {//Input 0
         8, { BOARD_KEY_OUTPUT0, BOARD_KEY_OUTPUT1, BOARD_KEY_OUTPUT2, BOARD_KEY_OUTPUT3, BOARD_KEY_OUTPUT4, BOARD_KEY_OUTPUT5, BOARD_KEY_OUTPUT6, BOARD_KEY_OUTPUT7}, { KEY_RESERVED, KEY3, KEY1, KEY50, KEY_RESERVED, KEY28, KEY2, KEY_RESERVED}
    },
    {//Input 1
         8, { BOARD_KEY_OUTPUT0, BOARD_KEY_OUTPUT1, BOARD_KEY_OUTPUT2, BOARD_KEY_OUTPUT3, BOARD_KEY_OUTPUT4, BOARD_KEY_OUTPUT5, BOARD_KEY_OUTPUT6, BOARD_KEY_OUTPUT7}, { KEY_RESERVED, KEY64, KEY_RESERVED, KEY_RESERVED, KEY58, KEY_RESERVED, KEY_RESERVED, KEY_RESERVED}
    },
    {//Input 2
         8, { BOARD_KEY_OUTPUT0, BOARD_KEY_OUTPUT1, BOARD_KEY_OUTPUT2, BOARD_KEY_OUTPUT3, BOARD_KEY_OUTPUT4, BOARD_KEY_OUTPUT5, BOARD_KEY_OUTPUT6, BOARD_KEY_OUTPUT7}, { KEY_RESERVED, KEY_RESERVED, KEY_RESERVED, KEY_RESERVED, KEY_RESERVED, KEY_RESERVED, KEY_RESERVED, KEY_RESERVED}
    },
    {//Input 3
         8, { BOARD_KEY_OUTPUT0, BOARD_KEY_OUTPUT1, BOARD_KEY_OUTPUT2, BOARD_KEY_OUTPUT3, BOARD_KEY_OUTPUT4, BOARD_KEY_OUTPUT5, BOARD_KEY_OUTPUT6, BOARD_KEY_OUTPUT7}, { KEY_RESERVED, KEY_RESERVED, KEY18, KEY_RESERVED, KEY_RESERVED, KEY110, KEY_RESERVED, KEY_RESERVED}
    },
    {//Input 4
         8, { BOARD_KEY_OUTPUT0, BOARD_KEY_OUTPUT1, BOARD_KEY_OUTPUT2, BOARD_KEY_OUTPUT3, BOARD_KEY_OUTPUT4, BOARD_KEY_OUTPUT5, BOARD_KEY_OUTPUT6, BOARD_KEY_OUTPUT7}, { KEY_RESERVED, KEY32, KEY_RESERVED, KEY_RESERVED, KEY84, KEY_RESERVED, KEY_RESERVED, KEY_RESERVED}
    },
    {//Input 5
         8, { BOARD_KEY_OUTPUT0, BOARD_KEY_OUTPUT1, BOARD_KEY_OUTPUT2, BOARD_KEY_OUTPUT3, BOARD_KEY_OUTPUT4, BOARD_KEY_OUTPUT5, BOARD_KEY_OUTPUT6, BOARD_KEY_OUTPUT7}, { KEY_RESERVED, KEY_RESERVED, KEY83, KEY23, KEY_RESERVED, KEY19, KEY24, KEY_RESERVED}
    },
    {//Input 6
         8, { BOARD_KEY_OUTPUT0, BOARD_KEY_OUTPUT1, BOARD_KEY_OUTPUT2, BOARD_KEY_OUTPUT3, BOARD_KEY_OUTPUT4, BOARD_KEY_OUTPUT5, BOARD_KEY_OUTPUT6, BOARD_KEY_OUTPUT7}, { KEY_RESERVED, KEY_RESERVED, KEY_RESERVED, KEY_RESERVED, KEY31, KEY79, KEY_RESERVED, KEY_RESERVED}
    },
    {//Input 7
         8, { BOARD_KEY_OUTPUT0, BOARD_KEY_OUTPUT1, BOARD_KEY_OUTPUT2, BOARD_KEY_OUTPUT3, BOARD_KEY_OUTPUT4, BOARD_KEY_OUTPUT5, BOARD_KEY_OUTPUT6, BOARD_KEY_OUTPUT7}, { KEY_RESERVED, KEY54, KEY38, KEY27, KEY39, KEY37, KEY_RESERVED, KEY_RESERVED}
    },
    {//Input 8
         8, { BOARD_KEY_OUTPUT0, BOARD_KEY_OUTPUT1, BOARD_KEY_OUTPUT2, BOARD_KEY_OUTPUT3, BOARD_KEY_OUTPUT4, BOARD_KEY_OUTPUT5, BOARD_KEY_OUTPUT6, BOARD_KEY_OUTPUT7}, { KEY46, KEY_RESERVED, KEY40, KEY_RESERVED, KEY12, KEY51, KEY34, KEY11}
    },
    {//Input 9
         8, { BOARD_KEY_OUTPUT0, BOARD_KEY_OUTPUT1, BOARD_KEY_OUTPUT2, BOARD_KEY_OUTPUT3, BOARD_KEY_OUTPUT4, BOARD_KEY_OUTPUT5, BOARD_KEY_OUTPUT6, BOARD_KEY_OUTPUT7}, { KEY_RESERVED, KEY4, KEY5, KEY35, KEY17, KEY36, KEY16, KEY_RESERVED}
    },
    {//Input 10
         8, { BOARD_KEY_OUTPUT0, BOARD_KEY_OUTPUT1, BOARD_KEY_OUTPUT2, BOARD_KEY_OUTPUT3, BOARD_KEY_OUTPUT4, BOARD_KEY_OUTPUT5, BOARD_KEY_OUTPUT6, BOARD_KEY_OUTPUT7}, { KEY_RESERVED, KEY_RESERVED, KEY_RESERVED, KEY30, KEY_RESERVED, KEY43, KEY_RESERVED, KEY_RESERVED}
    },
    {//Input 11
         8, { BOARD_KEY_OUTPUT0, BOARD_KEY_OUTPUT1, BOARD_KEY_OUTPUT2, BOARD_KEY_OUTPUT3, BOARD_KEY_OUTPUT4, BOARD_KEY_OUTPUT5, BOARD_KEY_OUTPUT6, BOARD_KEY_OUTPUT7}, { KEY49, KEY8, KEY53, KEY_RESERVED, KEY13, KEY52, KEY9, KEY10}
    },
    {//Input 12
         8, { BOARD_KEY_OUTPUT0, BOARD_KEY_OUTPUT1, BOARD_KEY_OUTPUT2, BOARD_KEY_OUTPUT3, BOARD_KEY_OUTPUT4, BOARD_KEY_OUTPUT5, BOARD_KEY_OUTPUT6, BOARD_KEY_OUTPUT7}, { KEY42, KEY_RESERVED, KEY6, KEY7, KEY20, KEY21, KEY22, KEY_RESERVED}
    },
    {//Input 13
         8, { BOARD_KEY_OUTPUT0, BOARD_KEY_OUTPUT1, BOARD_KEY_OUTPUT2, BOARD_KEY_OUTPUT3, BOARD_KEY_OUTPUT4, BOARD_KEY_OUTPUT5, BOARD_KEY_OUTPUT6, BOARD_KEY_OUTPUT7}, { KEY_RESERVED, KEY89, KEY41, KEY33, KEY_RESERVED, KEY26, KEY25, KEY55}
    },
    {//Input 14
         8, { BOARD_KEY_OUTPUT0, BOARD_KEY_OUTPUT1, BOARD_KEY_OUTPUT2, BOARD_KEY_OUTPUT3, BOARD_KEY_OUTPUT4, BOARD_KEY_OUTPUT5, BOARD_KEY_OUTPUT6, BOARD_KEY_OUTPUT7}, { KEY61, KEY_RESERVED, KEY_RESERVED, KEY_RESERVED, KEY29, KEY76, KEY47, KEY48}
    },
    {//Input 15
         8, { BOARD_KEY_OUTPUT0, BOARD_KEY_OUTPUT1, BOARD_KEY_OUTPUT2, BOARD_KEY_OUTPUT3, BOARD_KEY_OUTPUT4, BOARD_KEY_OUTPUT5, BOARD_KEY_OUTPUT6, BOARD_KEY_OUTPUT7}, { KEY_RESERVED, KEY_RESERVED, KEY44, KEY_RESERVED, KEY_RESERVED, KEY57, KEY_RESERVED, KEY_RESERVED}
    }
};

// Mapping of 
// 	a. Input Pin Index,
//  b. Output Pin Index,
//  c. Input Pin(Coloumn GPIO), 
//  d. Output Pin (Row GPIO)
// for each key Input.
static const keyPinMap_t gKeytoPinMap[KEYCOUNT] = 
{
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY0
    {BOARD_INPUT0,  BOARD_OUTPUT2, BOARD_KEY_INPUT0,  BOARD_KEY_OUTPUT2},       //KEY1
    {BOARD_INPUT0,  BOARD_OUTPUT6, BOARD_KEY_INPUT0,  BOARD_KEY_OUTPUT6},       //KEY2
    {BOARD_INPUT0,  BOARD_OUTPUT1, BOARD_KEY_INPUT0,  BOARD_KEY_OUTPUT1},       //KEY3
    {BOARD_INPUT9,  BOARD_OUTPUT1, BOARD_KEY_INPUT9,  BOARD_KEY_OUTPUT1},       //KEY4
    {BOARD_INPUT9,  BOARD_OUTPUT2, BOARD_KEY_INPUT9,  BOARD_KEY_OUTPUT2},       //KEY5
    {BOARD_INPUT12, BOARD_OUTPUT2, BOARD_KEY_INPUT12, BOARD_KEY_OUTPUT2},       //KEY6
    {BOARD_INPUT12, BOARD_OUTPUT3, BOARD_KEY_INPUT12, BOARD_KEY_OUTPUT3},       //KEY7
    {BOARD_INPUT11, BOARD_OUTPUT1, BOARD_KEY_INPUT11, BOARD_KEY_OUTPUT1},       //KEY8
    {BOARD_INPUT11, BOARD_OUTPUT6, BOARD_KEY_INPUT11, BOARD_KEY_OUTPUT6},       //KEY9
    {BOARD_INPUT11, BOARD_OUTPUT7, BOARD_KEY_INPUT11, BOARD_KEY_OUTPUT7},       //KEY10
    {BOARD_INPUT8,  BOARD_OUTPUT7, BOARD_KEY_INPUT8,  BOARD_KEY_OUTPUT7},       //KEY11
    {BOARD_INPUT8,  BOARD_OUTPUT4, BOARD_KEY_INPUT8,  BOARD_KEY_OUTPUT4},       //KEY12
    {BOARD_INPUT11, BOARD_OUTPUT4, BOARD_KEY_INPUT11, BOARD_KEY_OUTPUT4},       //KEY13
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY14
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY15
    {BOARD_INPUT9,  BOARD_OUTPUT6, BOARD_KEY_INPUT9,  BOARD_KEY_OUTPUT6},       //KEY16
    {BOARD_INPUT9,  BOARD_OUTPUT4, BOARD_KEY_INPUT9,  BOARD_KEY_OUTPUT4},       //KEY17
    {BOARD_INPUT3,  BOARD_OUTPUT2, BOARD_KEY_INPUT3,  BOARD_KEY_OUTPUT2},       //KEY18
    {BOARD_INPUT5,  BOARD_OUTPUT5, BOARD_KEY_INPUT5,  BOARD_KEY_OUTPUT5},       //KEY19
    {BOARD_INPUT12, BOARD_OUTPUT4, BOARD_KEY_INPUT12, BOARD_KEY_OUTPUT4},       //KEY20
    {BOARD_INPUT12, BOARD_OUTPUT5, BOARD_KEY_INPUT12, BOARD_KEY_OUTPUT5},       //KEY21
    {BOARD_INPUT12, BOARD_OUTPUT6, BOARD_KEY_INPUT12, BOARD_KEY_OUTPUT6},       //KEY22
    {BOARD_INPUT5,  BOARD_OUTPUT3, BOARD_KEY_INPUT5,  BOARD_KEY_OUTPUT3},       //KEY23
    {BOARD_INPUT5,  BOARD_OUTPUT6, BOARD_KEY_INPUT5,  BOARD_KEY_OUTPUT6},       //KEY24
    {BOARD_INPUT13, BOARD_OUTPUT6, BOARD_KEY_INPUT13, BOARD_KEY_OUTPUT6},       //KEY25
    {BOARD_INPUT13, BOARD_OUTPUT5, BOARD_KEY_INPUT13, BOARD_KEY_OUTPUT5},       //KEY26
    {BOARD_INPUT7,  BOARD_OUTPUT3, BOARD_KEY_INPUT7,  BOARD_KEY_OUTPUT3},       //KEY27
    {BOARD_INPUT0,  BOARD_OUTPUT5, BOARD_KEY_INPUT0,  BOARD_KEY_OUTPUT5},       //KEY28
    {BOARD_INPUT14, BOARD_OUTPUT4, BOARD_KEY_INPUT14, BOARD_KEY_OUTPUT4},       //KEY29
    {BOARD_INPUT10, BOARD_OUTPUT3, BOARD_KEY_INPUT10, BOARD_KEY_OUTPUT3},       //KEY30
    {BOARD_INPUT6,  BOARD_OUTPUT4, BOARD_KEY_INPUT6,  BOARD_KEY_OUTPUT4},       //KEY31
    {BOARD_INPUT4,  BOARD_OUTPUT1, BOARD_KEY_INPUT4,  BOARD_KEY_OUTPUT1},       //KEY32
    {BOARD_INPUT13, BOARD_OUTPUT3, BOARD_KEY_INPUT13, BOARD_KEY_OUTPUT3},       //KEY33
    {BOARD_INPUT8,  BOARD_OUTPUT6, BOARD_KEY_INPUT8,  BOARD_KEY_OUTPUT6},       //KEY34
    {BOARD_INPUT9,  BOARD_OUTPUT3, BOARD_KEY_INPUT9,  BOARD_KEY_OUTPUT3},       //KEY35
    {BOARD_INPUT9,  BOARD_OUTPUT5, BOARD_KEY_INPUT9,  BOARD_KEY_OUTPUT5},       //KEY36
    {BOARD_INPUT7,  BOARD_OUTPUT5, BOARD_KEY_INPUT7,  BOARD_KEY_OUTPUT5},       //KEY37
    {BOARD_INPUT7,  BOARD_OUTPUT2, BOARD_KEY_INPUT7,  BOARD_KEY_OUTPUT2},       //KEY38
    {BOARD_INPUT7,  BOARD_OUTPUT4, BOARD_KEY_INPUT7,  BOARD_KEY_OUTPUT4},       //KEY39
    {BOARD_INPUT8,  BOARD_OUTPUT2, BOARD_KEY_INPUT8,  BOARD_KEY_OUTPUT2},       //KEY40
    {BOARD_INPUT13, BOARD_OUTPUT2, BOARD_KEY_INPUT13, BOARD_KEY_OUTPUT2},       //KEY41
    {BOARD_INPUT12, BOARD_OUTPUT0, BOARD_KEY_INPUT12, BOARD_KEY_OUTPUT0},       //KEY42
    {BOARD_INPUT10, BOARD_OUTPUT5, BOARD_KEY_INPUT10, BOARD_KEY_OUTPUT5},       //KEY43
    {BOARD_INPUT15, BOARD_OUTPUT2, BOARD_KEY_INPUT15, BOARD_KEY_OUTPUT2},       //KEY44
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY45
    {BOARD_INPUT8,  BOARD_OUTPUT0, BOARD_KEY_INPUT8,  BOARD_KEY_OUTPUT0},       //KEY46
    {BOARD_INPUT14, BOARD_OUTPUT6, BOARD_KEY_INPUT14, BOARD_KEY_OUTPUT6},       //KEY47
    {BOARD_INPUT14, BOARD_OUTPUT7, BOARD_KEY_INPUT14, BOARD_KEY_OUTPUT7},       //KEY48
    {BOARD_INPUT11, BOARD_OUTPUT0, BOARD_KEY_INPUT11, BOARD_KEY_OUTPUT0},       //KEY49
    {BOARD_INPUT0,  BOARD_OUTPUT3, BOARD_KEY_INPUT0,  BOARD_KEY_OUTPUT3},       //KEY50
    {BOARD_INPUT8,  BOARD_OUTPUT5, BOARD_KEY_INPUT8,  BOARD_KEY_OUTPUT5},       //KEY51
    {BOARD_INPUT11, BOARD_OUTPUT5, BOARD_KEY_INPUT11, BOARD_KEY_OUTPUT5},       //KEY52
    {BOARD_INPUT11, BOARD_OUTPUT2, BOARD_KEY_INPUT11, BOARD_KEY_OUTPUT2},       //KEY53
    {BOARD_INPUT7,  BOARD_OUTPUT1, BOARD_KEY_INPUT7,  BOARD_KEY_OUTPUT1},       //KEY54
    {BOARD_INPUT13, BOARD_OUTPUT7, BOARD_KEY_INPUT13, BOARD_KEY_OUTPUT7},       //KEY55
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY56
    {BOARD_INPUT15, BOARD_OUTPUT5, BOARD_KEY_INPUT15, BOARD_KEY_OUTPUT5},       //KEY57
    {BOARD_INPUT1,  BOARD_OUTPUT4, BOARD_KEY_INPUT1,  BOARD_KEY_OUTPUT4},       //KEY58
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY59
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY60
    {BOARD_INPUT14, BOARD_OUTPUT0, BOARD_KEY_INPUT14, BOARD_KEY_OUTPUT0},       //KEY61
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY62
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY63
    {BOARD_INPUT1,  BOARD_OUTPUT1, BOARD_KEY_INPUT1,  BOARD_KEY_OUTPUT1},       //KEY64
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY65
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY66
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY67
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY68
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY69
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY70
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY71
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY72
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY73
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY74
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY75
    {BOARD_INPUT14, BOARD_OUTPUT5, BOARD_KEY_INPUT14, BOARD_KEY_OUTPUT5},       //KEY76
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY77
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY78
    {BOARD_INPUT6,  BOARD_OUTPUT5, BOARD_KEY_INPUT6,  BOARD_KEY_OUTPUT5},       //KEY79
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY80
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY81
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY82
    {BOARD_INPUT5,  BOARD_OUTPUT2, BOARD_KEY_INPUT5,  BOARD_KEY_OUTPUT2},       //KEY83
    {BOARD_INPUT4,  BOARD_OUTPUT4, BOARD_KEY_INPUT4,  BOARD_KEY_OUTPUT4},       //KEY84
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY85
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY86
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY87
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY88
    {BOARD_INPUT13, BOARD_OUTPUT1, BOARD_KEY_INPUT13, BOARD_KEY_OUTPUT1},       //KEY89
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY90
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY91
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY92
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY93
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY94
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY95
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY96
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY97
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY98
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY99
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY100
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY101
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY102
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY103
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY104
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY105
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY106
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY107
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY108
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY109
    {BOARD_INPUT3,  BOARD_OUTPUT5, BOARD_KEY_INPUT3,  BOARD_KEY_OUTPUT5},       //KEY110
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY111
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY112
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY113
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY114
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY115
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY116
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY117
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY118
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY119
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY120
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY121
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY122
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY123
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY124
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY125
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY126
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY127
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY128
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY129
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY130
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY131
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY132
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY133
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY134
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY135
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY136
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY137
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY138
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY139
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY140
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY141
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY142
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY143
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY144
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY145
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY146
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY147
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY148
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY149
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY150
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY151
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY152
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY153
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY154
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY155
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY156
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY157
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY158
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY159
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY160
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY161
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY162
    {KEY_RESERVED,  KEY_RESERVED,  KEY_RESERVED,      KEY_RESERVED},            //KEY163
};
// Pointer to application callback
const keyDetectCBs_t* pKeyDetect = NULL;
// Memory for the GPIO module to construct a Hwi
Hwi_Struct callbackHwiKeys;
// PIN configuration structure to set all KEY pins as inputs with pullups enabled
PIN_Config keyPinsCfg[] = 
{
    BOARD_KEY_INPUT0    | PIN_GPIO_OUTPUT_DIS  | PIN_INPUT_EN  | PIN_PULLUP   | PIN_HYSTERESIS,
    BOARD_KEY_INPUT1    | PIN_GPIO_OUTPUT_DIS  | PIN_INPUT_EN  | PIN_PULLUP   | PIN_HYSTERESIS,
    BOARD_KEY_INPUT2    | PIN_GPIO_OUTPUT_DIS  | PIN_INPUT_EN  | PIN_PULLUP   | PIN_HYSTERESIS,
    BOARD_KEY_INPUT3    | PIN_GPIO_OUTPUT_DIS  | PIN_INPUT_EN  | PIN_PULLUP   | PIN_HYSTERESIS,
    BOARD_KEY_INPUT4    | PIN_GPIO_OUTPUT_DIS  | PIN_INPUT_EN  | PIN_PULLUP   | PIN_HYSTERESIS,
    BOARD_KEY_INPUT5    | PIN_GPIO_OUTPUT_DIS  | PIN_INPUT_EN  | PIN_PULLUP   | PIN_HYSTERESIS,
    BOARD_KEY_INPUT6    | PIN_GPIO_OUTPUT_DIS  | PIN_INPUT_EN  | PIN_PULLUP   | PIN_HYSTERESIS,
    BOARD_KEY_INPUT7    | PIN_GPIO_OUTPUT_DIS  | PIN_INPUT_EN  | PIN_PULLUP   | PIN_HYSTERESIS,
    BOARD_KEY_INPUT8    | PIN_GPIO_OUTPUT_DIS  | PIN_INPUT_EN  | PIN_PULLUP   | PIN_HYSTERESIS,
    BOARD_KEY_INPUT9    | PIN_GPIO_OUTPUT_DIS  | PIN_INPUT_EN  | PIN_PULLUP   | PIN_HYSTERESIS,
    BOARD_KEY_INPUT10   | PIN_GPIO_OUTPUT_DIS  | PIN_INPUT_EN  | PIN_PULLUP   | PIN_HYSTERESIS,
    BOARD_KEY_INPUT11   | PIN_GPIO_OUTPUT_DIS  | PIN_INPUT_EN  | PIN_PULLUP   | PIN_HYSTERESIS,
    BOARD_KEY_INPUT12   | PIN_GPIO_OUTPUT_DIS  | PIN_INPUT_EN  | PIN_PULLUP   | PIN_HYSTERESIS,
    BOARD_KEY_INPUT13   | PIN_GPIO_OUTPUT_DIS  | PIN_INPUT_EN  | PIN_PULLUP   | PIN_HYSTERESIS,
    BOARD_KEY_INPUT14   | PIN_GPIO_OUTPUT_DIS  | PIN_INPUT_EN  | PIN_PULLUP   | PIN_HYSTERESIS,
    BOARD_KEY_INPUT15   | PIN_GPIO_OUTPUT_DIS  | PIN_INPUT_EN  | PIN_PULLUP   | PIN_HYSTERESIS,
    BOARD_KEY_OUTPUT0   | PIN_GPIO_OUTPUT_EN   | PIN_GPIO_LOW  | PIN_PUSHPULL | PIN_DRVSTR_MAX,       /* Pin initially Low */
    BOARD_KEY_OUTPUT1   | PIN_GPIO_OUTPUT_EN   | PIN_GPIO_LOW  | PIN_PUSHPULL | PIN_DRVSTR_MAX,       /* Pin initially Low */
    BOARD_KEY_OUTPUT2   | PIN_GPIO_OUTPUT_EN   | PIN_GPIO_LOW  | PIN_PUSHPULL | PIN_DRVSTR_MAX,       /* Pin initially Low */
    BOARD_KEY_OUTPUT3   | PIN_GPIO_OUTPUT_EN   | PIN_GPIO_LOW  | PIN_PUSHPULL | PIN_DRVSTR_MAX,       /* Pin initially Low */
    BOARD_KEY_OUTPUT4   | PIN_GPIO_OUTPUT_EN   | PIN_GPIO_LOW  | PIN_PUSHPULL | PIN_DRVSTR_MAX,       /* Pin initially Low */
    BOARD_KEY_OUTPUT5   | PIN_GPIO_OUTPUT_EN   | PIN_GPIO_LOW  | PIN_PUSHPULL | PIN_DRVSTR_MAX,       /* Pin initially Low */
    BOARD_KEY_OUTPUT6   | PIN_GPIO_OUTPUT_EN   | PIN_GPIO_LOW  | PIN_PUSHPULL | PIN_DRVSTR_MAX,       /* Pin initially Low */
    BOARD_KEY_OUTPUT7   | PIN_GPIO_OUTPUT_EN   | PIN_GPIO_LOW  | PIN_PUSHPULL | PIN_DRVSTR_MAX,       /* Pin initially Low */
    BOARD_LED_POWER     | PIN_GPIO_OUTPUT_EN   | PIN_GPIO_LOW  | PIN_PUSHPULL | PIN_DRVSTR_MAX,       /* LED initially Off */
    BOARD_LED_BLE       | PIN_GPIO_OUTPUT_EN   | PIN_GPIO_LOW  | PIN_PUSHPULL | PIN_DRVSTR_MAX,       /* LED initially Off */
    PIN_TERMINATE                                                                                     /* Terminate list   */
};
/*============================================================================*/

PIN_State  keyPins;
PIN_Handle hKeyPins;
/*********************************************************************
 * PUBLIC FUNCTIONS
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
void Board_initKeys(void)
{
    // Initialize KEY pins. Enable int after callback registered
    hKeyPins = PIN_open(&keyPins, keyPinsCfg);
    PIN_registerIntCb(hKeyPins, Board_keyCallback);

    PIN_setConfig(hKeyPins, PIN_BM_IRQ, BOARD_KEY_INPUT0   | PIN_IRQ_NEGEDGE);
    PIN_setConfig(hKeyPins, PIN_BM_IRQ, BOARD_KEY_INPUT1   | PIN_IRQ_NEGEDGE);
    PIN_setConfig(hKeyPins, PIN_BM_IRQ, BOARD_KEY_INPUT2   | PIN_IRQ_NEGEDGE);
    PIN_setConfig(hKeyPins, PIN_BM_IRQ, BOARD_KEY_INPUT3   | PIN_IRQ_NEGEDGE);
    PIN_setConfig(hKeyPins, PIN_BM_IRQ, BOARD_KEY_INPUT4   | PIN_IRQ_NEGEDGE);
    PIN_setConfig(hKeyPins, PIN_BM_IRQ, BOARD_KEY_INPUT5   | PIN_IRQ_NEGEDGE);
    PIN_setConfig(hKeyPins, PIN_BM_IRQ, BOARD_KEY_INPUT6   | PIN_IRQ_NEGEDGE);
    PIN_setConfig(hKeyPins, PIN_BM_IRQ, BOARD_KEY_INPUT7   | PIN_IRQ_NEGEDGE);
    PIN_setConfig(hKeyPins, PIN_BM_IRQ, BOARD_KEY_INPUT8   | PIN_IRQ_NEGEDGE);
    PIN_setConfig(hKeyPins, PIN_BM_IRQ, BOARD_KEY_INPUT9   | PIN_IRQ_NEGEDGE);
    PIN_setConfig(hKeyPins, PIN_BM_IRQ, BOARD_KEY_INPUT10  | PIN_IRQ_NEGEDGE);
    PIN_setConfig(hKeyPins, PIN_BM_IRQ, BOARD_KEY_INPUT11  | PIN_IRQ_NEGEDGE);
    PIN_setConfig(hKeyPins, PIN_BM_IRQ, BOARD_KEY_INPUT12  | PIN_IRQ_NEGEDGE);
    PIN_setConfig(hKeyPins, PIN_BM_IRQ, BOARD_KEY_INPUT13  | PIN_IRQ_NEGEDGE);
    PIN_setConfig(hKeyPins, PIN_BM_IRQ, BOARD_KEY_INPUT14  | PIN_IRQ_NEGEDGE);
    PIN_setConfig(hKeyPins, PIN_BM_IRQ, BOARD_KEY_INPUT15  | PIN_IRQ_NEGEDGE);

    PIN_setOutputValue(hKeyPins, PIN_ID(BOARD_KEY_OUTPUT0), OFF);
    PIN_setOutputValue(hKeyPins, PIN_ID(BOARD_KEY_OUTPUT1), OFF);
    PIN_setOutputValue(hKeyPins, PIN_ID(BOARD_KEY_OUTPUT2), OFF);
    PIN_setOutputValue(hKeyPins, PIN_ID(BOARD_KEY_OUTPUT3), OFF);
    PIN_setOutputValue(hKeyPins, PIN_ID(BOARD_KEY_OUTPUT4), OFF);
    PIN_setOutputValue(hKeyPins, PIN_ID(BOARD_KEY_OUTPUT5), OFF);
    PIN_setOutputValue(hKeyPins, PIN_ID(BOARD_KEY_OUTPUT6), OFF);
    PIN_setOutputValue(hKeyPins, PIN_ID(BOARD_KEY_OUTPUT7), OFF);
    PIN_setOutputValue(hKeyPins, PIN_ID(BOARD_LED_POWER), BOARD_LED_OFF);
    PIN_setOutputValue(hKeyPins, PIN_ID(BOARD_LED_BLE),   BOARD_LED_OFF);
}


/*********************************************************************
 * @fn      Board_setCapsLockLed
 *
 * @brief   Control CAPS lock LED
 *
 * @param   state - Enable/Disable
 *
 * @return  none
 */
void Board_setCapsLockLed(bool state)
{
    if(state == TRUE)
    {
        PIN_setOutputValue(hKeyPins, BOARD_LED_POWER, BOARD_LED_OFF);
    }
    else
    {
        PIN_setOutputValue(hKeyPins, BOARD_LED_POWER, BOARD_LED_ON);
    }
}


/*********************************************************************
 * @fn      Board_setBLEAdvLed
 *
 * @brief   Blink LED when Advertising Turned on.
 *
 * @param   state - Enable/Disable
 *
 * @return  none
 */
void Board_setBLEAdvLed(bool state)
{
   if(state == TRUE)
   {
       PIN_setOutputValue(hKeyPins, BOARD_LED_BLE, BOARD_LED_OFF);
   }
   else
   {
       PIN_setOutputValue(hKeyPins, BOARD_LED_BLE, BOARD_LED_ON);
   }
}


/*********************************************************************
 * @fn      Board_enablePinInt
 *
 * @brief   Enable Interrupt on Negative Edge
 *
 * @param   inputPinIndex - Input Pin Index
 *
 * @return  none
 */
void Board_enablePinInt(uint8_t inputPinIndex)
{
    /* Enable Interrupt */
    PIN_setConfig(hKeyPins, PIN_BM_IRQ, PIN_ID(inputPinIndex) | PIN_IRQ_NEGEDGE);
}

/*********************************************************************
 * @fn      Board_disablePinInt
 *
 * @brief   Disable Interrupt
 *
 * @param   inputPinIndex - Input Pin Index
 *
 * @return  none
 */
void Board_disablePinInt(uint8_t inputPinIndex)
{
    /* Disable Interrupt */
    PIN_setConfig(hKeyPins, PIN_BM_IRQ, PIN_ID(inputPinIndex)|PIN_IRQ_DIS);  
}


/*********************************************************************
 * @fn      Board_checkIfNewKey
 *
 * @brief   Check If key is already availble on Key list
 *          If not, add it to the list
 *
 * @param   iPinIndex - Input Pin Number
 * @param   pKeyDetectList - Pointer to Key Press List
 * @param   pNewKey - Pointer to newKey detection flag.
 *
 * @return  none
 */

static void Board_checkIfNewKey( uint8_t key, uint8_t* pKeyDetectList, bool* pNewKey)
{
    uint8_t i = 0;
    
    for(i = 0; i < MAX_KEY_LIST; i++)
    {
        if(*(pKeyDetectList+i) == key) //Key already used.
        {
            return;
        }
    }  
    
    for(i = 0; i < MAX_KEY_LIST; i++)
    {
        if(*(pKeyDetectList+i) == 255) //Field Not used
        {
            *(pKeyDetectList+i) = key;
			*pNewKey = TRUE;
            return;
        }
    }  
}

/*********************************************************************
 * @fn      Board_scanKeys
 *
 * @brief   Check which Combination of Input Pin and Output Pin caused Interrupt on GPIOs.
 *
 * @param   pKeyDetectList - Pointer to Key Press List
 *
 * @param   pNewKey - Pointer to New key detected Flag
 *
 * @return  none
 */
void Board_scanKeys( uint8_t* pKeyDetectList, bool* pNewKey )
{
    uint8_t i, iCnt = 0;
    uint32_t keyDetected = FALSE;
    uint32_t value = 0xFFFF;
    
    //Change All Output Pins to Input Pins
    for(iCnt = 0; iCnt < MAX_NUM_OUTPUTS; iCnt++)
    {
        PIN_setConfig(hKeyPins,
                      PIN_BM_ALL,
                      gOutputPinMap[iCnt] | PIN_GPIO_OUTPUT_DIS | PIN_INPUT_EN);
    }
    
    for(iCnt = 0; iCnt < MAX_NUM_OUTPUTS; iCnt++)
    {
        PIN_setConfig(hKeyPins,
                      PIN_BM_OUTPUT_MODE,
                      gOutputPinMap[iCnt] | PIN_GPIO_OUTPUT_EN | PIN_DRVSTR_MED | PIN_GPIO_LOW);
        
        //Read DIN register to get Input port status
        value = PIN_getPortInputValue(hKeyPins);

        for(i = 0; i < BOARD_INPUTCOUNT; i++)
        {
            uint32_t inPinBm = (0x1 << gInputPinMap[i]);
          
            // Check if Input Pin value is same as Output Pin
            if((value & inPinBm) == OFF)
            {
                Board_checkIfNewKey(gOutpinMap[i].keys[iCnt], pKeyDetectList, pNewKey);
                keyDetected = TRUE;
            } 
        }
        
        PIN_setConfig(hKeyPins,
                      PIN_BM_ALL,
                      gOutputPinMap[iCnt] | PIN_GPIO_OUTPUT_DIS | PIN_INPUT_EN);
    }
        
    if(keyDetected == FALSE)
    {
        //Revert Back All Original Output Pins 
        for(iCnt = 0; iCnt < MAX_NUM_OUTPUTS; iCnt++)
        {
            PIN_setConfig(hKeyPins,
                          PIN_BM_OUTPUT_MODE,
                          gOutputPinMap[iCnt] |PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW  | PIN_PUSHPULL | PIN_DRVSTR_MAX);

        }
        
        for(i = 0; i < BOARD_INPUTCOUNT; i++)
        {
            //Enable Interrupt
            PIN_setInterrupt(hKeyPins,
                             gInputPinMap[i] | PIN_IRQ_NEGEDGE);

        }
    }
}

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
bool Board_isGhostKey(uint8_t keyIndex,  uint8_t* pKeyDetectList)
{
    uint32_t inputPinIndex = 0xFF;
    uint32_t outputPinIndex = 0xFF;
    uint8_t  iRowIndex[MAX_KEY_LIST] = {255,255,255,255,255,255,255,255,255,255};
    uint8_t  iColIndex[MAX_KEY_LIST] = {255,255,255,255,255,255,255,255,255,255};
    uint8_t  i = 0, j = 0;

    inputPinIndex  = gKeytoPinMap[keyIndex].inPin;
    outputPinIndex = gKeytoPinMap[keyIndex].outPin;
    
    
    //Collect all Rows(Output Pins) and Columns(Input Pins) being used
    for(i = 0; i < MAX_KEY_LIST; i++)
    {
        if(( *(pKeyDetectList+i) != 255)&&( *(pKeyDetectList+i)  != keyIndex)) 
        {
            iColIndex[i]  = gKeytoPinMap[(*(pKeyDetectList+i))].inPin;
            iRowIndex[i]  = gKeytoPinMap[(*(pKeyDetectList+i))].outPin;
        }
    }  

    // Check if current key row and columns are being used any where else
    for(i = 0; i < MAX_KEY_LIST; i++)
    {
        if(outputPinIndex == iRowIndex[i])
        {
            for(j = 0; j < MAX_KEY_LIST; j++)
            {
                // Check if any key is there on the same column.
                if((iColIndex[i] == iColIndex[j]) && (i != j) )
                {
                    // Report if there's a potential ghost key (current col and row
                    // is already pressed) so that the key is ignored
                    return TRUE;
                }
            }
        }
    }

    // Now do the same for Columns
    for(i = 0; i < MAX_KEY_LIST; i++)
    {
        if(inputPinIndex == iColIndex[i])
        {
            for(j = 0; j < MAX_KEY_LIST; j++)
            {
                // Check if any key is there on the same Row.
                if((iRowIndex[i] == iRowIndex[j]) && (i != j) )
                {
                    // Report if there's a potential ghost key (current col and row
                    // is already pressed) so that the key is ignored
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

void Board_RegisterKeyCallBack( keyDetectCBs_t *pAppCallBack)
{
	pKeyDetect = pAppCallBack;
	return;
}

/*********************************************************************
 * @fn      Board_keyCallback
 *
 * @brief   Interrupt handler for Keys
 *
 * @param   none
 *
 * @return  none
 */
static void Board_keyCallback(PIN_Handle hPin, PIN_Id pinId)
{
   uint8 i, inputPinIndex;
   
   if (pKeyDetect != NULL)
   {
        for(i = 0; i < BOARD_INPUTCOUNT; i++)
        {
            inputPinIndex = gInputPinMap[i];
            
            //Disable Interrupt

            PIN_setConfig(hKeyPins,
                          PIN_BM_ALL,
                          inputPinIndex | PIN_IRQ_DIS);
            /*
            PIN_setConfig(hKeyPins,
                          PIN_BM_ALL,
                          PIN_ID(inputPinIndex) | PIN_IRQ_DIS);
            */
            //IO_CC26XX_disInterrupt(inputPinIndex);
        }
               
        // Notify the application
        pKeyDetect->pfnKeyChange(inPinId2inPinIndex[pinId]); 
   }
}


void Board_SystemReset(void)
{
    HAL_SYSTEM_RESET();
}

/*********************************************************************
*********************************************************************/
