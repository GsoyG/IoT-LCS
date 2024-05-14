#include "zcl_general.h"
#include "zcl_lighting.h"

#define LIGHT_OFF 0x00
#define LIGHT_ON 0x01

// OnOff attributes
extern uint8 zclSmartLight_OnOff;

// Level Control attributes
extern uint8 zclSmartLight_CurrentLevel;

// Lighting Color Control attributes
extern uint8 zclSmartLight_CurrentHue;
extern uint8 zclSmartLight_CurrentSaturation;
extern uint16 zclSmartLight_AutoDimmerMode;
extern uint16 zclSmartLight_ColorTemperature;
extern uint8 zclSmartLight_ColorMode;

// ZCL General Profile Callback table
extern zclGeneral_AppCallbacks_t zclSmartLight_CmdCallbacks;

// ZCL Lighting Profile Callback table
extern zclLighting_AppCallbacks_t zclSmartLight_LightingCallbacks;
