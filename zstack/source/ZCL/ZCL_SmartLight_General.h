#include "zcl_general.h"

#define LIGHT_OFF 0x00
#define LIGHT_ON 0x01

// OnOff attributes
extern uint8 zclSmartLight_OnOff;

// ZCL General Profile Callback table
extern zclGeneral_AppCallbacks_t zclSmartLight_CmdCallbacks;

extern void zclSmartLight_BasicResetCB(void);
extern void zclSmartLight_OnOffCB(uint8 cmd);
