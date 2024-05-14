#include "ZComDef.h"
#include "OSAL.h"
#include "AF.h"
#include "ZDConfig.h"

#include "zcl.h"
#include "zcl_ha.h"
#include "zcl_ms.h"
#include "zcl_general.h"
#include "zcl_lighting.h"

#include "ZCL_SmartLight.h"
#include "ZCL_SmartLight_Callbacks.h"

#define SMARTLIGHT_DEVICE_VERSION     0
#define SMARTLIGHT_FLAGS              0

#define SMARTLIGHT_HWVERSION          1
#define SMARTLIGHT_ZCLVERSION         1

// Global attributes
const uint16 zclSmartLight_clusterRevision_all = 0x0001; 

// Basic Cluster
const uint8 zclSmartLight_HWRevision = SMARTLIGHT_HWVERSION;
const uint8 zclSmartLight_ZCLVersion = SMARTLIGHT_ZCLVERSION;
const uint8 zclSmartLight_ManufacturerName[] = { 4, 'G','s','o','y' };
const uint8 zclSmartLight_ModelId[] = { 6, 'S','L','0','0','0','1' };
const uint8 zclSmartLight_DateCode[] = { 16, '2','0','2','4','0','4','0','1',' ',' ',' ',' ',' ',' ',' ',' ' };
const uint8 zclSmartLight_PowerSource = POWER_SOURCE_MAINS_1_PHASE;

uint8 zclSmartLight_LocationDescription[17] = { 16, ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ' };
uint8 zclSmartLight_PhysicalEnvironment = 0;
uint8 zclSmartLight_DeviceEnable = DEVICE_ENABLED;

// Identify Cluster
uint16 zclSmartLight_IdentifyTime;

// On/Off Cluster
uint8 zclSmartLight_OnOff;

// Level Control Cluster
uint8 zclSmartLight_CurrentLevel;

// Lighting Color Control Cluster
uint8 zclSmartLight_CurrentHue;
uint8 zclSmartLight_CurrentSaturation;
uint16 zclSmartLight_ColorTemperature;
uint8 zclSmartLight_ColorMode;

// Illuminance Measurement Cluster
uint16 zclSmartLight_Illuminance;

// Temperature Measurement Cluster
uint16 zclSmartLight_Temperature;

// Relative Humidity Cluster
uint16 zclSmartLight_Humidity;

/* SMARTLIGHT_TODO: declare attribute variables here. If its value can change,
 * initialize it in zclSmartLight_ResetAttributesToDefaultValues. If its
 * value will not change, initialize it here.
 */

#if ZCL_DISCOVER
CONST zclCommandRec_t zclSmartLight_Cmds[] = {
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    COMMAND_BASIC_RESET_FACT_DEFAULT,
    CMD_DIR_SERVER_RECEIVED
  },
};

CONST uint8 zclCmdsArraySize = ( sizeof(zclSmartLight_Cmds) / sizeof(zclSmartLight_Cmds[0]) );
#endif // ZCL_DISCOVER

// ATTRIBUTE DEFINITIONS - Uses REAL cluster IDs
CONST zclAttrRec_t zclSmartLight_Attrs[] = {
  // General Basic Cluster Attributes
  {
    ZCL_CLUSTER_ID_GEN_BASIC,           // Cluster IDs - defined in the foundation (ie. zcl.h)
    {
      ATTRID_BASIC_HW_VERSION,          // Attribute ID - Found in Cluster Library header (ie. zcl_general.h)
      ZCL_DATATYPE_UINT8,               // Data Type - found in zcl.h
      ACCESS_CONTROL_READ,              // Variable access control - found in zcl.h
      (void *)&zclSmartLight_HWRevision // Pointer to attribute variable
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    {
      ATTRID_BASIC_ZCL_VERSION,
      ZCL_DATATYPE_UINT8,
      ACCESS_CONTROL_READ,
      (void *)&zclSmartLight_ZCLVersion
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    {
      ATTRID_BASIC_MANUFACTURER_NAME,
      ZCL_DATATYPE_CHAR_STR,
      ACCESS_CONTROL_READ,
      (void *)zclSmartLight_ManufacturerName
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    {
      ATTRID_BASIC_MODEL_ID,
      ZCL_DATATYPE_CHAR_STR,
      ACCESS_CONTROL_READ,
      (void *)zclSmartLight_ModelId
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    {
      ATTRID_BASIC_DATE_CODE,
      ZCL_DATATYPE_CHAR_STR,
      ACCESS_CONTROL_READ,
      (void *)zclSmartLight_DateCode
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    {
      ATTRID_BASIC_POWER_SOURCE,
      ZCL_DATATYPE_ENUM8,
      ACCESS_CONTROL_READ,
      (void *)&zclSmartLight_PowerSource
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    {
      ATTRID_BASIC_LOCATION_DESC,
      ZCL_DATATYPE_CHAR_STR,
      (ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE),
      (void *)zclSmartLight_LocationDescription
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    {
      ATTRID_BASIC_PHYSICAL_ENV,
      ZCL_DATATYPE_ENUM8,
      (ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE),
      (void *)&zclSmartLight_PhysicalEnvironment
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    {
      ATTRID_BASIC_DEVICE_ENABLED,
      ZCL_DATATYPE_BOOLEAN,
      (ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE),
      (void *)&zclSmartLight_DeviceEnable
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_BASIC,
    {
      ATTRID_CLUSTER_REVISION,
      ZCL_DATATYPE_UINT16,
      ACCESS_CONTROL_READ,
      (void*)&zclSmartLight_clusterRevision_all
    }
  },
#ifdef ZCL_IDENTIFY
  // *** Identify Cluster Attribute ***
  {
    ZCL_CLUSTER_ID_GEN_IDENTIFY,
    {
      ATTRID_IDENTIFY_TIME,
      ZCL_DATATYPE_UINT16,
      (ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE),
      (void *)&zclSmartLight_IdentifyTime
    }
  },
#endif
  {
    ZCL_CLUSTER_ID_GEN_IDENTIFY,
    {
      ATTRID_CLUSTER_REVISION,
      ZCL_DATATYPE_UINT16,
      ACCESS_CONTROL_READ,
      (void *)&zclSmartLight_clusterRevision_all
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_ON_OFF,
    {
      ATTRID_ON_OFF,
      ZCL_DATATYPE_BOOLEAN,
      ACCESS_CONTROL_READ | ACCESS_REPORTABLE,
      (void*)&zclSmartLight_OnOff
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_ON_OFF,
    {
      ATTRID_CLUSTER_REVISION,
      ZCL_DATATYPE_UINT16,
      ACCESS_CONTROL_READ,
      (void*)&zclSmartLight_clusterRevision_all
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_LEVEL_CONTROL,
    {
      ATTRID_LEVEL_CURRENT_LEVEL,
      ZCL_DATATYPE_UINT8,
      ACCESS_CONTROL_READ | ACCESS_REPORTABLE,
      (void*)&zclSmartLight_CurrentLevel
    }
  },
  {
    ZCL_CLUSTER_ID_GEN_LEVEL_CONTROL,
    {
      ATTRID_CLUSTER_REVISION,
      ZCL_DATATYPE_UINT16,
      ACCESS_CONTROL_READ,
      (void*)&zclSmartLight_clusterRevision_all
    }
  },
  {
    ZCL_CLUSTER_ID_LIGHTING_COLOR_CONTROL,
    {
      ATTRID_LIGHTING_COLOR_CONTROL_CURRENT_HUE,
      ZCL_DATATYPE_UINT8,
      ACCESS_CONTROL_READ | ACCESS_REPORTABLE,
      (void*)&zclSmartLight_CurrentHue
    }
  },
  {
    ZCL_CLUSTER_ID_LIGHTING_COLOR_CONTROL,
    {
      ATTRID_LIGHTING_COLOR_CONTROL_CURRENT_SATURATION,
      ZCL_DATATYPE_UINT8,
      ACCESS_CONTROL_READ | ACCESS_REPORTABLE,
      (void*)&zclSmartLight_CurrentSaturation
    }
  },
  {
    ZCL_CLUSTER_ID_LIGHTING_COLOR_CONTROL,
    {
      ATTRID_LIGHTING_COLOR_CONTROL_COLOR_TEMPERATURE,
      ZCL_DATATYPE_UINT16,
      ACCESS_CONTROL_READ | ACCESS_REPORTABLE,
      (void*)&zclSmartLight_ColorTemperature
    }
  },
  {
    ZCL_CLUSTER_ID_LIGHTING_COLOR_CONTROL,
    {
      ATTRID_LIGHTING_COLOR_CONTROL_COLOR_MODE,
      ZCL_DATATYPE_ENUM8,
      ACCESS_CONTROL_READ,
      (void*)&zclSmartLight_ColorMode
    }
  },
  {
    ZCL_CLUSTER_ID_LIGHTING_COLOR_CONTROL,
    {
      ATTRID_CLUSTER_REVISION,
      ZCL_DATATYPE_UINT16,
      ACCESS_CONTROL_READ,
      (void*)&zclSmartLight_clusterRevision_all
    }
  },
  {
    ZCL_CLUSTER_ID_MS_ILLUMINANCE_MEASUREMENT,
    {
      ATTRID_MS_ILLUMINANCE_MEASURED_VALUE,
      ZCL_DATATYPE_UINT16,
      ACCESS_CONTROL_READ,
      (void*)&zclSmartLight_Illuminance
    }
  },
  {
    ZCL_CLUSTER_ID_MS_ILLUMINANCE_MEASUREMENT,
    {
      ATTRID_CLUSTER_REVISION,
      ZCL_DATATYPE_UINT16,
      ACCESS_CONTROL_READ,
      (void*)&zclSmartLight_clusterRevision_all
    }
  },
  {
    ZCL_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT,
    {
      ATTRID_MS_TEMPERATURE_MEASURED_VALUE,
      ZCL_DATATYPE_UINT16,
      ACCESS_CONTROL_READ | ACCESS_REPORTABLE,
      (void*)&zclSmartLight_Temperature
    }
  },
  {
    ZCL_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT,
    {
      ATTRID_CLUSTER_REVISION,
      ZCL_DATATYPE_UINT16,
      ACCESS_CONTROL_READ,
      (void*)&zclSmartLight_clusterRevision_all
    }
  },
  {
    ZCL_CLUSTER_ID_MS_RELATIVE_HUMIDITY,
    {
      ATTRID_MS_RELATIVE_HUMIDITY_MEASURED_VALUE,
      ZCL_DATATYPE_UINT16,
      ACCESS_CONTROL_READ | ACCESS_REPORTABLE,
      (void*)&zclSmartLight_Humidity
    }
  },
  {
    ZCL_CLUSTER_ID_MS_RELATIVE_HUMIDITY,
    {
      ATTRID_CLUSTER_REVISION,
      ZCL_DATATYPE_UINT16,
      ACCESS_CONTROL_READ,
      (void*)&zclSmartLight_clusterRevision_all
    }
  }
};

uint8 CONST zclSmartLight_NumAttributes = ( sizeof(zclSmartLight_Attrs) / sizeof(zclSmartLight_Attrs[0]) );

// This is the Cluster ID List and should be filled with Application specific cluster IDs.
const cId_t zclSmartLight_InClusterList[] = {
  ZCL_CLUSTER_ID_GEN_BASIC,
  ZCL_CLUSTER_ID_GEN_IDENTIFY,
  ZCL_CLUSTER_ID_GEN_ON_OFF,
  ZCL_CLUSTER_ID_GEN_LEVEL_CONTROL,
  ZCL_CLUSTER_ID_LIGHTING_COLOR_CONTROL,
  ZCL_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT,
  ZCL_CLUSTER_ID_MS_RELATIVE_HUMIDITY
  // SMARTLIGHT_TODO: Add application specific Input Clusters Here. 
  // See zcl.h for Cluster ID definitions
};
#define ZCLSMARTLIGHT_MAX_INCLUSTERS   (sizeof(zclSmartLight_InClusterList) / sizeof(zclSmartLight_InClusterList[0]))

const cId_t zclSmartLight_OutClusterList[] = {
  ZCL_CLUSTER_ID_GEN_BASIC
  // SMARTLIGHT_TODO: Add application specific Output Clusters Here. 
  // See zcl.h for Cluster ID definitions
};
#define ZCLSMARTLIGHT_MAX_OUTCLUSTERS  (sizeof(zclSmartLight_OutClusterList) / sizeof(zclSmartLight_OutClusterList[0]))

SimpleDescriptionFormat_t zclSmartLight_SimpleDesc = {
  SMARTLIGHT_ENDPOINT,                    //  int Endpoint;
  ZCL_HA_PROFILE_ID,                      //  uint16 AppProfId;
  // SMARTLIGHT_TODO: Replace ZCL_HA_DEVICEID_ON_OFF_LIGHT with application specific device ID
  ZCL_HA_DEVICEID_COLORED_DIMMABLE_LIGHT, //  uint16 AppDeviceId; 
  SMARTLIGHT_DEVICE_VERSION,              //  int   AppDevVer:4;
  SMARTLIGHT_FLAGS,                       //  int   AppFlags:4;
  ZCLSMARTLIGHT_MAX_INCLUSTERS,           //  byte  AppNumInClusters;
  (cId_t *)zclSmartLight_InClusterList,   //  byte *pAppInClusterList;
  ZCLSMARTLIGHT_MAX_OUTCLUSTERS,          //  byte  AppNumInClusters;
  (cId_t *)zclSmartLight_OutClusterList   //  byte *pAppInClusterList;
};

// Added to include ZLL Target functionality
#if defined ( BDB_TL_INITIATOR ) || defined ( BDB_TL_TARGET )
bdbTLDeviceInfo_t tlSmartLight_DeviceInfo = {
  SMARTLIGHT_ENDPOINT,          //uint8 endpoint;
  ZCL_HA_PROFILE_ID,            //uint16 profileID;
  // SMARTLIGHT_TODO: Replace ZCL_HA_DEVICEID_ON_OFF_LIGHT with application specific device ID
  ZCL_HA_DEVICEID_ON_OFF_LIGHT, //uint16 deviceID;
  SMARTLIGHT_DEVICE_VERSION,    //uint8 version;
  SMARTLIGHT_NUM_GRPS           //uint8 grpIdCnt;
};
#endif

void zclSmartLight_ResetAttributesToDefaultValues(void) {
  zclSmartLight_LocationDescription[0] = 16;
  for (int i = 1; i <= 16; i++) {
    zclSmartLight_LocationDescription[i] = ' ';
  }
  
  zclSmartLight_PhysicalEnvironment = PHY_UNSPECIFIED_ENV;
  zclSmartLight_DeviceEnable = DEVICE_ENABLED;
  
#ifdef ZCL_IDENTIFY
  zclSmartLight_IdentifyTime = 0;
#endif

  zclSmartLight_OnOff = LIGHT_OFF;
  zclSmartLight_CurrentLevel = 254;

  zclSmartLight_CurrentHue = 0;
  zclSmartLight_CurrentSaturation = 254;
  zclSmartLight_ColorTemperature = 0;
  zclSmartLight_ColorMode = COLOR_MODE_COLOR_TEMPERATURE;

  zclSmartLight_Illuminance = 0;
  zclSmartLight_Temperature = 0;
  zclSmartLight_Humidity = 0;

  /* SMARTLIGHT_TODO: initialize cluster attribute variables. */
}
