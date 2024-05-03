#include "ZCL_SmartLight.h"
#include "HAL/HAL_WsLed.h"
#include "ZCL/ZCL_SmartLight_General.h"

void zclSmartLight_BasicResetCB(void);
void zclSmartLight_OnOffCB(uint8 cmd);

// ZCL General Profile Callback table
zclGeneral_AppCallbacks_t zclSmartLight_CmdCallbacks = {
  zclSmartLight_BasicResetCB, // Basic Cluster Reset command
  NULL,                       // Identify Trigger Effect command
  zclSmartLight_OnOffCB,      // On/Off cluster commands
  NULL,                       // On/Off cluster enhanced command Off with Effect
  NULL,                       // On/Off cluster enhanced command On with Recall Global Scene
  NULL,                       // On/Off cluster enhanced command On with Timed Off
#ifdef ZCL_LEVEL_CTRL
  NULL,                       // Level Control Move to Level command
  NULL,                       // Level Control Move command
  NULL,                       // Level Control Step command
  NULL,                       // Level Control Stop command
#endif
#ifdef ZCL_GROUPS
  NULL,                       // Group Response commands
#endif
#ifdef ZCL_SCENES
  NULL,                       // Scene Store Request command
  NULL,                       // Scene Recall Request command
  NULL,                       // Scene Response command
#endif
#ifdef ZCL_ALARMS
  NULL,                       // Alarm (Response) commands
#endif
#ifdef SE_UK_EXT
  NULL,                       // Get Event Log command
  NULL,                       // Publish Event Log command
#endif
  NULL,                       // RSSI Location command
  NULL                        // RSSI Location Response command
};

// Callback from the ZCL General Cluster Library to set all the Basic Cluster attributes to default values.
void zclSmartLight_BasicResetCB(void) {
    /* SMARTLIGHT_TODO: remember to update this function with any
       application-specific cluster attribute variables */
    zclSmartLight_ResetAttributesToDefaultValues();
}

// Callback from the ZCL General Cluster Library when it received an On / Off Command for this application.
void zclSmartLight_OnOffCB(uint8 cmd) {
    uint8 state;
    switch (cmd) {
    case COMMAND_ON:
        state = LIGHT_ON;
        break;
    case COMMAND_OFF:
        state = LIGHT_OFF;
        break;
    case COMMAND_TOGGLE:
        state = zclSmartLight_OnOff == LIGHT_ON ? LIGHT_OFF : LIGHT_ON;
    default:
        state = LIGHT_OFF;
        break;
    }
    zclSmartLight_OnOff = state;
    if (state == LIGHT_ON) Hal_WsLed_SetRGB(255, 255, 255);
    else Hal_WsLed_SetRGB(0, 0, 0);
}
