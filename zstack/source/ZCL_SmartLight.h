#ifndef ZCL_SMARTLIGHT_H
#define ZCL_SMARTLIGHT_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "zcl.h"


// Added to include ZLL Target functionality
#if defined ( BDB_TL_INITIATOR ) || defined ( BDB_TL_TARGET )
  #include "zcl_general.h"
  #include "bdb_tlCommissioning.h"
#endif

/*********************************************************************
 * CONSTANTS
 */
#define SMARTLIGHT_ENDPOINT            8
// Added to include ZLL Target functionality
#define SMARTLIGHT_NUM_GRPS            2


// Application Events
#define SMARTLIGHT_MAIN_SCREEN_EVT          0x0001
#define SMARTLIGHT_LEVEL_CTRL_EVT           0x0002
#define SMARTLIGHT_END_DEVICE_REJOIN_EVT    0x0004  
  
/* SMARTLIGHT_TODO: define app events here */
  
#define SMARTLIGHT_EVT_1                    0x0008
/*
#define SMARTLIGHT_EVT_2                    0x0010
#define SMARTLIGHT_EVT_3                    0x0020
*/

// Application Display Modes
#define SMARTLIGHT_MAINMODE      0x00
#define SMARTLIGHT_HELPMODE      0x01
  
#define SMARTLIGHT_END_DEVICE_REJOIN_DELAY 10000

/*********************************************************************
 * MACROS
 */
/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * VARIABLES
 */

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


/*********************************************************************
 * FUNCTIONS
 */

 /*
  * Initialization for the task
  */
extern void zclSmartLight_Init( byte task_id );

/*
 *  Event Process for the task
 */
extern UINT16 zclSmartLight_event_loop( byte task_id, UINT16 events );

/*
 *  Reset all writable attributes to their default values.
 */
extern void zclSmartLight_ResetAttributesToDefaultValues(void);


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* ZCL_SMARTLIGHT_H */
