/*********************************************************************
 * INCLUDES
 */
#include "ZComDef.h"
#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "MT_SYS.h"

#include "nwk_util.h"

#include "zcl.h"
#include "zcl_general.h"
#include "zcl_ha.h"
#include "zcl_diagnostic.h"
#include "ZCL_SmartLight.h"

#include "bdb.h"
#include "bdb_interface.h"
#include "gp_interface.h"

#if defined ( INTER_PAN )
#if defined ( BDB_TL_INITIATOR )
  #include "bdb_touchlink_initiator.h"
#endif // BDB_TL_INITIATOR
#if defined ( BDB_TL_TARGET )
  #include "bdb_touchlink_target.h"
#endif // BDB_TL_TARGET
#endif // INTER_PAN

#if defined ( BDB_TL_INITIATOR ) || defined ( BDB_TL_TARGET )
  #include "bdb_touchlink.h"
#endif

#include "onboard.h"

/* HAL */
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"

/*********************************************************************
 * MACROS
 */


/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
byte zclSmartLight_TaskID;


/*********************************************************************
 * GLOBAL FUNCTIONS
 */
 
/*********************************************************************
 * LOCAL VARIABLES
 */

uint8 giSmartLightScreenMode = SMARTLIGHT_MAINMODE;   // display the main screen mode first

uint8 gPermitDuration = 0;    // permit joining default to disabled

devStates_t zclSmartLight_NwkState = DEV_INIT;


/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void zclSmartLight_HandleKeys( byte shift, byte keys );
static void zclSmartLight_BasicResetCB( void );
static void zclSmartLight_ProcessIdentifyTimeChange( uint8 endpoint );
static void zclSmartLight_BindNotification( bdbBindNotificationData_t *data );
#if ( defined ( BDB_TL_TARGET ) && (BDB_TOUCHLINK_CAPABILITY_ENABLED == TRUE) )
static void zclSmartLight_ProcessTouchlinkTargetEnable( uint8 enable );
#endif

static void zclSmartLight_ProcessCommissioningStatus(bdbCommissioningModeMsg_t *bdbCommissioningModeMsg);

// app display functions
static void zclSmartLight_LcdDisplayUpdate( void );
#ifdef LCD_SUPPORTED
static void zclSmartLight_LcdDisplayMainMode( void );
static void zclSmartLight_LcdDisplayHelpMode( void );
#endif

// Functions to process ZCL Foundation incoming Command/Response messages
static void zclSmartLight_ProcessIncomingMsg( zclIncomingMsg_t *msg );
#ifdef ZCL_READ
static uint8 zclSmartLight_ProcessInReadRspCmd( zclIncomingMsg_t *pInMsg );
#endif
#ifdef ZCL_WRITE
static uint8 zclSmartLight_ProcessInWriteRspCmd( zclIncomingMsg_t *pInMsg );
#endif
static uint8 zclSmartLight_ProcessInDefaultRspCmd( zclIncomingMsg_t *pInMsg );
#ifdef ZCL_DISCOVER
static uint8 zclSmartLight_ProcessInDiscCmdsRspCmd( zclIncomingMsg_t *pInMsg );
static uint8 zclSmartLight_ProcessInDiscAttrsRspCmd( zclIncomingMsg_t *pInMsg );
static uint8 zclSmartLight_ProcessInDiscAttrsExtRspCmd( zclIncomingMsg_t *pInMsg );
#endif

static void zclSampleApp_BatteryWarningCB( uint8 voltLevel);

/*********************************************************************
 * STATUS STRINGS
 */
#ifdef LCD_SUPPORTED
const char sDeviceName[]   = "  SmartLight";
const char sClearLine[]    = " ";
const char sSwSmartLight[]      = "SW1:SMARTLIGHT_TODO";  // SMARTLIGHT_TODO
const char sSwBDBMode[]     = "SW2: Start BDB";
char sSwHelp[]             = "SW4: Help       ";  // last character is * if NWK open
#endif

/*********************************************************************
 * ZCL General Profile Callback table
 */
static zclGeneral_AppCallbacks_t zclSmartLight_CmdCallbacks =
{
  zclSmartLight_BasicResetCB,             // Basic Cluster Reset command
  NULL,                                   // Identify Trigger Effect command
  NULL,                                   // On/Off cluster commands
  NULL,                                   // On/Off cluster enhanced command Off with Effect
  NULL,                                   // On/Off cluster enhanced command On with Recall Global Scene
  NULL,                                   // On/Off cluster enhanced command On with Timed Off
#ifdef ZCL_LEVEL_CTRL
  NULL,                                   // Level Control Move to Level command
  NULL,                                   // Level Control Move command
  NULL,                                   // Level Control Step command
  NULL,                                   // Level Control Stop command
#endif
#ifdef ZCL_GROUPS
  NULL,                                   // Group Response commands
#endif
#ifdef ZCL_SCENES
  NULL,                                  // Scene Store Request command
  NULL,                                  // Scene Recall Request command
  NULL,                                  // Scene Response command
#endif
#ifdef ZCL_ALARMS
  NULL,                                  // Alarm (Response) commands
#endif
#ifdef SE_UK_EXT
  NULL,                                  // Get Event Log command
  NULL,                                  // Publish Event Log command
#endif
  NULL,                                  // RSSI Location command
  NULL                                   // RSSI Location Response command
};

/*********************************************************************
 * SMARTLIGHT_TODO: Add other callback structures for any additional application specific 
 *       Clusters being used, see available callback structures below.
 *
 *       bdbTL_AppCallbacks_t 
 *       zclApplianceControl_AppCallbacks_t 
 *       zclApplianceEventsAlerts_AppCallbacks_t 
 *       zclApplianceStatistics_AppCallbacks_t 
 *       zclElectricalMeasurement_AppCallbacks_t 
 *       zclGeneral_AppCallbacks_t 
 *       zclGp_AppCallbacks_t 
 *       zclHVAC_AppCallbacks_t 
 *       zclLighting_AppCallbacks_t 
 *       zclMS_AppCallbacks_t 
 *       zclPollControl_AppCallbacks_t 
 *       zclPowerProfile_AppCallbacks_t 
 *       zclSS_AppCallbacks_t  
 *
 */

/*********************************************************************
 * @fn          zclSmartLight_Init
 *
 * @brief       Initialization function for the zclGeneral layer.
 *
 * @param       none
 *
 * @return      none
 */
void zclSmartLight_Init( byte task_id )
{
  zclSmartLight_TaskID = task_id;

  // This app is part of the Home Automation Profile
  bdb_RegisterSimpleDescriptor( &zclSmartLight_SimpleDesc );

  // Register the ZCL General Cluster Library callback functions
  zclGeneral_RegisterCmdCallbacks( SMARTLIGHT_ENDPOINT, &zclSmartLight_CmdCallbacks );
  
  // SMARTLIGHT_TODO: Register other cluster command callbacks here

  // Register the application's attribute list
  zcl_registerAttrList( SMARTLIGHT_ENDPOINT, zclSmartLight_NumAttributes, zclSmartLight_Attrs );

  // Register the Application to receive the unprocessed Foundation command/response messages
  zcl_registerForMsg( zclSmartLight_TaskID );

#ifdef ZCL_DISCOVER
  // Register the application's command list
  zcl_registerCmdList( SMARTLIGHT_ENDPOINT, zclCmdsArraySize, zclSmartLight_Cmds );
#endif

  // Register low voltage NV memory protection application callback
  RegisterVoltageWarningCB( zclSampleApp_BatteryWarningCB );

  // Register for all key events - This app will handle all key events
  RegisterForKeys( zclSmartLight_TaskID );

  bdb_RegisterCommissioningStatusCB( zclSmartLight_ProcessCommissioningStatus );
  bdb_RegisterIdentifyTimeChangeCB( zclSmartLight_ProcessIdentifyTimeChange );
  bdb_RegisterBindNotificationCB( zclSmartLight_BindNotification );

#if ( defined ( BDB_TL_TARGET ) && (BDB_TOUCHLINK_CAPABILITY_ENABLED == TRUE) )
  bdb_RegisterTouchlinkTargetEnableCB( zclSmartLight_ProcessTouchlinkTargetEnable );
#endif

#ifdef ZCL_DIAGNOSTIC
  // Register the application's callback function to read/write attribute data.
  // This is only required when the attribute data format is unknown to ZCL.
  zcl_registerReadWriteCB( SMARTLIGHT_ENDPOINT, zclDiagnostic_ReadWriteAttrCB, NULL );

  if ( zclDiagnostic_InitStats() == ZSuccess )
  {
    // Here the user could start the timer to save Diagnostics to NV
  }
#endif


#ifdef LCD_SUPPORTED
  HalLcdWriteString ( (char *)sDeviceName, HAL_LCD_LINE_3 );
#endif  // LCD_SUPPORTED


}

/*********************************************************************
 * @fn          zclSample_event_loop
 *
 * @brief       Event Loop Processor for zclGeneral.
 *
 * @param       none
 *
 * @return      none
 */
uint16 zclSmartLight_event_loop( uint8 task_id, uint16 events )
{
  afIncomingMSGPacket_t *MSGpkt;

  (void)task_id;  // Intentionally unreferenced parameter

  if ( events & SYS_EVENT_MSG )
  {
    while ( (MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( zclSmartLight_TaskID )) )
    {
      switch ( MSGpkt->hdr.event )
      {
        case ZCL_INCOMING_MSG:
          // Incoming ZCL Foundation command/response messages
          zclSmartLight_ProcessIncomingMsg( (zclIncomingMsg_t *)MSGpkt );
          break;

        case KEY_CHANGE:
          zclSmartLight_HandleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
          break;

        case ZDO_STATE_CHANGE:
          zclSmartLight_NwkState = (devStates_t)(MSGpkt->hdr.status);

          // now on the network
          if ( (zclSmartLight_NwkState == DEV_ZB_COORD) ||
               (zclSmartLight_NwkState == DEV_ROUTER)   ||
               (zclSmartLight_NwkState == DEV_END_DEVICE) )
          {
            giSmartLightScreenMode = SMARTLIGHT_MAINMODE;
            zclSmartLight_LcdDisplayUpdate();
          }
          break;

        default:
          break;
      }

      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  if ( events & SMARTLIGHT_MAIN_SCREEN_EVT )
  {
    giSmartLightScreenMode = SMARTLIGHT_MAINMODE;
    zclSmartLight_LcdDisplayUpdate();

    return ( events ^ SMARTLIGHT_MAIN_SCREEN_EVT );
  }
  
#if ZG_BUILD_ENDDEVICE_TYPE    
  if ( events & SMARTLIGHT_END_DEVICE_REJOIN_EVT )
  {
    bdb_ZedAttemptRecoverNwk();
    return ( events ^ SMARTLIGHT_END_DEVICE_REJOIN_EVT );
  }
#endif

  /* SMARTLIGHT_TODO: handle app events here */
  
  
  if ( events & SMARTLIGHT_EVT_1 )
  {
    // toggle LED 2 state, start another timer for 500ms
    HalLedSet ( HAL_LED_2, HAL_LED_MODE_TOGGLE );
    osal_start_timerEx( zclSmartLight_TaskID, SMARTLIGHT_EVT_1, 500 );
    
    return ( events ^ SMARTLIGHT_EVT_1 );
  }
  
  /*
  if ( events & SMARTLIGHT_EVT_2 )
  {
    
    return ( events ^ SMARTLIGHT_EVT_2 );
  }
  
  if ( events & SMARTLIGHT_EVT_3 )
  {
    
    return ( events ^ SMARTLIGHT_EVT_3 );
  }
  */
  
  // Discard unknown events
  return 0;
}


/*********************************************************************
 * @fn      zclSmartLight_HandleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *                 HAL_KEY_SW_5
 *                 HAL_KEY_SW_4
 *                 HAL_KEY_SW_2
 *                 HAL_KEY_SW_1
 *
 * @return  none
 */
static void zclSmartLight_HandleKeys( byte shift, byte keys )
{
  if ( keys & HAL_KEY_SW_1 )
  {
    static bool LED_OnOff = FALSE;
    
    giSmartLightScreenMode = SMARTLIGHT_MAINMODE;
    
    /* SMARTLIGHT_TODO: add app functionality to hardware keys here */
    
    // for example, start/stop LED 2 toggling with 500ms period
    if (LED_OnOff)
    { 
      // if the LED is blinking, stop the osal timer and turn the LED off
      osal_stop_timerEx(zclSmartLight_TaskID, SMARTLIGHT_EVT_1);
      HalLedSet ( HAL_LED_2, HAL_LED_MODE_OFF );
      LED_OnOff = FALSE;
    }
    else
    {
      // turn on LED 2 and start an osal timer to toggle it after 500ms, search
      // for SMARTLIGHT_EVT_1 to see event handling after expired timer
      osal_start_timerEx( zclSmartLight_TaskID, SMARTLIGHT_EVT_1, 500 );
      HalLedSet ( HAL_LED_2, HAL_LED_MODE_ON );
      LED_OnOff = TRUE;
    }
  }
  // Start the BDB commissioning method
  if ( keys & HAL_KEY_SW_2 )
  {
    giSmartLightScreenMode = SMARTLIGHT_MAINMODE;

    bdb_StartCommissioning(BDB_COMMISSIONING_MODE_NWK_FORMATION | BDB_COMMISSIONING_MODE_NWK_STEERING | BDB_COMMISSIONING_MODE_FINDING_BINDING | BDB_COMMISSIONING_MODE_INITIATOR_TL);
  }
  if ( keys & HAL_KEY_SW_3 )
  {
    giSmartLightScreenMode = SMARTLIGHT_MAINMODE;
  
    // touchlink target commissioning, if enabled  
#if ( defined ( BDB_TL_TARGET ) && (BDB_TOUCHLINK_CAPABILITY_ENABLED == TRUE) )
    bdb_StartCommissioning(BDB_COMMISSIONING_MODE_FINDING_BINDING);
    touchLinkTarget_EnableCommissioning( 30000 );
#endif
    
  }
  if ( keys & HAL_KEY_SW_4 )
  {
    
   giSmartLightScreenMode = giSmartLightScreenMode ? SMARTLIGHT_MAINMODE : SMARTLIGHT_HELPMODE;
#ifdef LCD_SUPPORTED
    HalLcdWriteString( (char *)sClearLine, HAL_LCD_LINE_2 );
#endif
    
  }
  if ( keys & HAL_KEY_SW_5 )
  {
    bdb_resetLocalAction();
  }

  zclSmartLight_LcdDisplayUpdate();
}

/*********************************************************************
 * @fn      zclSmartLight_LcdDisplayUpdate
 *
 * @brief   Called to update the LCD display.
 *
 * @param   none
 *
 * @return  none
 */
void zclSmartLight_LcdDisplayUpdate( void )
{
#ifdef LCD_SUPPORTED
  if ( giSmartLightScreenMode == SMARTLIGHT_HELPMODE )
  {
    zclSmartLight_LcdDisplayHelpMode();
  }
  else
  {
    zclSmartLight_LcdDisplayMainMode();
  }
#endif
}

#ifdef LCD_SUPPORTED
/*********************************************************************
 * @fn      zclSmartLight_LcdDisplayMainMode
 *
 * @brief   Called to display the main screen on the LCD.
 *
 * @param   none
 *
 * @return  none
 */
static void zclSmartLight_LcdDisplayMainMode( void )
{
  // display line 1 to indicate NWK status
  if ( zclSmartLight_NwkState == DEV_ZB_COORD )
  {
    zclHA_LcdStatusLine1( ZCL_HA_STATUSLINE_ZC );
  }
  else if ( zclSmartLight_NwkState == DEV_ROUTER )
  {
    zclHA_LcdStatusLine1( ZCL_HA_STATUSLINE_ZR );
  }
  else if ( zclSmartLight_NwkState == DEV_END_DEVICE )
  {
    zclHA_LcdStatusLine1( ZCL_HA_STATUSLINE_ZED );
  }

  // end of line 3 displays permit join status (*)
  if ( gPermitDuration )
  {
    sSwHelp[15] = '*';
  }
  else
  {
    sSwHelp[15] = ' ';
  }
  HalLcdWriteString( (char *)sSwHelp, HAL_LCD_LINE_3 );
}

/*********************************************************************
 * @fn      zclSmartLight_LcdDisplayHelpMode
 *
 * @brief   Called to display the SW options on the LCD.
 *
 * @param   none
 *
 * @return  none
 */
static void zclSmartLight_LcdDisplayHelpMode( void )
{
  HalLcdWriteString( (char *)sSwSmartLight, HAL_LCD_LINE_1 );
  HalLcdWriteString( (char *)sSwBDBMode, HAL_LCD_LINE_2 );
  HalLcdWriteString( (char *)sSwHelp, HAL_LCD_LINE_3 );
}
#endif  // LCD_SUPPORTED

/*********************************************************************
 * @fn      zclSmartLight_ProcessCommissioningStatus
 *
 * @brief   Callback in which the status of the commissioning process are reported
 *
 * @param   bdbCommissioningModeMsg - Context message of the status of a commissioning process
 *
 * @return  none
 */
static void zclSmartLight_ProcessCommissioningStatus(bdbCommissioningModeMsg_t *bdbCommissioningModeMsg)
{
  switch(bdbCommissioningModeMsg->bdbCommissioningMode)
  {
    case BDB_COMMISSIONING_FORMATION:
      if(bdbCommissioningModeMsg->bdbCommissioningStatus == BDB_COMMISSIONING_SUCCESS)
      {
        //After formation, perform nwk steering again plus the remaining commissioning modes that has not been process yet
        bdb_StartCommissioning(BDB_COMMISSIONING_MODE_NWK_STEERING | bdbCommissioningModeMsg->bdbRemainingCommissioningModes);
      }
      else
      {
        //Want to try other channels?
        //try with bdb_setChannelAttribute
      }
    break;
    case BDB_COMMISSIONING_NWK_STEERING:
      if(bdbCommissioningModeMsg->bdbCommissioningStatus == BDB_COMMISSIONING_SUCCESS)
      {
        //YOUR JOB:
        //We are on the nwk, what now?
      }
      else
      {
        //See the possible errors for nwk steering procedure
        //No suitable networks found
        //Want to try other channels?
        //try with bdb_setChannelAttribute
      }
    break;
    case BDB_COMMISSIONING_FINDING_BINDING:
      if(bdbCommissioningModeMsg->bdbCommissioningStatus == BDB_COMMISSIONING_SUCCESS)
      {
        //YOUR JOB:
      }
      else
      {
        //YOUR JOB:
        //retry?, wait for user interaction?
      }
    break;
    case BDB_COMMISSIONING_INITIALIZATION:
      //Initialization notification can only be successful. Failure on initialization
      //only happens for ZED and is notified as BDB_COMMISSIONING_PARENT_LOST notification

      //YOUR JOB:
      //We are on a network, what now?

    break;
#if ZG_BUILD_ENDDEVICE_TYPE    
    case BDB_COMMISSIONING_PARENT_LOST:
      if(bdbCommissioningModeMsg->bdbCommissioningStatus == BDB_COMMISSIONING_NETWORK_RESTORED)
      {
        //We did recover from losing parent
      }
      else
      {
        //Parent not found, attempt to rejoin again after a fixed delay
        osal_start_timerEx(zclSmartLight_TaskID, SMARTLIGHT_END_DEVICE_REJOIN_EVT, SMARTLIGHT_END_DEVICE_REJOIN_DELAY);
      }
    break;
#endif 
  }
}

/*********************************************************************
 * @fn      zclSmartLight_ProcessIdentifyTimeChange
 *
 * @brief   Called to process any change to the IdentifyTime attribute.
 *
 * @param   endpoint - in which the identify has change
 *
 * @return  none
 */
static void zclSmartLight_ProcessIdentifyTimeChange( uint8 endpoint )
{
  (void) endpoint;

  if ( zclSmartLight_IdentifyTime > 0 )
  {
    HalLedBlink ( HAL_LED_2, 0xFF, HAL_LED_DEFAULT_DUTY_CYCLE, HAL_LED_DEFAULT_FLASH_TIME );
  }
  else
  {
    HalLedSet ( HAL_LED_2, HAL_LED_MODE_OFF );
  }
}

/*********************************************************************
 * @fn      zclSmartLight_BindNotification
 *
 * @brief   Called when a new bind is added.
 *
 * @param   data - pointer to new bind data
 *
 * @return  none
 */
static void zclSmartLight_BindNotification( bdbBindNotificationData_t *data )
{
  // SMARTLIGHT_TODO: process the new bind information
}


/*********************************************************************
 * @fn      zclSmartLight_ProcessTouchlinkTargetEnable
 *
 * @brief   Called to process when the touchlink target functionality
 *          is enabled or disabled
 *
 * @param   none
 *
 * @return  none
 */
#if ( defined ( BDB_TL_TARGET ) && (BDB_TOUCHLINK_CAPABILITY_ENABLED == TRUE) )
static void zclSmartLight_ProcessTouchlinkTargetEnable( uint8 enable )
{
  if ( enable )
  {
    HalLedSet ( HAL_LED_1, HAL_LED_MODE_ON );
  }
  else
  {
    HalLedSet ( HAL_LED_1, HAL_LED_MODE_OFF );
  }
}
#endif

/*********************************************************************
 * @fn      zclSmartLight_BasicResetCB
 *
 * @brief   Callback from the ZCL General Cluster Library
 *          to set all the Basic Cluster attributes to default values.
 *
 * @param   none
 *
 * @return  none
 */
static void zclSmartLight_BasicResetCB( void )
{

  /* SMARTLIGHT_TODO: remember to update this function with any
     application-specific cluster attribute variables */
  
  zclSmartLight_ResetAttributesToDefaultValues();
  
}
/*********************************************************************
 * @fn      zclSampleApp_BatteryWarningCB
 *
 * @brief   Called to handle battery-low situation.
 *
 * @param   voltLevel - level of severity
 *
 * @return  none
 */
void zclSampleApp_BatteryWarningCB( uint8 voltLevel )
{
  if ( voltLevel == VOLT_LEVEL_CAUTIOUS )
  {
    // Send warning message to the gateway and blink LED
  }
  else if ( voltLevel == VOLT_LEVEL_BAD )
  {
    // Shut down the system
  }
}

/******************************************************************************
 *
 *  Functions for processing ZCL Foundation incoming Command/Response messages
 *
 *****************************************************************************/

/*********************************************************************
 * @fn      zclSmartLight_ProcessIncomingMsg
 *
 * @brief   Process ZCL Foundation incoming message
 *
 * @param   pInMsg - pointer to the received message
 *
 * @return  none
 */
static void zclSmartLight_ProcessIncomingMsg( zclIncomingMsg_t *pInMsg )
{
  switch ( pInMsg->zclHdr.commandID )
  {
#ifdef ZCL_READ
    case ZCL_CMD_READ_RSP:
      zclSmartLight_ProcessInReadRspCmd( pInMsg );
      break;
#endif
#ifdef ZCL_WRITE
    case ZCL_CMD_WRITE_RSP:
      zclSmartLight_ProcessInWriteRspCmd( pInMsg );
      break;
#endif
    case ZCL_CMD_CONFIG_REPORT:
    case ZCL_CMD_CONFIG_REPORT_RSP:
    case ZCL_CMD_READ_REPORT_CFG:
    case ZCL_CMD_READ_REPORT_CFG_RSP:
    case ZCL_CMD_REPORT:
      //bdb_ProcessIncomingReportingMsg( pInMsg );
      break;
      
    case ZCL_CMD_DEFAULT_RSP:
      zclSmartLight_ProcessInDefaultRspCmd( pInMsg );
      break;
#ifdef ZCL_DISCOVER
    case ZCL_CMD_DISCOVER_CMDS_RECEIVED_RSP:
      zclSmartLight_ProcessInDiscCmdsRspCmd( pInMsg );
      break;

    case ZCL_CMD_DISCOVER_CMDS_GEN_RSP:
      zclSmartLight_ProcessInDiscCmdsRspCmd( pInMsg );
      break;

    case ZCL_CMD_DISCOVER_ATTRS_RSP:
      zclSmartLight_ProcessInDiscAttrsRspCmd( pInMsg );
      break;

    case ZCL_CMD_DISCOVER_ATTRS_EXT_RSP:
      zclSmartLight_ProcessInDiscAttrsExtRspCmd( pInMsg );
      break;
#endif
    default:
      break;
  }

  if ( pInMsg->attrCmd )
    osal_mem_free( pInMsg->attrCmd );
}

#ifdef ZCL_READ
/*********************************************************************
 * @fn      zclSmartLight_ProcessInReadRspCmd
 *
 * @brief   Process the "Profile" Read Response Command
 *
 * @param   pInMsg - incoming message to process
 *
 * @return  none
 */
static uint8 zclSmartLight_ProcessInReadRspCmd( zclIncomingMsg_t *pInMsg )
{
  zclReadRspCmd_t *readRspCmd;
  uint8 i;

  readRspCmd = (zclReadRspCmd_t *)pInMsg->attrCmd;
  for (i = 0; i < readRspCmd->numAttr; i++)
  {
    // Notify the originator of the results of the original read attributes
    // attempt and, for each successfull request, the value of the requested
    // attribute
  }

  return ( TRUE );
}
#endif // ZCL_READ

#ifdef ZCL_WRITE
/*********************************************************************
 * @fn      zclSmartLight_ProcessInWriteRspCmd
 *
 * @brief   Process the "Profile" Write Response Command
 *
 * @param   pInMsg - incoming message to process
 *
 * @return  none
 */
static uint8 zclSmartLight_ProcessInWriteRspCmd( zclIncomingMsg_t *pInMsg )
{
  zclWriteRspCmd_t *writeRspCmd;
  uint8 i;

  writeRspCmd = (zclWriteRspCmd_t *)pInMsg->attrCmd;
  for ( i = 0; i < writeRspCmd->numAttr; i++ )
  {
    // Notify the device of the results of the its original write attributes
    // command.
  }

  return ( TRUE );
}
#endif // ZCL_WRITE

/*********************************************************************
 * @fn      zclSmartLight_ProcessInDefaultRspCmd
 *
 * @brief   Process the "Profile" Default Response Command
 *
 * @param   pInMsg - incoming message to process
 *
 * @return  none
 */
static uint8 zclSmartLight_ProcessInDefaultRspCmd( zclIncomingMsg_t *pInMsg )
{
  // zclDefaultRspCmd_t *defaultRspCmd = (zclDefaultRspCmd_t *)pInMsg->attrCmd;

  // Device is notified of the Default Response command.
  (void)pInMsg;

  return ( TRUE );
}

#ifdef ZCL_DISCOVER
/*********************************************************************
 * @fn      zclSmartLight_ProcessInDiscCmdsRspCmd
 *
 * @brief   Process the Discover Commands Response Command
 *
 * @param   pInMsg - incoming message to process
 *
 * @return  none
 */
static uint8 zclSmartLight_ProcessInDiscCmdsRspCmd( zclIncomingMsg_t *pInMsg )
{
  zclDiscoverCmdsCmdRsp_t *discoverRspCmd;
  uint8 i;

  discoverRspCmd = (zclDiscoverCmdsCmdRsp_t *)pInMsg->attrCmd;
  for ( i = 0; i < discoverRspCmd->numCmd; i++ )
  {
    // Device is notified of the result of its attribute discovery command.
  }

  return ( TRUE );
}

/*********************************************************************
 * @fn      zclSmartLight_ProcessInDiscAttrsRspCmd
 *
 * @brief   Process the "Profile" Discover Attributes Response Command
 *
 * @param   pInMsg - incoming message to process
 *
 * @return  none
 */
static uint8 zclSmartLight_ProcessInDiscAttrsRspCmd( zclIncomingMsg_t *pInMsg )
{
  zclDiscoverAttrsRspCmd_t *discoverRspCmd;
  uint8 i;

  discoverRspCmd = (zclDiscoverAttrsRspCmd_t *)pInMsg->attrCmd;
  for ( i = 0; i < discoverRspCmd->numAttr; i++ )
  {
    // Device is notified of the result of its attribute discovery command.
  }

  return ( TRUE );
}

/*********************************************************************
 * @fn      zclSmartLight_ProcessInDiscAttrsExtRspCmd
 *
 * @brief   Process the "Profile" Discover Attributes Extended Response Command
 *
 * @param   pInMsg - incoming message to process
 *
 * @return  none
 */
static uint8 zclSmartLight_ProcessInDiscAttrsExtRspCmd( zclIncomingMsg_t *pInMsg )
{
  zclDiscoverAttrsExtRsp_t *discoverRspCmd;
  uint8 i;

  discoverRspCmd = (zclDiscoverAttrsExtRsp_t *)pInMsg->attrCmd;
  for ( i = 0; i < discoverRspCmd->numAttr; i++ )
  {
    // Device is notified of the result of its attribute discovery command.
  }

  return ( TRUE );
}
#endif // ZCL_DISCOVER

/****************************************************************************
****************************************************************************/


