const uint8_t keyMap[KEYCOUNT] = 
{
    HID_KEYBOARD_RESERVED,          //KEY0
    HID_KEYBOARD_GRV_ACCENT,        //KEY1
    HID_KEYBOARD_1,                 //KEY2
    HID_KEYBOARD_2,                 //KEY3
    HID_KEYBOARD_3,                 //KEY4
    HID_KEYBOARD_4,                 //KEY5
    HID_KEYBOARD_5,                 //KEY6
    HID_KEYBOARD_6,                 //KEY7
    HID_KEYBOARD_7,                 //KEY8
    HID_KEYBOARD_8,                 //KEY9
    HID_KEYBOARD_9,                 //KEY10
    HID_KEYBOARD_0,                 //KEY11
    HID_KEYBOARD_MINUS,             //KEY12
    HID_KEYBOARD_EQUAL,             //KEY13
    HID_KEYBOARD_RESERVED,          //KEY14
    HID_KEYBOARD_DELETE,            //KEY15
    HID_KEYBOARD_TAB,               //KEY16
    HID_KEYBOARD_Q,                 //KEY17
    HID_KEYBOARD_W,                 //KEY18
    HID_KEYBOARD_E,                 //KEY19
    HID_KEYBOARD_R,                 //KEY20
    HID_KEYBOARD_T,                 //KEY21
    HID_KEYBOARD_Y,                 //KEY22
    HID_KEYBOARD_U,                 //KEY23
    HID_KEYBOARD_I,                 //KEY24
    HID_KEYBOARD_O,                 //KEY25
    HID_KEYBOARD_P,                 //KEY26
    HID_KEYBOARD_LEFT_BRKT,         //KEY27
    HID_KEYBOARD_RIGHT_BRKT,        //KEY28
    HID_KEYBOARD_BACK_SLASH,        //KEY29
    HID_KEYBOARD_CAPS_LOCK,         //KEY30
    HID_KEYBOARD_A,                 //KEY31
    HID_KEYBOARD_S,                 //KEY32
    HID_KEYBOARD_D,                 //KEY33
    HID_KEYBOARD_F,                 //KEY34
    HID_KEYBOARD_G,                 //KEY35
    HID_KEYBOARD_H,                 //KEY36
    HID_KEYBOARD_J,                 //KEY37
    HID_KEYBOARD_K,                 //KEY38
    HID_KEYBOARD_L,                 //KEY39
    HID_KEYBOARD_SEMI_COLON,        //KEY40
    HID_KEYBOARD_SGL_QUOTE,         //KEY41
    HID_KEYBOARD_DELETE,            //KEY42
    HID_KEYBOARD_RETURN,            //KEY43
    HID_KEYBOARD_LEFT_SHIFT,        //KEY44
    HID_KEYBOARD_RESERVED,          //KEY45
    HID_KEYBOARD_Z,                 //KEY46
    HID_KEYBOARD_X,                 //KEY47
    HID_KEYBOARD_C,                 //KEY48
    HID_KEYBOARD_V,                 //KEY49
    HID_KEYBOARD_B,                 //KEY50
    HID_KEYBOARD_N,                 //KEY51
    HID_KEYBOARD_M,                 //KEY52
    HID_KEYBOARD_COMMA,             //KEY53
    HID_KEYBOARD_DOT,               //KEY54
    HID_KEYBOARD_FWD_SLASH,         //KEY55
    HID_KEYBOARD_RESERVED,          //KEY56
    HID_KEYBOARD_RIGHT_SHIFT,       //KEY57
    HID_KEYBOARD_LEFT_CTRL,         //KEY58
    HID_KEYBOARD_RESERVED,          //KEY59
    HID_KEYBOARD_LEFT_ALT,          //KEY60
    HID_KEYBOARD_SPACEBAR,          //KEY61
    HID_KEYBOARD_RIGHT_ALT,         //KEY62
    HID_KEYBOARD_RESERVED,          //KEY63
    HID_KEYBOARD_RIGHT_CTRL,        //KEY64
    HID_KEYBOARD_RESERVED,          //KEY65
    HID_KEYBOARD_RESERVED,          //KEY66
    HID_KEYBOARD_RESERVED,          //KEY67
    HID_KEYBOARD_RESERVED,          //KEY68
    HID_KEYBOARD_RESERVED,          //KEY69
    HID_KEYBOARD_RESERVED,          //KEY70
    HID_KEYBOARD_RESERVED,          //KEY71
    HID_KEYBOARD_RESERVED,          //KEY72
    HID_KEYBOARD_RESERVED,          //KEY73
    HID_KEYBOARD_RESERVED,          //KEY74
    HID_KEYBOARD_RESERVED,          //KEY75
    HID_KEYBOARD_DELETE_FWD,        //KEY76
    HID_KEYBOARD_RESERVED,          //KEY77
    HID_KEYBOARD_RESERVED,          //KEY78
    HID_KEYBOARD_LEFT_ARROW,        //KEY79
    HID_KEYBOARD_RESERVED,          //KEY80
    HID_KEYBOARD_RESERVED,          //KEY81
    HID_KEYBOARD_RESERVED,          //KEY82
    HID_KEYBOARD_UP_ARROW,          //KEY83
    HID_KEYBOARD_DOWN_ARROW,        //KEY84
    HID_KEYBOARD_RESERVED,          //KEY85
    HID_KEYBOARD_RESERVED,          //KEY86
    HID_KEYBOARD_RESERVED,          //KEY87
    HID_KEYBOARD_RESERVED,          //KEY88
    HID_KEYBOARD_RIGHT_ARROW,       //KEY89
    HID_KEYBOARD_RESERVED,          //KEY90
    HID_KEYBOARD_RESERVED,          //KEY91
    HID_KEYBOARD_RESERVED,          //KEY92
    HID_KEYBOARD_RESERVED,          //KEY93
    HID_KEYBOARD_RESERVED,          //KEY94
    HID_KEYBOARD_RESERVED,          //KEY95
    HID_KEYBOARD_RESERVED,          //KEY96
    HID_KEYBOARD_RESERVED,          //KEY97
    HID_KEYBOARD_RESERVED,          //KEY98
    HID_KEYBOARD_RESERVED,          //KEY99
    HID_KEYBOARD_RESERVED,          //KEY100
    HID_KEYBOARD_RESERVED,          //KEY101
    HID_KEYBOARD_RESERVED,          //KEY102
    HID_KEYBOARD_RESERVED,          //KEY103
    HID_KEYBOARD_RESERVED,          //KEY104
    HID_KEYBOARD_RESERVED,          //KEY105
    HID_KEYBOARD_RESERVED,          //KEY106
    HID_KEYBOARD_RESERVED,          //KEY107
    HID_KEYBOARD_RESERVED,          //KEY108
    HID_KEYBOARD_RESERVED,          //KEY109
    HID_KEYBOARD_ESCAPE,            //KEY110
    HID_KEYBOARD_RESERVED,          //KEY111
    HID_KEYBOARD_MUTE,              //KEY112
    HID_KEYBOARD_VOLUME_DOWN,       //KEY113
    HID_KEYBOARD_VOLUME_UP,         //KEY114
    HID_CONSUMER_REWIND,            //KEY115
    HID_CONSUMER_PLAY,              //KEY116
    HID_CONSUMER_FAST_FORWARD,      //KEY117
    HID_CONSUMER_SCAN_PREV_TRK,     //KEY118
    HID_CONSUMER_SCAN_NEXT_TRK,     //KEY119
    HID_KEYBOARD_RESERVED,          //KEY120
    HID_KEYBOARD_RESERVED,          //KEY121
    HID_KEYBOARD_RESERVED,          //KEY122
    HID_KEYBOARD_RESERVED,          //KEY123
    HID_KEYBOARD_RESERVED,          //KEY124
    HID_KEYBOARD_RESERVED,          //KEY125
    HID_KEYBOARD_RESERVED,          //KEY126
    HID_KEYBOARD_LEFT_GUI,          //KEY127
    HID_KEYBOARD_RESERVED,          //KEY128
    HID_KEYBOARD_RIGHT_GUI,         //KEY129
    HID_KEYBOARD_RESERVED,          //KEY130
    HID_KEYBOARD_RESERVED,          //KEY131
    HID_KEYBOARD_RESERVED,          //KEY132
    HID_KEYBOARD_RESERVED,          //KEY133
    HID_KEYBOARD_RESERVED,          //KEY134
    HID_KEYBOARD_RESERVED,          //KEY135
    HID_KEYBOARD_RESERVED,          //KEY136
    HID_KEYBOARD_RESERVED,          //KEY137
    HID_KEYBOARD_RESERVED,          //KEY138
    HID_KEYBOARD_RESERVED,          //KEY139
    HID_KEYBOARD_RESERVED,          //KEY140
    HID_KEYBOARD_RESERVED,          //KEY141
    HID_KEYBOARD_RESERVED,          //KEY142
    HID_KEYBOARD_RESERVED,          //KEY143
    HID_KEYBOARD_RESERVED,          //KEY144
    HID_KEYBOARD_RESERVED,          //KEY145
    HID_KEYBOARD_RESERVED,          //KEY146
    HID_KEYBOARD_RESERVED,          //KEY147
    HID_KEYBOARD_RESERVED,          //KEY148
    HID_KEYBOARD_RESERVED,          //KEY149
    HID_KEYBOARD_RESERVED,          //KEY150
    HID_KEYBOARD_RESERVED,          //KEY151
    HID_KEYBOARD_RESERVED,          //KEY152
    HID_KEYBOARD_RESERVED,          //KEY153
    HID_KEYBOARD_RESERVED,          //KEY154
    HID_KEYBOARD_RESERVED,          //KEY155
    HID_KEYBOARD_RESERVED,          //KEY156
    HID_KEYBOARD_RESERVED,          //KEY157
    HID_KEYBOARD_RESERVED,          //KEY158
    HID_KEYBOARD_RESERVED,          //KEY159
    HID_KEYBOARD_RESERVED,          //KEY160
    HID_KEYBOARD_RESERVED,          //KEY161
    HID_KEYBOARD_RESERVED,          //KEY162
    HID_KEYBOARD_RESERVED           //KEY163
};

const uint8_t releaseKey = KEY_RESERVED;
const uint8_t capsLock = KEY30;
const uint8_t leftShift = KEY44;
const uint8_t rightShift = KEY57;
const uint8_t leftCtrl = KEY58;
const uint8_t leftAlt = KEY60;
const uint8_t rightAlt = KEY62;
const uint8_t rightCtrl = KEY64;
const uint8_t leftArrow = KEY79;
const uint8_t upArrow = KEY83;
const uint8_t downArrow = KEY84;
const uint8_t rightArrow = KEY89;
//const uint8_t mediaStart = KEY112;
//const uint8_t mediaEnd = KEY119;
//const uint8_t leftGui = KEY127;
//const uint8_t rightGui = KEY129;
const uint8_t fnKey = KEY59;

