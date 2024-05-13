#include "ZCL_SmartLight.h"
#include "ZCL_SmartLight_Callbacks.h"
#include "HAL/hal_wsled.h"

void zclSmartLight_BasicResetCB(void);
void zclSmartLight_OnOffCB(uint8 cmd);
void zclSmartLight_MoveToLevelCB(zclLCMoveToLevel_t* pCmd);

ZStatus_t zclSmartLight_MoveToHueCB(zclCCMoveToHue_t* pCmd);
ZStatus_t zclSmartLight_MoveToSaturationCB(zclCCMoveToSaturation_t* pCmd);
ZStatus_t zclSmartLight_MoveToColorTemperatureCB(zclCCMoveToColorTemperature_t* pCmd);

// ZCL General Profile Callback table
zclGeneral_AppCallbacks_t zclSmartLight_CmdCallbacks = {
    zclSmartLight_BasicResetCB, // Basic Cluster Reset command
    NULL,                       // Identify Trigger Effect command
    zclSmartLight_OnOffCB,      // On/Off cluster commands
    NULL,                       // On/Off cluster enhanced command Off with Effect
    NULL,                       // On/Off cluster enhanced command On with Recall Global Scene
    NULL,                       // On/Off cluster enhanced command On with Timed Off
    #ifdef ZCL_LEVEL_CTRL
    zclSmartLight_MoveToLevelCB,// Level Control Move to Level command
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

zclLighting_AppCallbacks_t zclSmartLight_LightingCallbacks = {
    zclSmartLight_MoveToHueCB,
    NULL,
    NULL,
    zclSmartLight_MoveToSaturationCB,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    zclSmartLight_MoveToColorTemperatureCB,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
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
    if (state == LIGHT_ON) hal_wsled_setBrightness(zclSmartLight_CurrentLevel);
    else hal_wsled_setBrightness(0);
}

void zclSmartLight_MoveToLevelCB(zclLCMoveToLevel_t* pCmd) {
    zclSmartLight_CurrentLevel = pCmd->level;
    zclSmartLight_OnOff = pCmd->withOnOff;
    if (pCmd->withOnOff == LIGHT_ON) hal_wsled_setBrightness(pCmd->level);
    else hal_wsled_setBrightness(0);
}

ZStatus_t zclSmartLight_MoveToHueCB(zclCCMoveToHue_t* pCmd) {
    zclSmartLight_CurrentHue = pCmd->hue;
    if (zclSmartLight_CurrentHue < 1) zclSmartLight_CurrentHue = 1;
    if (zclSmartLight_CurrentHue > 254) zclSmartLight_CurrentHue = 254;

    zclSmartLight_ColorMode = COLOR_MODE_CURRENT_HUE_SATURATION;
    hal_wsled_setHueSat(zclSmartLight_CurrentHue, zclSmartLight_CurrentSaturation);
    return SUCCESS;
}

ZStatus_t zclSmartLight_MoveToSaturationCB(zclCCMoveToSaturation_t* pCmd) {
    zclSmartLight_CurrentSaturation = pCmd->saturation;
    if (zclSmartLight_CurrentSaturation < 1) zclSmartLight_CurrentSaturation = 1;
    if (zclSmartLight_CurrentSaturation > 254) zclSmartLight_CurrentSaturation = 254;

    zclSmartLight_ColorMode = COLOR_MODE_CURRENT_HUE_SATURATION;
    hal_wsled_setHueSat(zclSmartLight_CurrentHue, zclSmartLight_CurrentSaturation);
    return SUCCESS;
}

ZStatus_t zclSmartLight_MoveToColorTemperatureCB(zclCCMoveToColorTemperature_t* pCmd) {
    zclSmartLight_ColorTemperature = pCmd->colorTemperature;
    if (zclSmartLight_ColorTemperature < 1) zclSmartLight_ColorTemperature = 1;
    if (zclSmartLight_ColorTemperature > 500) zclSmartLight_ColorTemperature = 500;
    
    zclSmartLight_ColorMode = COLOR_MODE_COLOR_TEMPERATURE;
    hal_wsled_setColorTemp(zclSmartLight_ColorTemperature);
    return SUCCESS;
}
