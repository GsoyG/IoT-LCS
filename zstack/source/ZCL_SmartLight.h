#ifndef ZCL_SMARTLIGHT_H
#define ZCL_SMARTLIGHT_H

#include "zcl.h"

// Added to include ZLL Target functionality
#if defined ( BDB_TL_INITIATOR ) || defined ( BDB_TL_TARGET )
  #include "zcl_general.h"
  #include "bdb_tlCommissioning.h"
#endif

// Added to include ZLL Target functionality
#define SMARTLIGHT_NUM_GRPS 2
#define SMARTLIGHT_ENDPOINT 8

// Application Events
#define SMARTLIGHT_MAIN_SCREEN_EVT       0x0001
#define SMARTLIGHT_LEVEL_CTRL_EVT        0x0002
#define SMARTLIGHT_END_DEVICE_REJOIN_EVT 0x0004

#define SMARTLIGHT_END_DEVICE_REJOIN_DELAY 10000

// Added to include ZLL Target functionality
#if defined ( BDB_TL_INITIATOR ) || defined ( BDB_TL_TARGET )
  extern bdbTLDeviceInfo_t tlSmartLight_DeviceInfo;
#endif

extern SimpleDescriptionFormat_t zclSmartLight_SimpleDesc;

extern CONST zclCommandRec_t zclSmartLight_Cmds[];

extern CONST uint8 zclCmdsArraySize;

// attribute list
extern CONST zclAttrRec_t zclSmartLight_Attrs[];
extern CONST uint8 zclSmartLight_NumAttributes;

// Identify attributes
extern uint16 zclSmartLight_IdentifyTime;
extern uint8  zclSmartLight_IdentifyCommissionState;

// SMARTLIGHT_TODO: Declare application specific attributes here
// Initialization for the task
extern void zclSmartLight_Init( byte task_id );

//Event Process for the task
extern uint16 zclSmartLight_event_loop( byte task_id, uint16 events );

// Reset all writable attributes to their default values
extern void zclSmartLight_ResetAttributesToDefaultValues(void);

#endif // ZCL_SMARTLIGHT_H
