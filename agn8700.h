/***************************************************************************** 
 *     2007-2010, National Instruments, Corporation.  All Rights Reserved.    * 
 *****************************************************************************/

/****************************************************************************
 *                                                                          
 * Title:    agn8700.h                                        
 * Purpose:  agn8700                                       
 *           instrument driver declarations.                                
 *                                                                          
 ****************************************************************************/

#ifndef __AGN8700_HEADER
#define __AGN8700_HEADER

#include <ivi.h>
#include <ividcpwr.h>

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

/****************************************************************************
 *----------------- Instrument Driver Revision Information -----------------*
 ****************************************************************************/

#define AGN8700_MAJOR_VERSION                1     /* Instrument driver major version   */
#define AGN8700_MINOR_VERSION                1     /* Instrument driver minor version   */
                                                                
#define AGN8700_CLASS_SPEC_MAJOR_VERSION     3     /* Class specification major version */
#define AGN8700_CLASS_SPEC_MINOR_VERSION     0     /* Class specification minor version */


#define AGN8700_SUPPORTED_INSTRUMENT_MODELS  "N8731A,N8732A,N8733A,N8734A,N8735A,N8736A,N8737A,N8738A,N8739A,N8740A,N8741A,N8742A,\
	                                          N8754A,N8755A,N8756A,N8757A,N8758A,N8759A,N8760A,N8761A,N8762A"
#define AGN8700_DRIVER_VENDOR                "National Instruments"
#ifdef	_IVI_mswin64_
#define AGN8700_DRIVER_DESCRIPTION           "Agilent N8700 System DC Power System [Compiled for 64-bit.]"
#else                    
#define AGN8700_DRIVER_DESCRIPTION           "Agilent N8700 System DC Power System"
#endif                    

/**************************************************************************** 
 *------------------------------ Useful Macros -----------------------------* 
 ****************************************************************************/

    /*- Defined values for rangeType parameter of function -----------------*/
    /*- ConfigureOutputRange -----------------------------------------------*/
#define AGN8700_VAL_RANGE_CURRENT               IVIDCPWR_VAL_RANGE_CURRENT              
#define AGN8700_VAL_RANGE_VOLTAGE               IVIDCPWR_VAL_RANGE_VOLTAGE              
                                                                                            

    /*- Defined values for outputState parameter of function ---------------*/
    /*- QueryOutputState ---------------------------------------------------*/
#define AGN8700_VAL_OUTPUT_CONSTANT_VOLTAGE     IVIDCPWR_VAL_OUTPUT_CONSTANT_VOLTAGE
#define AGN8700_VAL_OUTPUT_CONSTANT_CURRENT     IVIDCPWR_VAL_OUTPUT_CONSTANT_CURRENT
#define AGN8700_VAL_OUTPUT_OVER_VOLTAGE         IVIDCPWR_VAL_OUTPUT_OVER_VOLTAGE       
#define AGN8700_VAL_OUTPUT_OVER_CURRENT         IVIDCPWR_VAL_OUTPUT_OVER_CURRENT       
#define AGN8700_VAL_OUTPUT_UNREGULATED          IVIDCPWR_VAL_OUTPUT_UNREGULATED    
#define AGN8700_VAL_OUTPUT_OVER_TEMPERATURE     (IVIDCPWR_VAL_OUTPUT_STATE_SPECIFIC_EXT_BASE + 1L)
#define AGN8700_VAL_WAIT_TRANS_TRIG             (IVIDCPWR_VAL_OUTPUT_STATE_SPECIFIC_EXT_BASE + 2L) 
#define AGN8700_VAL_INHIBIT                     (IVIDCPWR_VAL_OUTPUT_STATE_SPECIFIC_EXT_BASE + 3L) 
#define AGN8700_VAL_POWER_FAILED                (IVIDCPWR_VAL_OUTPUT_STATE_SPECIFIC_EXT_BASE + 4L) 

    /*- Defined values for measurementType parameter of function -----------*/
    /*- Measure ------------------------------------------------------------*/
#define AGN8700_VAL_MEASURE_CURRENT             IVIDCPWR_VAL_MEASURE_CURRENT
#define AGN8700_VAL_MEASURE_VOLTAGE             IVIDCPWR_VAL_MEASURE_VOLTAGE


/**************************************************************************** 
 *---------------------------- Attribute Defines ---------------------------* 
 ****************************************************************************/

    /*- IVI Inherent Instrument Attributes ---------------------------------*/    

        /* User Options */
#define AGN8700_ATTR_RANGE_CHECK                   IVI_ATTR_RANGE_CHECK                    /* ViBoolean */
#define AGN8700_ATTR_QUERY_INSTRUMENT_STATUS       IVI_ATTR_QUERY_INSTRUMENT_STATUS        /* ViBoolean */
#define AGN8700_ATTR_CACHE                         IVI_ATTR_CACHE                          /* ViBoolean */
#define AGN8700_ATTR_SIMULATE                      IVI_ATTR_SIMULATE                       /* ViBoolean */
#define AGN8700_ATTR_RECORD_COERCIONS              IVI_ATTR_RECORD_COERCIONS               /* ViBoolean */
#define AGN8700_ATTR_INTERCHANGE_CHECK             IVI_ATTR_INTERCHANGE_CHECK              /* ViBoolean */
        
        /* Driver Information  */
#define AGN8700_ATTR_SPECIFIC_DRIVER_PREFIX        IVI_ATTR_SPECIFIC_DRIVER_PREFIX         /* ViString, read-only  */
#define AGN8700_ATTR_SUPPORTED_INSTRUMENT_MODELS   IVI_ATTR_SUPPORTED_INSTRUMENT_MODELS    /* ViString, read-only  */
#define AGN8700_ATTR_GROUP_CAPABILITIES            IVI_ATTR_GROUP_CAPABILITIES             /* ViString, read-only  */
#define AGN8700_ATTR_INSTRUMENT_MANUFACTURER       IVI_ATTR_INSTRUMENT_MANUFACTURER        /* ViString, read-only  */
#define AGN8700_ATTR_INSTRUMENT_MODEL              IVI_ATTR_INSTRUMENT_MODEL               /* ViString, read-only  */
#define AGN8700_ATTR_INSTRUMENT_FIRMWARE_REVISION  IVI_ATTR_INSTRUMENT_FIRMWARE_REVISION              /* ViString, read-only  */
#define AGN8700_ATTR_SPECIFIC_DRIVER_REVISION      IVI_ATTR_SPECIFIC_DRIVER_REVISION       /* ViString, read-only  */
#define AGN8700_ATTR_SPECIFIC_DRIVER_VENDOR        IVI_ATTR_SPECIFIC_DRIVER_VENDOR         /* ViString, read-only  */
#define AGN8700_ATTR_SPECIFIC_DRIVER_DESCRIPTION   IVI_ATTR_SPECIFIC_DRIVER_DESCRIPTION    /* ViString, read-only  */
#define AGN8700_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION IVI_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION /* ViInt32, read-only */
#define AGN8700_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION IVI_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION /* ViInt32, read-only */

        /* Advanced Session Information */
#define AGN8700_ATTR_LOGICAL_NAME                  IVI_ATTR_LOGICAL_NAME                   /* ViString, read-only  */
#define AGN8700_ATTR_IO_RESOURCE_DESCRIPTOR        IVI_ATTR_IO_RESOURCE_DESCRIPTOR         /* ViString, read-only  */
#define AGN8700_ATTR_DRIVER_SETUP                  IVI_ATTR_DRIVER_SETUP                   /* ViString, read-only  */        
    /*- Instrument-Specific Attributes -------------------------------------*/
    /*- Basic Instrument Operations ----------------------------------------*/
#define AGN8700_ATTR_VOLTAGE_LEVEL             IVIDCPWR_ATTR_VOLTAGE_LEVEL           /* ViReal64,  multi-channel, volts */
#define AGN8700_ATTR_CHANNEL_COUNT             IVI_ATTR_CHANNEL_COUNT                /* ViInt32,  read-only  */
#define AGN8700_ATTR_OVP_ENABLED               IVIDCPWR_ATTR_OVP_ENABLED             /* ViBoolean, multi-channel        */
#define AGN8700_ATTR_OVP_LIMIT                 IVIDCPWR_ATTR_OVP_LIMIT               /* ViReal64,  multi-channel, volts */
#define AGN8700_ATTR_CURRENT_LIMIT_BEHAVIOR    IVIDCPWR_ATTR_CURRENT_LIMIT_BEHAVIOR  /* ViInt32,   multi-channel        */
#define AGN8700_ATTR_CURRENT_LIMIT             IVIDCPWR_ATTR_CURRENT_LIMIT           /* ViReal64,  multi-channel, amps  */
#define AGN8700_ATTR_OUTPUT_ENABLED            IVIDCPWR_ATTR_OUTPUT_ENABLED          /* ViBoolean, multi-channel        */

    /*- Trigger Attributes -------------------------------------------------*/
#define AGN8700_ATTR_TRIGGER_SOURCE            IVIDCPWR_ATTR_TRIGGER_SOURCE          /* ViInt32,   multi-channel        */
#define AGN8700_ATTR_TRIGGERED_CURRENT_LIMIT   IVIDCPWR_ATTR_TRIGGERED_CURRENT_LIMIT /* ViReal64,  multi-channel, amps  */
#define AGN8700_ATTR_TRIGGERED_VOLTAGE_LEVEL   IVIDCPWR_ATTR_TRIGGERED_VOLTAGE_LEVEL /* ViReal64,  multi-channel, volts */

#define AGN8700_ATTR_ID_QUERY_RESPONSE      (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 1L)     /* ViString (Read Only) */
#define AGN8700_ATTR_INITIATE_CONTINUOUS    (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 2L)	 /* ViBoolean, multi-channel        */   
#define AGN8700_ATTR_VOLTAGE_LIMIT_LOW      (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 4L)     /* ViReal64,  multi-channel, volts */     

 


    /*- Obsolete Inherent Instrument Attributes ----------------------------*/
        /* Driver Information */
#define AGN8700_ATTR_SPECIFIC_DRIVER_MAJOR_VERSION IVI_ATTR_SPECIFIC_DRIVER_MAJOR_VERSION  /* ViInt32,  read-only */ 
#define AGN8700_ATTR_SPECIFIC_DRIVER_MINOR_VERSION IVI_ATTR_SPECIFIC_DRIVER_MINOR_VERSION  /* ViInt32,  read-only */ 

        /* Error Info */
#define AGN8700_ATTR_PRIMARY_ERROR                 IVI_ATTR_PRIMARY_ERROR                  /* ViInt32 */
#define AGN8700_ATTR_SECONDARY_ERROR               IVI_ATTR_SECONDARY_ERROR                /* ViInt32 */
#define AGN8700_ATTR_ERROR_ELABORATION             IVI_ATTR_ERROR_ELABORATION              /* ViString */

        /* Advanced Session Information */
#define AGN8700_ATTR_IO_SESSION_TYPE               IVI_ATTR_IO_SESSION_TYPE                /* ViString, read-only */ 

     /* Obsolete Attribute Identifiers for Renamed Attributes */
#define AGN8700_ATTR_NUM_CHANNELS             IVI_ATTR_CHANNEL_COUNT
#define AGN8700_ATTR_QUERY_INSTR_STATUS       AGN8700_ATTR_QUERY_INSTRUMENT_STATUS            
#define AGN8700_ATTR_RESOURCE_DESCRIPTOR      AGN8700_ATTR_IO_RESOURCE_DESCRIPTOR


/**************************************************************************** 
 *------------------------ Attribute Value Defines -------------------------* 
 ****************************************************************************/
    /*- Defined values for attribute AGN8700_ATTR_CURRENT_LIMIT_BEHAVIOR ---*/
#define AGN8700_VAL_CURRENT_REGULATE           IVIDCPWR_VAL_CURRENT_REGULATE
#define AGN8700_VAL_CURRENT_TRIP               IVIDCPWR_VAL_CURRENT_TRIP    

    /*- Defined values for attribute AGN8700_ATTR_TRIGGER_SOURCE -----------*/
#define AGN8700_VAL_SOFTWARE_TRIG              IVIDCPWR_VAL_SOFTWARE_TRIG     


/**************************************************************************** 
 *---------------- Instrument Driver Function Declarations -----------------* 
 ****************************************************************************/
    /*- Init and Close Functions -------------------------------------------*/
ViStatus _VI_FUNC  agn8700_init (ViRsrc resourceName, ViBoolean IDQuery,
                                  ViBoolean resetDevice, ViSession *vi);
ViStatus _VI_FUNC  agn8700_InitWithOptions (ViRsrc resourceName, ViBoolean IDQuery,
                                             ViBoolean resetDevice, ViConstString optionString, 
                                             ViSession *newVi);
ViStatus _VI_FUNC  agn8700_close (ViSession vi);   

    /*- Coercion Info Functions --------------------------------------------*/
ViStatus _VI_FUNC  agn8700_GetNextCoercionRecord (ViSession vi,
                                                   ViInt32 bufferSize,
                                                   ViChar record[]);
    
    /*- Locking Functions --------------------------------------------------*/
ViStatus _VI_FUNC  agn8700_LockSession (ViSession vi, ViBoolean *callerHasLock);   
ViStatus _VI_FUNC  agn8700_UnlockSession (ViSession vi, ViBoolean *callerHasLock);

    /*- Channel Info Functions ---------------------------------------------*/
ViStatus _VI_FUNC agn8700_GetChannelName (ViSession vi, ViInt32 index, 
                                   ViInt32 bufferSize, ViChar name[]);
    
    /*- Basic Instrument Operation -----------------------------------------*/
ViStatus _VI_FUNC agn8700_ConfigureOutputEnabled (ViSession vi,
                                                  ViConstString channelName,
                                                  ViBoolean enabled);

ViStatus _VI_FUNC agn8700_ConfigureOutputRange (ViSession vi,
                                                ViConstString channelName,
                                                ViInt32 rangeType,
                                                ViReal64 range);

ViStatus _VI_FUNC agn8700_ConfigureCurrentLimit (ViSession vi,
                                                 ViConstString channelName,
                                                 ViInt32 behavior,
                                                 ViReal64 limit);
ViStatus _VI_FUNC agn8700_ConfigureOVP (ViSession vi,
                                        ViConstString channelName,
                                        ViBoolean enabled,
                                        ViReal64 limit);

ViStatus _VI_FUNC agn8700_ConfigureVoltageLevel (ViSession vi,
                                                 ViConstString channelName,
                                                 ViReal64 level);

ViStatus _VI_FUNC agn8700_ConfigureVoltageLimitLow (ViSession vi,
                                           ViConstString channelName, ViReal64 limit);

ViStatus _VI_FUNC agn8700_QueryOutputState (ViSession vi,
                                            ViConstString channelName,
                                            ViInt32 outputState,
                                            ViBoolean* inState);

ViStatus _VI_FUNC agn8700_QueryMaxCurrentLimit (ViSession vi,
                                                 ViConstString channelName,
                                                 ViReal64 voltageLevel,
                                                 ViReal64* maxCurrentLimit);

ViStatus _VI_FUNC agn8700_QueryMaxVoltageLevel (ViSession vi,
                                                 ViConstString channelName,
                                                 ViReal64 currentLimit,
                                                 ViReal64* maxVoltageLevel);

ViStatus _VI_FUNC agn8700_ImmediateTransientTrigger (ViSession vi,
                                            ViConstString channelName);

ViStatus _VI_FUNC agn8700_ResetOutputProtection (ViSession vi,
                                                 ViConstString channelName);

    /*- Triggering ---------------------------------------------------------*/
ViStatus _VI_FUNC agn8700_ConfigureTriggerSource (ViSession vi,
                                                  ViConstString channelName,
                                                  ViInt32 source);

ViStatus _VI_FUNC agn8700_ConfigureTriggeredVoltageLevel (ViSession vi,
                                                          ViConstString channelName,
                                                          ViReal64 level);

ViStatus _VI_FUNC agn8700_ConfigureTriggeredCurrentLimit (ViSession vi,
                                                          ViConstString channelName,
                                                          ViReal64 limit);

ViStatus _VI_FUNC agn8700_Abort (ViSession vi);

ViStatus _VI_FUNC agn8700_Initiate (ViSession vi);

    /*- Software Triggering ------------------------------------------------*/
ViStatus _VI_FUNC agn8700_SendSoftwareTrigger (ViSession vi);

    /*- Measuring ----------------------------------------------------------*/
ViStatus _VI_FUNC agn8700_Measure (ViSession vi,
                                   ViConstString channelName,
                                   ViInt32 measurementType,
                                   ViReal64* measurement);
    
    /*- Error Functions ----------------------------------------------------*/
ViStatus _VI_FUNC  agn8700_error_query (ViSession vi, ViInt32 *errorCode,
                                         ViChar errorMessage[]);

ViStatus _VI_FUNC  agn8700_GetError (ViSession vi, ViStatus *code, 
                                      ViInt32 bufferSize, ViChar description[]);
ViStatus _VI_FUNC  agn8700_ClearError (ViSession vi);

ViStatus _VI_FUNC  agn8700_error_message (ViSession vi, ViStatus errorCode,
                                           ViChar errorMessage[256]);

    /*- Interchangeability Checking Functions ------------------------------*/
ViStatus _VI_FUNC agn8700_GetNextInterchangeWarning (ViSession vi, 
                                                      ViInt32 bufferSize, 
                                                      ViChar warnString[]);
ViStatus _VI_FUNC agn8700_ResetInterchangeCheck (ViSession vi);
ViStatus _VI_FUNC agn8700_ClearInterchangeWarnings (ViSession vi);

    /*- Utility Functions --------------------------------------------------*/
ViStatus _VI_FUNC agn8700_InvalidateAllAttributes (ViSession vi);
ViStatus _VI_FUNC  agn8700_reset (ViSession vi);
ViStatus _VI_FUNC  agn8700_ResetWithDefaults (ViSession vi);
ViStatus _VI_FUNC  agn8700_self_test (ViSession vi, ViInt16 *selfTestResult,
                                       ViChar selfTestMessage[]);
ViStatus _VI_FUNC  agn8700_revision_query (ViSession vi, 
                                            ViChar instrumentDriverRevision[],
                                            ViChar firmwareRevision[]);
ViStatus _VI_FUNC agn8700_Disable (ViSession vi);
ViStatus _VI_FUNC  agn8700_WriteInstrData (ViSession vi, ViConstString writeBuffer); 
ViStatus _VI_FUNC  agn8700_ReadInstrData  (ViSession vi, ViInt32 numBytes, 
                                            ViChar rdBuf[], ViInt32 *bytesRead);

    /*- Set, Get, and Check Attribute Functions ----------------------------*/
ViStatus _VI_FUNC  agn8700_GetAttributeViInt32 (ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 *value);
ViStatus _VI_FUNC  agn8700_GetAttributeViReal64 (ViSession vi, ViConstString channelName, ViAttr attribute, ViReal64 *value);
ViStatus _VI_FUNC  agn8700_GetAttributeViString (ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 bufSize, ViChar value[]); 
ViStatus _VI_FUNC  agn8700_GetAttributeViSession (ViSession vi, ViConstString channelName, ViAttr attribute, ViSession *value);
ViStatus _VI_FUNC  agn8700_GetAttributeViBoolean (ViSession vi, ViConstString channelName, ViAttr attribute, ViBoolean *value);

ViStatus _VI_FUNC  agn8700_SetAttributeViInt32 (ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 value);
ViStatus _VI_FUNC  agn8700_SetAttributeViReal64 (ViSession vi, ViConstString channelName, ViAttr attribute, ViReal64 value);
ViStatus _VI_FUNC  agn8700_SetAttributeViString (ViSession vi, ViConstString channelName, ViAttr attribute, ViConstString value); 
ViStatus _VI_FUNC  agn8700_SetAttributeViSession (ViSession vi, ViConstString channelName, ViAttr attribute, ViSession value);
ViStatus _VI_FUNC  agn8700_SetAttributeViBoolean (ViSession vi, ViConstString channelName, ViAttr attribute, ViBoolean value);

ViStatus _VI_FUNC  agn8700_CheckAttributeViInt32 (ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 value);
ViStatus _VI_FUNC  agn8700_CheckAttributeViReal64 (ViSession vi, ViConstString channelName, ViAttr attribute, ViReal64 value);
ViStatus _VI_FUNC  agn8700_CheckAttributeViString (ViSession vi, ViConstString channelName, ViAttr attribute, ViConstString value); 
ViStatus _VI_FUNC  agn8700_CheckAttributeViSession (ViSession vi, ViConstString channelName, ViAttr attribute, ViSession value);
ViStatus _VI_FUNC  agn8700_CheckAttributeViBoolean (ViSession vi, ViConstString channelName, ViAttr attribute, ViBoolean value);

/****************************************************************************
 *------------------------ Error And Completion Codes ----------------------*
 ****************************************************************************/
#define AGN8700_ERROR_TRIGGER_NOT_SOFTWARE         IVIDCPWR_ERROR_TRIGGER_NOT_SOFTWARE

/**************************************************************************** 
 *---------------------------- End Include File ----------------------------* 
 ****************************************************************************/
#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif
#endif /* __AGN8700_HEADER */




