#include "ZComDef.h"
#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "MT_SYS.h"

#include "nwk_util.h"

#include "zcl.h"
#include "zcl_ha.h"
#include "zcl_ms.h"
#include "zcl_diagnostic.h"
#include "ZCL_SmartLight.h"
#include "ZCL/ZCL_SmartLight_General.h"

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

#include "HAL/hal_i2c.h"
#include "HAL/hal_wsled.h"
#include "HAL/hal_hdc1080.h"
#include "HAL/hal_bh1750.h"

// GLOBAL VARIABLES

byte zclSmartLight_TaskID;

devStates_t zclSmartLight_NwkState = DEV_INIT;

// Illuminance Measurement Cluster
extern uint16 zclSmartLight_Illuminance;

// Temperature Measurement Cluster
extern uint16 zclSmartLight_Temperature;

// Relative Humidity Cluster
extern uint16 zclSmartLight_Humidity;

// LOCAL FUNCTIONS

static void zclSampleApp_BatteryWarningCB(uint8 voltLevel);

static void zclSmartLight_ProcessIdentifyTimeChange( uint8 endpoint );
static void zclSmartLight_BindNotification( bdbBindNotificationData_t *data );
#if ( defined ( BDB_TL_TARGET ) && (BDB_TOUCHLINK_CAPABILITY_ENABLED == TRUE) )
static void zclSmartLight_ProcessTouchlinkTargetEnable( uint8 enable );
#endif

void zclSmartLight_Reporting(void);

static void zclSmartLight_ProcessCommissioningStatus(bdbCommissioningModeMsg_t *bdbCommissioningModeMsg);

// Functions to process ZCL Foundation incoming Command/Response messages
static void zclSmartLight_ProcessIncomingMsg( zclIncomingMsg_t *msg );
static uint8 zclSmartLight_ProcessInDefaultRspCmd(zclIncomingMsg_t* pInMsg);
#ifdef ZCL_READ
static uint8 zclSmartLight_ProcessInReadRspCmd( zclIncomingMsg_t *pInMsg );
#endif
#ifdef ZCL_WRITE
static uint8 zclSmartLight_ProcessInWriteRspCmd( zclIncomingMsg_t *pInMsg );
#endif
#ifdef ZCL_DISCOVER
static uint8 zclSmartLight_ProcessInDiscCmdsRspCmd( zclIncomingMsg_t *pInMsg );
static uint8 zclSmartLight_ProcessInDiscAttrsRspCmd( zclIncomingMsg_t *pInMsg );
static uint8 zclSmartLight_ProcessInDiscAttrsExtRspCmd( zclIncomingMsg_t *pInMsg );
#endif

/*********************************************************************
 * SMARTLIGHT_TODO: Add other callback structures for any additional application specific 
 * Clusters being used, see available callback structures below.
 *
 * bdbTL_AppCallbacks_t 
 * zclApplianceControl_AppCallbacks_t 
 * zclApplianceEventsAlerts_AppCallbacks_t 
 * zclApplianceStatistics_AppCallbacks_t 
 * zclElectricalMeasurement_AppCallbacks_t 
 * zclGeneral_AppCallbacks_t 
 * zclGp_AppCallbacks_t 
 * zclHVAC_AppCallbacks_t 
 * zclLighting_AppCallbacks_t 
 * zclMS_AppCallbacks_t 
 * zclPollControl_AppCallbacks_t 
 * zclPowerProfile_AppCallbacks_t 
 * zclSS_AppCallbacks_t  
 *
 */

// Initialization function for the zclGeneral layer.
void zclSmartLight_Init( byte task_id ) {
  zclSmartLight_TaskID = task_id;

  // This app is part of the Home Automation Profile
  bdb_RegisterSimpleDescriptor( &zclSmartLight_SimpleDesc );

  // Register the ZCL General Cluster Library callback functions
  zclGeneral_RegisterCmdCallbacks( SMARTLIGHT_ENDPOINT, &zclSmartLight_CmdCallbacks );
  
  /* SMARTLIGHT_TODO: Register other cluster command callbacks here */

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

  if ( zclDiagnostic_InitStats() == ZSuccess ) {
    // Here the user could start the timer to save Diagnostics to NV
  }
#endif

  hal_i2c_init();
  hal_wsled_init();
  hal_hdc1080_init(Temperature_Resolution_14_bit, Humidity_Resolution_14_bit);
  hal_bh1750_init_ex(BH1750FVI_MODE_HIGH_RESOLUTION_MODE, BH1750FVI_DEFAULT_MEASUREMENT_TIME);

  // If it is not on a network, blink the LED
  if (!bdbAttributes.bdbNodeIsOnANetwork) {
    osal_start_reload_timer(zclSmartLight_TaskID, SMARTLIGHT_LED_BLINK_EVT, 1000); // 1 second
  }

  // Start BDB Commissioning
  bdb_StartCommissioning(BDB_COMMISSIONING_MODE_NWK_STEERING);

  // Start measurement task for reporting of values
  osal_start_reload_timer(zclSmartLight_TaskID, SMARTLIGHT_REPORTING__EVT, 10000); // 10 seconds for debug
}

// Event Loop Processor for zclGeneral.
uint16 zclSmartLight_event_loop( byte task_id, uint16 events ) {
  afIncomingMSGPacket_t *MSGpkt;

  (void)task_id; // Intentionally unreferenced parameter

  if ( events & SYS_EVENT_MSG ) {
    while ( (MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( zclSmartLight_TaskID )) ) {
      switch ( MSGpkt->hdr.event ) {
        case ZCL_INCOMING_MSG:
          // Incoming ZCL Foundation command/response messages
          zclSmartLight_ProcessIncomingMsg( (zclIncomingMsg_t *)MSGpkt );
          break;
        case ZDO_STATE_CHANGE:
          zclSmartLight_NwkState = (devStates_t)(MSGpkt->hdr.status);
          // If the device is leaving the network, start commission process
          if (zclSmartLight_NwkState == DEV_NWK_ORPHAN) {
            zclSmartLight_ResetAttributesToDefaultValues();
            osal_start_reload_timer(zclSmartLight_TaskID, SMARTLIGHT_LED_BLINK_EVT, 1000);
            bdb_StartCommissioning(BDB_COMMISSIONING_MODE_NWK_STEERING);
          }
          break;
        default:
          break;
      }
      osal_msg_deallocate( (uint8 *)MSGpkt ); // Release the memory
    }
    return (events ^ SYS_EVENT_MSG); // return unprocessed events
  }

#if ZG_BUILD_ENDDEVICE_TYPE
  if ( events & SMARTLIGHT_END_DEVICE_REJOIN_EVT ) {
    bdb_ZedAttemptRecoverNwk();
    return ( events ^ SMARTLIGHT_END_DEVICE_REJOIN_EVT );
  }
#endif

  // Led blink event
  if (events & SMARTLIGHT_LED_BLINK_EVT) {
    if (zclSmartLight_OnOff == LIGHT_ON) {
      hal_wsled_setRgb(0, 0, 0);
      zclSmartLight_OnOff = LIGHT_OFF;
    }
    else {
      hal_wsled_setRgb(255, 255, 255);
      zclSmartLight_OnOff = LIGHT_ON;
    }
    return (events ^ SMARTLIGHT_LED_BLINK_EVT);
  }

  // Attributes reporting event
  if (events & SMARTLIGHT_REPORTING__EVT) {
    hal_hdc1080_measurement(&zclSmartLight_Temperature, &zclSmartLight_Humidity);
    hal_bh1750_measurement(&zclSmartLight_Illuminance);

    zclSmartLight_Reporting();

    return (events ^ SMARTLIGHT_REPORTING__EVT);
  }

  // Discard unknown events
  return 0;
}

// Called to handle battery-low situation.
void zclSampleApp_BatteryWarningCB(uint8 voltLevel) {
  if (voltLevel == VOLT_LEVEL_CAUTIOUS) {
    // Send warning message to the gateway and blink LED
  }
  else if (voltLevel == VOLT_LEVEL_BAD) {
    // Shut down the system
  }
}

// Called to process any change to the IdentifyTime attribute.
static void zclSmartLight_ProcessIdentifyTimeChange( uint8 endpoint ) {
  (void) endpoint;
  if ( zclSmartLight_IdentifyTime > 0 ) {}
}

// Called when a new bind is added.
static void zclSmartLight_BindNotification( bdbBindNotificationData_t *data ) {
  // SMARTLIGHT_TODO: process the new bind information
}

#if ( defined ( BDB_TL_TARGET ) && (BDB_TOUCHLINK_CAPABILITY_ENABLED == TRUE) )
// Called to process when the touchlink target functionality is enabled or disabled
static void zclSmartLight_ProcessTouchlinkTargetEnable( uint8 enable ) {
  if ( enable ) HalLedSet ( HAL_LED_1, HAL_LED_MODE_ON );
  else HalLedSet ( HAL_LED_1, HAL_LED_MODE_OFF );
}
#endif

void zclSmartLight_Reporting(void) {
  static uint16 zclSmartLight_SeqNum = 0;
  afAddrType_t zclSmartLight_DstAddr;
  zclSmartLight_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
  zclSmartLight_DstAddr.addr.shortAddr = 0;
  zclSmartLight_DstAddr.endPoint = 1;

  const uint8 pReportCmd_NUM = 1;
  zclReportCmd_t* pReportCmd;
  pReportCmd = osal_mem_alloc(sizeof(zclReportCmd_t) + (pReportCmd_NUM * sizeof(zclReport_t)));
  if (pReportCmd != NULL) {
    pReportCmd->numAttr = pReportCmd_NUM;
    pReportCmd->attrList[0].attrID = ATTRID_ON_OFF;
    pReportCmd->attrList[0].dataType = ZCL_DATATYPE_BOOLEAN;
    pReportCmd->attrList[0].attrData = (void*)(&zclSmartLight_OnOff);
    zcl_SendReportCmd(SMARTLIGHT_ENDPOINT, &zclSmartLight_DstAddr, ZCL_CLUSTER_ID_GEN_ON_OFF, pReportCmd, ZCL_FRAME_CLIENT_SERVER_DIR, false, zclSmartLight_SeqNum++);

    pReportCmd->attrList[0].attrID = ATTRID_MS_ILLUMINANCE_MEASURED_VALUE;
    pReportCmd->attrList[0].dataType = ZCL_DATATYPE_UINT16;
    pReportCmd->attrList[0].attrData = (void*)(&zclSmartLight_Illuminance);
    zcl_SendReportCmd(SMARTLIGHT_ENDPOINT, &zclSmartLight_DstAddr, ZCL_CLUSTER_ID_MS_ILLUMINANCE_MEASUREMENT, pReportCmd, ZCL_FRAME_CLIENT_SERVER_DIR, false, zclSmartLight_SeqNum++);

    pReportCmd->attrList[0].attrID = ATTRID_MS_TEMPERATURE_MEASURED_VALUE;
    pReportCmd->attrList[0].dataType = ZCL_DATATYPE_UINT16;
    pReportCmd->attrList[0].attrData = (void*)(&zclSmartLight_Temperature);
    zcl_SendReportCmd(SMARTLIGHT_ENDPOINT, &zclSmartLight_DstAddr, ZCL_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT, pReportCmd, ZCL_FRAME_CLIENT_SERVER_DIR, false, zclSmartLight_SeqNum++);

    pReportCmd->attrList[0].attrID = ATTRID_MS_RELATIVE_HUMIDITY_MEASURED_VALUE;
    pReportCmd->attrList[0].dataType = ZCL_DATATYPE_UINT16;
    pReportCmd->attrList[0].attrData = (void*)(&zclSmartLight_Humidity);
    zcl_SendReportCmd(SMARTLIGHT_ENDPOINT, &zclSmartLight_DstAddr, ZCL_CLUSTER_ID_MS_RELATIVE_HUMIDITY, pReportCmd, ZCL_FRAME_CLIENT_SERVER_DIR, false, zclSmartLight_SeqNum++);
  }
  osal_mem_free(pReportCmd);
}

// Callback in which the status of the commissioning process are reported
static void zclSmartLight_ProcessCommissioningStatus(bdbCommissioningModeMsg_t* bdbCommissioningModeMsg) {
  switch (bdbCommissioningModeMsg->bdbCommissioningMode) {
  case BDB_COMMISSIONING_FORMATION:
    if (bdbCommissioningModeMsg->bdbCommissioningStatus == BDB_COMMISSIONING_SUCCESS) {
      //After formation, perform nwk steering again plus the remaining commissioning modes that has not been process yet
      bdb_StartCommissioning(BDB_COMMISSIONING_MODE_NWK_STEERING | bdbCommissioningModeMsg->bdbRemainingCommissioningModes);
    }
    else {
      //Want to try other channels?
      //try with bdb_setChannelAttribute
    }
    break;
  case BDB_COMMISSIONING_NWK_STEERING:
    if (bdbCommissioningModeMsg->bdbCommissioningStatus == BDB_COMMISSIONING_SUCCESS) {
      // End of LED blinking
      osal_stop_timerEx(zclSmartLight_TaskID, SMARTLIGHT_LED_BLINK_EVT);
    }
    else {
      // Try again
      bdb_StartCommissioning(BDB_COMMISSIONING_MODE_NWK_STEERING);
    }
    break;
  case BDB_COMMISSIONING_FINDING_BINDING:
    if (bdbCommissioningModeMsg->bdbCommissioningStatus == BDB_COMMISSIONING_SUCCESS) {
      //YOUR JOB:
    }
    else {
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
    if (bdbCommissioningModeMsg->bdbCommissioningStatus == BDB_COMMISSIONING_NETWORK_RESTORED) {
      //We did recover from losing parent
    }
    else {
      //Parent not found, attempt to rejoin again after a fixed delay
      osal_start_timerEx(zclSmartLight_TaskID, SMARTLIGHT_END_DEVICE_REJOIN_EVT, SMARTLIGHT_END_DEVICE_REJOIN_DELAY);
    }
    break;
#endif
    }
}

// Functions for processing ZCL Foundation incoming Command/Response messages
// Process ZCL Foundation incoming message
static void zclSmartLight_ProcessIncomingMsg( zclIncomingMsg_t *pInMsg ) {
  switch ( pInMsg->zclHdr.commandID ) {
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

  if ( pInMsg->attrCmd ) osal_mem_free( pInMsg->attrCmd );
}

// Process the "Profile" Default Response Command
static uint8 zclSmartLight_ProcessInDefaultRspCmd(zclIncomingMsg_t* pInMsg) {
  // zclDefaultRspCmd_t *defaultRspCmd = (zclDefaultRspCmd_t *)pInMsg->attrCmd;

  // Device is notified of the Default Response command.
  (void)pInMsg;
  return (TRUE);
}

#ifdef ZCL_READ
// Process the "Profile" Read Response Command
static uint8 zclSmartLight_ProcessInReadRspCmd( zclIncomingMsg_t *pInMsg ) {
  zclReadRspCmd_t *readRspCmd = (zclReadRspCmd_t *)pInMsg->attrCmd;
  for (uint8 i = 0; i < readRspCmd->numAttr; i++) {
    // Notify the originator of the results of the original read attributes
    // attempt and, for each successfull request, the value of the requested
    // attribute
  }
  return ( TRUE );
}
#endif // ZCL_READ

#ifdef ZCL_WRITE
// Process the "Profile" Write Response Command
static uint8 zclSmartLight_ProcessInWriteRspCmd( zclIncomingMsg_t *pInMsg ) {
  zclWriteRspCmd_t *writeRspCmd = (zclWriteRspCmd_t *)pInMsg->attrCmd;
  for (uint8 i = 0; i < writeRspCmd->numAttr; i++) {
    // Notify the device of the results of the its original write attributes
    // command.
  }
  return ( TRUE );
}
#endif // ZCL_WRITE

#ifdef ZCL_DISCOVER
// Process the Discover Commands Response Command
static uint8 zclSmartLight_ProcessInDiscCmdsRspCmd( zclIncomingMsg_t *pInMsg ) {
  zclDiscoverCmdsCmdRsp_t *discoverRspCmd = (zclDiscoverCmdsCmdRsp_t *)pInMsg->attrCmd;
  for (uint8 i = 0; i < discoverRspCmd->numCmd; i++) {
    // Device is notified of the result of its attribute discovery command.
  }
  return ( TRUE );
}

// Process the "Profile" Discover Attributes Response Command
static uint8 zclSmartLight_ProcessInDiscAttrsRspCmd( zclIncomingMsg_t *pInMsg ) {
  zclDiscoverAttrsRspCmd_t *discoverRspCmd = (zclDiscoverAttrsRspCmd_t *)pInMsg->attrCmd;
  for (uint8 i = 0; i < discoverRspCmd->numAttr; i++) {
    // Device is notified of the result of its attribute discovery command.
  }
  return ( TRUE );
}

// Process the "Profile" Discover Attributes Extended Response Command
static uint8 zclSmartLight_ProcessInDiscAttrsExtRspCmd( zclIncomingMsg_t *pInMsg ) {
  zclDiscoverAttrsExtRsp_t *discoverRspCmd = (zclDiscoverAttrsExtRsp_t *)pInMsg->attrCmd;
  for (uint8 i = 0; i < discoverRspCmd->numAttr; i++) {
    // Device is notified of the result of its attribute discovery command.
  }
  return ( TRUE );
}
#endif // ZCL_DISCOVER
