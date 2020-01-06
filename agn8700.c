/***************************************************************************** 
 *     2007-2010, National Instruments, Corporation.  All Rights Reserved.    * 
 *****************************************************************************/

/*****************************************************************************
 *  Agilent N8700 DC Power Supply Instrument Driver
 *  LabWindows/CVI Instrument Driver
 *  Original Release: 10/11/2010
 *  By: Haoran Feng
 *
 *  Modification History:
 *
 *  2010-10-11 [Haoran Feng] Instrument Driver Created.
 *
 *****************************************************************************/

#include <string.h>
#include <stdio.h>  
#include <formatio.h>
#include <ctype.h>
#include "agn8700.h"

/*****************************************************************************
 *--------------------- Hidden Attribute Declarations -----------------------*
 *****************************************************************************/

#define AGN8700_ATTR_OPC_TIMEOUT      (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 1L)       /* ViInt32 */
#define AGN8700_ATTR_VISA_RM_SESSION  (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 5L)   /* ViSession */
#define AGN8700_ATTR_OPC_CALLBACK     (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 6L)   /* ViAddr */
#define AGN8700_ATTR_CHECK_STATUS_CALLBACK    (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 7L)   /* ViAddr */
#define AGN8700_ATTR_USER_INTERCHANGE_CHECK_CALLBACK  (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 8L)   /* ViAddr */
#define AGN8700_ATTR_INSTR_INSTRUMENT_MODULES_RANGES (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 9L)   /* ViAddr */


/**
 * Define supported instrument modules.
 */
enum
{
	NOT_SUPPORTED = -1,
	N8731A = 8731,
	N8732A,
	N8733A,
	N8734A,
	N8735A,
	N8736A,
	N8737A,
	N8738A,
	N8739A,
	N8740A,
	N8741A,
	N8742A,
	N8754A = 8754,
	N8755A,
	N8756A,
	N8757A,
	N8758A,
	N8759A,
	N8760A,
	N8761A,
	N8762A,
} agn8700_ESupportedModules;

/**
 * Define table of the supported instrument modules
 * and their corresponding strings in ....
 */
static IviStringValueTable agn8700_ModuleTable =
{
    {NOT_SUPPORTED,""},
	{N8731A, "N8731A"},
	{N8732A, "N8732A"},
	{N8733A, "N8733A"},
	{N8734A, "N8734A"},
	{N8735A, "N8735A"},
	{N8736A, "N8736A"},
	{N8737A, "N8737A"},
	{N8738A, "N8738A"},
	{N8739A, "N8739A"},
	{N8740A, "N8740A"},
	{N8741A, "N8741A"},
	{N8742A, "N8742A"},

	{N8754A, "N8754A"},
	{N8755A, "N8755A"},
	{N8756A, "N8756A"},
	{N8757A, "N8757A"},
	{N8758A, "N8758A"},
	{N8759A, "N8759A"},
	{N8760A, "N8760A"},
	{N8761A, "N8761A"},
	{N8762A, "N8762A"},
    {VI_NULL,  VI_NULL}
};

/**
 * Define structure with ranges for instrument modules.
 */
typedef struct
{
	ViReal64	MinCurrent;
	ViReal64	MaxCurrent;
	ViReal64	MinVoltage;
	ViReal64	MaxVoltage;
	ViReal64	MaxPower;
	ViReal64	MinOVP;
	ViReal64	MaxOVP;
	ViReal64	MinLowVoltage;
	ViReal64	MaxLowVoltage;
	ViInt32		ModelType;
} SInstrumentRanges, *SInstrumentRangesPtr;


static SInstrumentRanges gs_N8731Ranges =
{
	0.0,   	/* Min Current */
	400.0,  /* Max Current */
	0.0,    /* Min Voltage */
	8.4,    /* Max Voltage */
	3200.0, /* Max Power   */
	0.5,    /* Min OVP     */   
	10.0,   /* Max OVP     */
	0.0,    /* Min Low Voltage */
	7.6,    /* Max Low Voltage */	
	N8731A, /* Model Type  */
};


static SInstrumentRanges gs_N8732Ranges =
{
	0.0,  	/* Min Current */
	330.0,  /* Max Current */
	0.0,   	/* Min Voltage */
	10.5,   /* Max Voltage */
	3300, 	/* Max Power   */
	0.5,   	/* Min OVP     */	
	12.0,  	/* Max OVP     */
	0.0,   	/* Min Low Voltage */
	9.5,   	/* Max Low Voltage */	
	N8732A,	/* Model Type  */
};


static SInstrumentRanges gs_N8733Ranges =
{
	0.0,  	/* Min Current */
	220.0,  /* Max Current */
	0.0,   	/* Min Voltage */
	15.75,	/* Max Voltage */
	3300,	/* Max Power   */
	1.0,   	/* Min OVP     */	
	18.0,  	/* Max OVP     */
	0.0,   	/* Min Low Voltage */
	14.25,  /* Max Low Voltage */	
	N8733A,	/* Model Type  */
};

static SInstrumentRanges gs_N8734Ranges =
{
	0.0,  	/* Min Current */
	165.0,  /* Max Current */
	0.0,   	/* Min Voltage */
	21.0,  	/* Max Voltage */
	3300.0, /* Max Power   */
	1.0,   	/* Min OVP     */	
	24.0,  	/* Max OVP     */
	0.0,   	/* Min Low Voltage */
	19.0,  	/* Max Low Voltage */	
	N8734A,	/* Model Type  */
};

static SInstrumentRanges gs_N8735Ranges =
{
	0.0,  	/* Min Current */
	110.0,  /* Max Current */
	0.0,   	/* Min Voltage */
	31.5,  	/* Max Voltage */
	3300.0, /* Max Power   */
	2.0,   	/* Min OVP     */	
	36.0,  	/* Max OVP     */
	0.0,   	/* Min Low Voltage */
	28.5,  	/* Max Low Voltage */	
	N8735A,	/* Model Type  */
};

static SInstrumentRanges gs_N8736Ranges =
{
	0.0, 	/* Min Current */
	85.0,  	/* Max Current */
	0.0,   	/* Min Voltage */
	42.0,  	/* Max Voltage */
	3400.0, /* Max Power   */
	2.0,   	/* Min OVP     */	
	44.0,  	/* Max OVP     */
	0.0,   	/* Min Low Voltage */
	38.0,  	/* Max Low Voltage */	
	N8736A,	/* Model Type  */
};

static SInstrumentRanges gs_N8737Ranges =
{
	0.0,  	/* Min Current */
	55.0,  	/* Max Current */
	0.0,   	/* Min Voltage */
	63.0, 	/* Max Voltage */
	3300.0, /* Max Power   */
	5.0,   	/* Min OVP     */	
	66.0,  	/* Max OVP     */
	0.0,   	/* Min Low Voltage */
	57.0,  	/* Max Low Voltage */
	N8737A,	/* Model Type  */
};

static SInstrumentRanges gs_N8738Ranges =
{
	0.0,  	/* Min Current */
	42.0,   /* Max Current */
	0.0,   	/* Min Voltage */
	84.0,  	/* Max Voltage */
	3360.0, /* Max Power   */
	5.0,   	/* Min OVP     */	
	88.0,  	/* Max OVP     */
	0.0,   	/* Min Low Voltage */
	76.0,  	/* Max Low Voltage */
	N8738A,	/* Model Type  */
};

static SInstrumentRanges gs_N8739Ranges =
{
	0.0,  	/* Min Current */
	33.0,   /* Max Current */
	0.0,   	/* Min Voltage */
	105.0,	/* Max Voltage */
	3300,	/* Max Power   */
	5.0,   	/* Min OVP     */
	110.0, 	/* Max OVP     */
	0.0,   	/* Min Low Voltage */
	95.0,  	/* Max Low Voltage */
	N8739A,	/* Model Type  */
};

static SInstrumentRanges gs_N8740Ranges =
{
	0.0,  	/* Min Current */
	22.0,   /* Max Current */
	0.0,   	/* Min Voltage */
	157.5, 	/* Max Voltage */
	3300.0,	/* Max Power   */
	5.0,   	/* Min OVP     */
	165.0, 	/* Max OVP     */
	0.0,   	/* Min Low Voltage */
	142.0, 	/* Max Low Voltage */
	N8740A,	/* Model Type  */
};

static SInstrumentRanges gs_N8741Ranges =
{
	0.0,  	/* Min Current */
	11.0,   /* Max Current */
	0.0,   	/* Min Voltage */
	315.0, 	/* Max Voltage */
	3300,	/* Max Power   */
	5.0,   	/* Min OVP     */	
	330.0, 	/* Max OVP     */
	0.0,   	/* Min Low Voltage */
	285.0, 	/* Max Low Voltage */
	N8741A,	/* Model Type  */
};

static SInstrumentRanges gs_N8742Ranges =
{
	0.0,   	/* Min Current */
	5.5,   	/* Max Current */
	0.0,   	/* Min Voltage */
	630.0, 	/* Max Voltage */
	3300, 	/* Max Power   */
	5.0,   	/* Min OVP     */
	660.0, 	/* Max OVP     */
	0.0,   	/* Min Low Voltage */
	570.0, 	/* Max Low Voltage */
	N8742A,	/* Model Type  */
};

static SInstrumentRanges gs_N8754Ranges =
{
	0.0,  	/* Min Current */
	250.0, 	/* Max Current */
	0.0,   	/* Min Voltage */
	21.0,   /* Max Voltage */
	5000.0,	/* Max Power   */
	1.0,   	/* Min OVP     */
	24.0,   /* Max OVP     */
	0.0,   	/* Min Low Voltage */
	19.0,   /* Max Low Voltage */
	N8754A,	/* Model Type  */
};

static SInstrumentRanges gs_N8755Ranges =
{
	0.0,  	/* Min Current */
	170.0, 	/* Max Current */
	0.0,   	/* Min Voltage */
	31.5,   /* Max Voltage */
	5100.0,	/* Max Power   */
	2.0,   	/* Min OVP     */ 
	36.0,  	/* Max OVP     */
	0.0,   	/* Min Low Voltage */
	28.5,   /* Max Low Voltage */
	N8755A,	/* Model Type  */
};


static SInstrumentRanges gs_N8756Ranges =
{
	0.0,   	/* Min Current */
	125.0,  /* Max Current */
	0.0,    /* Min Voltage */
	42.0, 	/* Max Voltage */
	5000.0, /* Max Power   */
	2.0,    /* Min OVP     */ 
	44.0,   /* Max OVP     */
	0.0,   	/* Min Low Voltage */
	38.0,  	/* Max Low Voltage */
	N8756A,	/* Model Type  */
};

static SInstrumentRanges gs_N8757Ranges =
{
	0.0,  	/* Min Current */
	85.0,  	/* Max Current */
	0.0,   	/* Min Voltage */
	63.0,  	/* Max Voltage */
	5100.0,	/* Max Power   */
	5.0,   	/* Min OVP     */ 
	66.0,  	/* Max OVP     */
	0.0,   	/* Min Low Voltage */
	57.0,  	/* Max Low Voltage */
	N8757A,	/* Model Type  */
};


static SInstrumentRanges gs_N8758Ranges =
{
	0.0,  	/* Min Current */
	65.0,  	/* Max Current */
	0.0,   	/* Min Voltage */
	84.0,  	/* Max Voltage */
	5200.0,	/* Max Power   */
	5.0,   	/* Min OVP     */ 	
	88.0,  	/* Max OVP     */
	0.0,   	/* Min Low Voltage */
	76.0,  	/* Max Low Voltage */
	N8758A,	/* Model Type  */
};


static SInstrumentRanges gs_N8759Ranges =
{
	0.0,  	/* Min Current */
	50.0,  	/* Max Current */
	0.0,   	/* Min Voltage */
	105.0,  /* Max Voltage */
	5000.0,	/* Max Power   */
	5.0,   	/* Min OVP     */ 	
	110.0,  /* Max OVP     */
	0.0,   	/* Min Low Voltage */
	95.0,  	/* Max Low Voltage */
	N8759A,	/* Model Type  */
};

static SInstrumentRanges gs_N8760Ranges =
{
	0.0,  	/* Min Current */
	34.0,  	/* Max Current */
	0.0,   	/* Min Voltage */
	157.5, 	/* Max Voltage */
	5000.0,	/* Max Power   */
	5.0,   	/* Min OVP     */ 
	165.0,  /* Max OVP     */
	0.0,   	/* Min Low Voltage */
	142.0,  /* Max Low Voltage */
	N8760A,	/* Model Type  */
};

static SInstrumentRanges gs_N8761Ranges =
{
	0.0,  	/* Min Current */
	17.0,  	/* Max Current */
	0.0,   	/* Min Voltage */
	315.0,  /* Max Voltage */
	5100.0,	/* Max Power   */
	5.0,   	/* Min OVP     */  	
	330.0,  /* Max OVP     */
	0.0,   	/* Min Low Voltage */
	285.0,  /* Max Low Voltage */
	N8761A,	/* Model Type  */
};

static SInstrumentRanges gs_N8762Ranges =
{
	0.0,  	/* Min Current */
	8.5,  	/* Max Current */
	0.0,   	/* Min Voltage */
	630.0,	/* Max Voltage */
	5100.0,	/* Max Power   */
	5.0,   	/* Min OVP     */  	
	660.0, 	/* Max OVP     */
	0.0,   	/* Min Low Voltage */
	570.0,  /* Max Low Voltage */
	N8762A,	/* Model Type  */
};

/*****************************************************************************
 *---------------------------- Useful Macros --------------------------------*
 *****************************************************************************/

    /*- I/O buffer size -----------------------------------------------------*/
#define BUFFER_SIZE                             512L        

    /*- 488.2 Event Status Register (ESR) Bits ------------------------------*/
#define IEEE_488_2_ERROR_MASK              0x3C

    /*- List of default Model. -*/
#define DEFAUL_MODEL   N8737A  //N8731A

#define CHANNEL_LIST "1"

static IviStringValueTable agn8700ViBoolean_commands =
{
	{AGN8700_ATTR_OUTPUT_ENABLED,		       ":OUTP:STAT"	          },
	{AGN8700_ATTR_INITIATE_CONTINUOUS,         ":INIT:CONT:TRAN"      },
	{VI_NULL,							       VI_NULL			      }
};

static IviStringValueTable agn8700ViReal64_commands =
{
	{AGN8700_ATTR_OVP_LIMIT,				":SOUR:VOLT:PROT:LEV"         },
	{AGN8700_ATTR_CURRENT_LIMIT,			":SOUR:CURR:LEV:IMM:AMPL"     },
	{AGN8700_ATTR_VOLTAGE_LEVEL,			":SOUR:VOLT:LEV:IMM:AMPL"     },
	{AGN8700_ATTR_TRIGGERED_CURRENT_LIMIT,	":SOUR:CURR:LEV:TRIG:AMPL"    },   	
	{AGN8700_ATTR_TRIGGERED_VOLTAGE_LEVEL,	":SOUR:VOLT:LEV:TRIG:AMPL"    },
	{AGN8700_ATTR_VOLTAGE_LIMIT_LOW,	    ":SOUR:VOLT:LIM:LOW"          },  	
	{VI_NULL,								VI_NULL			              }
};

static IviStringValueTable agn8700Enum_commands =
{
	{AGN8700_ATTR_CURRENT_LIMIT_BEHAVIOR,			":SOUR:CURR:PROT:STAT"},
    {AGN8700_ATTR_TRIGGER_SOURCE,					":TRIG:SOUR"          }, 	
	{VI_NULL,								        VI_NULL		          }
};

/*****************************************************************************
 *-------------- Utility Function Declarations (Non-Exported) ---------------*
 *****************************************************************************/
static ViStatus agn8700_IviInit (ViRsrc resourceName, ViBoolean IDQuery, 
                                     ViBoolean reset, ViSession vi);
static ViStatus agn8700_IviClose (ViSession vi);   

static ViStatus agn8700_InitAttributes (ViSession vi,ViInt32 pInstaledModel);
static ViStatus agn8700_DefaultInstrSetup (ViSession openInstrSession);
static ViStatus agn8700_CheckStatus (ViSession vi);
static ViStatus agn8700_WaitForOPC (ViSession vi, ViInt32 maxTime);
static ViStatus agn8700_GetDriverSetupOption(ViSession	vi, ViInt32*	pInstaledModel);
static ViStatus agn8700_GetInstrumentSetupOption(ViSession	vi, ViSession io, ViInt32*	pInstaledModel);
static void CVIFUNC agn8700_TrimString(char string[]);    


    /*- File I/O Utility Functions -*/
static ViStatus agn8700_ReadToFile (ViSession vi, ViConstString filename, 
                                     ViInt32 maxBytesToRead, ViInt32 fileAction, ViInt32 *totalBytesWritten);
static ViStatus agn8700_WriteFromFile (ViSession vi, ViConstString filename, 
                                        ViInt32 maxBytesToWrite, ViInt32 byteOffset, 
                                        ViInt32 *totalBytesWritten);

/*****************************************************************************
 *----------------- Callback Declarations (Non-Exported) --------------------*
 *****************************************************************************/

    /*- Global Session Callbacks --------------------------------------------*/
    
static ViStatus _VI_FUNC agn8700_CheckStatusCallback (ViSession vi, ViSession io);
static ViStatus _VI_FUNC agn8700_WaitForOPCCallback (ViSession vi, ViSession io);
static ViStatus _VI_FUNC agn8700ViBoolean_ReadCallback (
	ViSession		vi,
	ViSession		io,
	ViConstString	channelName,
	ViAttr			attributeId,
	ViBoolean*		value
);
static ViStatus _VI_FUNC agn8700ViBoolean_WriteCallback (
	ViSession		vi,
	ViSession		io,
	ViConstString	channelName,
	ViAttr			attributeId,
	ViBoolean		value
);
static ViStatus _VI_FUNC agn8700ViReal64_WriteCallback (
	ViSession		vi,
	ViSession		io,
	ViConstString	channelName,
	ViAttr			attributeId,
	ViReal64		value
);
static ViStatus _VI_FUNC agn8700ViReal64_ReadCallback (
	ViSession		vi,
	ViSession		io,
	ViConstString	channelName,
	ViAttr			attributeId,
	ViReal64*		value
);
static ViStatus _VI_FUNC agn8700Enum_WriteCallback (
	ViSession      vi,
	ViSession      io,
	ViConstString  channelName,
	ViAttr         attributeId,
	ViInt32        value
);
static ViStatus _VI_FUNC agn8700Enum_ReadCallback (
	ViSession      vi,
	ViSession      io,
	ViConstString  channelName,
	ViAttr         attributeId,
	ViInt32*       value
);


    /*- Attribute callbacks -------------------------------------------------*/

static ViStatus _VI_FUNC agn8700AttrDriverRevision_ReadCallback (ViSession vi,
                                                                  ViSession io, 
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId, 
                                                                  const ViConstString cacheValue);
static ViStatus _VI_FUNC agn8700AttrInstrumentManufacturer_ReadCallback (ViSession vi, 
                                                                          ViSession io,
                                                                          ViConstString channelName, 
                                                                          ViAttr attributeId,
                                                                          const ViConstString cacheValue);
static ViStatus _VI_FUNC agn8700AttrInstrumentModel_ReadCallback (ViSession vi, 
                                                                   ViSession io,
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId,
                                                                   const ViConstString cacheValue);

static ViStatus _VI_FUNC agn8700AttrFirmwareRevision_ReadCallback (ViSession vi, 
                                                                    ViSession io,
                                                                    ViConstString channelName, 
                                                                    ViAttr attributeId,
                                                                    const ViConstString cacheValue);
static ViStatus _VI_FUNC agn8700AttrIdQueryResponse_ReadCallback (ViSession vi,
                                                                   ViSession io, 
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId, 
                                                                   const ViConstString cacheValue);
static ViStatus _VI_FUNC agn8700AttrVoltageLimitLow_CheckCallback (ViSession vi,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViReal64 value);
static ViStatus _VI_FUNC agn8700AttrOvpLimit_CheckCallback (ViSession vi,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViReal64 value);

static ViStatus _VI_FUNC agn8700AttrCurrentLimit_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);

static ViStatus _VI_FUNC agn8700AttrVoltageLevel_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);


static ViStatus _VI_FUNC agn8700AttrTriggeredCurrentLimit_CheckCallback (ViSession vi,
                                                                         ViConstString channelName,
                                                                         ViAttr attributeId,
                                                                         ViReal64 value);

static ViStatus _VI_FUNC agn8700AttrOvpEnabled_CheckCallback (ViSession vi,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViBoolean value);

static ViStatus _VI_FUNC agn8700ViBoolean_ReadCallback (ViSession vi,
                                                        ViSession io,
                                                        ViConstString channelName,
                                                        ViAttr attributeId,
                                                        ViBoolean *value);

static ViStatus _VI_FUNC agn8700ViBoolean_WriteCallback (ViSession vi,
                                                         ViSession io,
                                                         ViConstString channelName,
                                                         ViAttr attributeId,
                                                         ViBoolean value);

/*****************************************************************************
 *------------ User-Callable Functions (Exportable Functions) ---------------*
 *****************************************************************************/


/*****************************************************************************
 * Function: agn8700_init   
 * Purpose:  VXIplug&play required function.  Calls the   
 *           agn8700_InitWithOptions function.   
 *****************************************************************************/
ViStatus _VI_FUNC agn8700_init (ViRsrc resourceName, ViBoolean IDQuery,
                                 ViBoolean resetDevice, ViSession *newVi)
{   
    ViStatus    error = VI_SUCCESS;

    if (newVi == VI_NULL)
        {
        Ivi_SetErrorInfo (VI_NULL, VI_FALSE, IVI_ERROR_INVALID_PARAMETER, 
                          VI_ERROR_PARAMETER4, "Null address for Instrument Handle");
        checkErr( IVI_ERROR_INVALID_PARAMETER);
        }

    checkErr( agn8700_InitWithOptions (resourceName, IDQuery, resetDevice, 
                                        "", newVi));

Error:  
    return error;
}

/*****************************************************************************
 * Function: agn8700_InitWithOptions                                       
 * Purpose:  This function creates a new IVI session and calls the 
 *           IviInit function.                                     
 *****************************************************************************/
ViStatus _VI_FUNC agn8700_InitWithOptions(
    ViRsrc          resourceName,
    ViBoolean       IDQuery,
    ViBoolean       resetDevice,
    ViConstString   optionString,
    ViSession      *newVi
)
{   
    ViStatus    error = VI_SUCCESS;
    ViSession   vi = VI_NULL;
    ViChar      newResourceName[IVI_MAX_MESSAGE_BUF_SIZE];
    ViChar      newOptionString[IVI_MAX_MESSAGE_BUF_SIZE];
    ViBoolean   isLogicalName;
    
    if (newVi == VI_NULL)
        {
        Ivi_SetErrorInfo (VI_NULL, VI_FALSE, IVI_ERROR_INVALID_PARAMETER, 
                          VI_ERROR_PARAMETER5, "Null address for Instrument Handle");
        checkErr( IVI_ERROR_INVALID_PARAMETER);
        }

    *newVi = VI_NULL;
    
    checkErr( Ivi_GetInfoFromResourceName( resourceName, (ViString)optionString,
                                           newResourceName,
                                           newOptionString,
                                           &isLogicalName));
    
        /* create a new interchangeable driver */
    checkErr( Ivi_SpecificDriverNew ("agn8700", newOptionString, &vi));  
    if (!isLogicalName)
    {
        ViInt32 oldFlag = 0;
        
        checkErr (Ivi_GetAttributeFlags (vi, IVI_ATTR_IO_RESOURCE_DESCRIPTOR, &oldFlag));
        checkErr (Ivi_SetAttributeFlags (vi, IVI_ATTR_IO_RESOURCE_DESCRIPTOR, oldFlag & 0xfffb | 0x0010));
        checkErr (Ivi_SetAttributeViString (vi, "", IVI_ATTR_IO_RESOURCE_DESCRIPTOR, 0, newResourceName));
    }
        /* init the driver */
    checkErr( agn8700_IviInit (newResourceName, IDQuery, resetDevice, vi)); 
    if (isLogicalName)
        checkErr( Ivi_ApplyDefaultSetup (vi));
    *newVi = vi;
    
Error:
    if (error < VI_SUCCESS) 
        Ivi_Dispose (vi);
        
    return error;
}

/*****************************************************************************
 * Function: agn8700_close                                                           
 * Purpose:  This function closes the instrument.                            
 *
 *           Note:  This function must unlock the session before calling
 *           Ivi_Dispose.
 *****************************************************************************/
ViStatus _VI_FUNC agn8700_close (ViSession vi)
{
    ViStatus    error = VI_SUCCESS;
    
    checkErr( Ivi_LockSession (vi, VI_NULL));
    
    checkErr( agn8700_IviClose (vi));

Error:    
    Ivi_UnlockSession (vi, VI_NULL);
    Ivi_Dispose (vi);  

    return error;
}

/*****************************************************************************
 * Function: agn8700_ImmediateTransientTrigger                                                         
 * Purpose:  This function This function generates an immediate transient 
 *           trigger regardless of the selected trigger source.                          
 *****************************************************************************/
ViStatus _VI_FUNC agn8700_ImmediateTransientTrigger (ViSession vi,
                                            ViConstString channelName)
{
    ViStatus	error = VI_SUCCESS;

    checkErr( Ivi_LockSession(vi, VI_NULL));


    if (!Ivi_Simulating (vi))
	{
        ViSession io = Ivi_IOSession(vi);
        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

		viCheckErr( viPrintf (io, ":TRIG:TRAN"));
	}

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: agn8700_reset                                                         
 * Purpose:  This function resets the instrument.                          
 *****************************************************************************/
ViStatus _VI_FUNC agn8700_reset (ViSession vi)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

	if (!Ivi_Simulating(vi))                /* call only when locked */
	    {
	    ViSession   io = Ivi_IOSession(vi); /* call only when locked */
	    
	    checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
	    viCheckErr( viPrintf (io, "*RST"));
	    }
	
	checkErr( agn8700_DefaultInstrSetup (vi));                                
	
	checkErr( agn8700_CheckStatus (vi));                                      

Error:
    Ivi_UnlockSession (vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: agn8700_ResetWithDefaults
 * Purpose:  This function resets the instrument and applies default settings
 *           from the IVI Configuration Store based on the logical name
 *           from which the session was created.
 *****************************************************************************/
ViStatus _VI_FUNC agn8700_ResetWithDefaults (ViSession vi)
{
    ViStatus error = VI_SUCCESS;
    
    checkErr( Ivi_LockSession (vi, VI_NULL));
    checkErr( agn8700_reset(vi));
    checkErr( Ivi_ApplyDefaultSetup(vi));
    
Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/**************************************************************************** 
 *  Function: agn8700_Disable
 *  Purpose:  This function places the instrument in a quiescent state as 
 *            quickly as possible.
 ****************************************************************************/
ViStatus _VI_FUNC agn8700_Disable (ViSession vi)
{
    ViStatus error = VI_SUCCESS;
    
    checkErr( Ivi_LockSession (vi, VI_NULL));
    
    if (!Ivi_Simulating(vi)) 
        {
        }
Error:
    Ivi_UnlockSession (vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: agn8700_self_test                                                       
 * Purpose:  This function executes the instrument self-test and returns the 
 *           result.                                                         
 *****************************************************************************/
ViStatus _VI_FUNC agn8700_self_test (ViSession vi, ViInt16 *testResult, 
                                      ViChar testMessage[])
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (testResult == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Test Result");
    if (testMessage == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Test Message");

	if (!Ivi_Simulating(vi))                /* call only when locked */
	    {
	    ViSession   io = Ivi_IOSession(vi); /* call only when locked */
	
	    checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
	    viCheckErr( viPrintf (io, "*TST?"));
	        
		viCheckErr( viScanf (io, "%hd", testResult));
		
		if (testMessage)
        sprintf(testMessage, "Self-Test %s.",
                (!(*testResult)) ? "Passed" : "Failed");
	
	    }
	else
	    {
	        /* Simulate Self Test */
	    *testResult = 0;
	    strcpy (testMessage, "No error.");
	    }
	
	checkErr( agn8700_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: agn8700_revision_query                                                  
 * Purpose:  This function returns the driver and instrument revisions.      
 *****************************************************************************/
ViStatus _VI_FUNC agn8700_revision_query (ViSession vi, ViChar driverRev[], 
                                           ViChar instrRev[])
{
    ViStatus    error = VI_SUCCESS;
    
    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (driverRev == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Driver Revision");
    if (instrRev == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Instrument Revision");

    checkErr( Ivi_GetAttributeViString (vi, VI_NULL, AGN8700_ATTR_SPECIFIC_DRIVER_REVISION, 
                                        0, 256, driverRev));
    checkErr( Ivi_GetAttributeViString (vi, "", AGN8700_ATTR_INSTRUMENT_FIRMWARE_REVISION, 
                                        0, 256, instrRev));
    checkErr( agn8700_CheckStatus (vi));

Error:    
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: agn8700_error_query                                                     
 * Purpose:  This function queries the instrument error queue and returns   
 *           the result.                                                     
 *****************************************************************************/
ViStatus _VI_FUNC agn8700_error_query (ViSession vi, ViInt32 *errCode, 
                                        ViChar errMessage[])
{
    ViStatus    error = VI_SUCCESS;
    
    checkErr( Ivi_LockSession (vi, VI_NULL));
    
    if (errCode == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Error Code");
    if (errMessage == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Error Message");

	if (!Ivi_Simulating(vi))                /* call only when locked */
	    {
	    ViSession   io = Ivi_IOSession(vi); /* call only when locked */
	
	    checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
	    viCheckErr( viPrintf(io, ":SYST:ERR?"));
	
		viCheckErr( viScanf (io, "%ld,\"%256[^\"]", errCode, errMessage));
	
	    }
	else
	    {
	        /* Simulate Error Query */
	    *errCode = 0;
	    strcpy (errMessage, "No error.");
	    }

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: agn8700_error_message                                                  
 * Purpose:  This function translates the error codes returned by this       
 *           instrument driver into user-readable strings.  
 *
 *           Note:  The caller can pass VI_NULL for the vi parameter.  This 
 *           is useful if one of the init functions fail.
 *****************************************************************************/
ViStatus _VI_FUNC agn8700_error_message (ViSession vi, ViStatus errorCode,
                                          ViChar errorMessage[256])
{
    ViStatus    error = VI_SUCCESS;
    
    static      IviStringValueTable errorTable = 
        {
			IVIDCPWR_ERROR_CODES_AND_MSGS,
            {VI_NULL,                               VI_NULL}
        };
        
    if (vi)
        Ivi_LockSession(vi, VI_NULL);

        /* all VISA and IVI error codes are handled as well as codes in the table */
    if (errorMessage == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Error Message");

    checkErr( Ivi_GetSpecificDriverStatusDesc(vi, errorCode, errorMessage, errorTable));

Error:
    if (vi)
        Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: agn8700_InvalidateAllAttributes
 * Purpose:  This function invalidates the cached value of all attributes.
 *****************************************************************************/
ViStatus _VI_FUNC agn8700_InvalidateAllAttributes (ViSession vi)
{
    return Ivi_InvalidateAllAttributes(vi);
}

/*****************************************************************************
 * Function: agn8700_GetError and agn8700_ClearError Functions                       
 * Purpose:  These functions enable the instrument driver user to  
 *           get or clear the error information the driver associates with the
 *           IVI session.                                                        
 *****************************************************************************/
ViStatus _VI_FUNC agn8700_GetError (ViSession vi, 
                                     ViStatus *errorCode, 
                                     ViInt32 bufferSize,
                                     ViChar description[])  
{
    ViStatus error = VI_SUCCESS;
    ViStatus primary = VI_SUCCESS,
             secondary = VI_SUCCESS;
    ViChar   elaboration[256] = "";
    ViChar   errorMessage[1024] = "";
    ViChar  *appendPoint = errorMessage;
    ViInt32  actualSize = 0;
    ViBoolean locked = VI_FALSE;

    /* Lock Session */
    if (vi != 0)
        {
        checkErr( Ivi_LockSession(vi, &locked));
        }

    /* Test for nulls and acquire error data */
    if (bufferSize != 0)
        {
        if (errorCode == VI_NULL)
            {
            viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Error");
            }
        if (description == VI_NULL)
            {
            viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for Description");
            }
        checkErr( Ivi_GetErrorInfo (vi, &primary, &secondary, elaboration));
        }

    else
        {
        checkErr( Ivi_GetAttributeViString(vi, VI_NULL, IVI_ATTR_ERROR_ELABORATION, 0, 256, elaboration));
        checkErr( Ivi_GetAttributeViInt32(vi, VI_NULL, IVI_ATTR_SECONDARY_ERROR, 0, &secondary));
        checkErr( Ivi_GetAttributeViInt32(vi, VI_NULL, IVI_ATTR_PRIMARY_ERROR, 0, &primary));
        }
        
    /* Format data */
    if (primary != VI_SUCCESS)
        {
        ViChar msg[256] = "";
        checkErr( agn8700_error_message (vi, primary, msg));
        appendPoint += sprintf(appendPoint, "Primary Error: (Hex 0x%08X) %s\n", primary, msg);
        }
    
    if (secondary != VI_SUCCESS)
        {
        ViChar msg[256] = "";
        checkErr( agn8700_error_message (vi, secondary, msg));
        appendPoint += sprintf(appendPoint, "Secondary Error: (Hex 0x%08X) %s\n", secondary, msg);
        }
    
    if (elaboration[0])
        {
        sprintf(appendPoint, "Elaboration: %s", elaboration);
        }
    
    actualSize = (ViInt32)strlen(errorMessage) + 1;
    
    /* Prepare return values */
    if (bufferSize == 0)
        {
        error = actualSize;
        }
    else 
        {
        if (bufferSize > 0)
            {
            if (actualSize > bufferSize)
                {
                error = actualSize;
                actualSize = bufferSize;
                }
            }
        memcpy(description, errorMessage, actualSize-1);
        description[actualSize-1] = '\0';
        }
    
    if (errorCode) 
        {
        *errorCode = primary;
        }
    
Error:
    /* Unlock Session */
    Ivi_UnlockSession (vi, &locked);
    return error;
}   

ViStatus _VI_FUNC agn8700_ClearError (ViSession vi)                                                        
{                                                                                                           
    return Ivi_ClearErrorInfo (vi);                                                                             
}

/*****************************************************************************
 * Function: agn8700_GetNextCoercionRecord                        
 * Purpose:  This function enables the instrument driver user to obtain
 *           the coercion information associated with the IVI session.                                                              
 *           This function retrieves and clears the oldest instance in which 
 *           the instrument driver coerced a value the instrument driver user
 *           specified to another value.                     
 *****************************************************************************/
ViStatus _VI_FUNC agn8700_GetNextCoercionRecord (ViSession vi,
                                                  ViInt32 bufferSize,
                                                  ViChar  record[])
{
    return Ivi_GetNextCoercionString (vi, bufferSize, record);
}

/**************************************************************************** 
 *  Function: agn8700_GetNextInterchangeWarning,
 *            agn8700_ResetInterchangeCheck, and
 *            agn8700_ClearInterchangeWarnings
 *  Purpose:  These functions allow the user to retrieve interchangeability
 *            warnings, reset the driver's interchangeability checking 
 *            state, and clear all previously logged interchangeability warnings.
 ****************************************************************************/
ViStatus _VI_FUNC agn8700_GetNextInterchangeWarning (ViSession vi, 
                                                      ViInt32 bufferSize, 
                                                      ViChar warnString[])
{
    return Ivi_GetNextInterchangeCheckString (vi, bufferSize, warnString);
}

ViStatus _VI_FUNC agn8700_ResetInterchangeCheck (ViSession vi)
{
    return Ivi_ResetInterchangeCheck (vi);
}

ViStatus _VI_FUNC agn8700_ClearInterchangeWarnings (ViSession vi)
{
    return Ivi_ClearInterchangeWarnings (vi);
}

/*****************************************************************************
 * Function: agn8700_LockSession and agn8700_UnlockSession Functions                        
 * Purpose:  These functions enable the instrument driver user to lock the 
 *           session around a sequence of driver calls during which other
 *           execution threads must not disturb the instrument state.
 *                                                                          
 *           NOTE:  The callerHasLock parameter must be a local variable 
 *           initialized to VI_FALSE and passed by reference, or you can pass 
 *           VI_NULL.                     
 *****************************************************************************/
ViStatus _VI_FUNC agn8700_LockSession (ViSession vi, ViBoolean *callerHasLock)  
{                                              
    return Ivi_LockSession(vi,callerHasLock);      
}                                              
ViStatus _VI_FUNC agn8700_UnlockSession (ViSession vi, ViBoolean *callerHasLock) 
{                                              
    return Ivi_UnlockSession(vi,callerHasLock);    
}   

/**************************************************************************** 
 *  Function: agn8700_GetChannelName
 *  Purpose:  This function returns the highest-level channel name that
 *            corresponds to the specific driver channel string that is in
 *            the channel table at an index you specify.
 ****************************************************************************/
ViStatus _VI_FUNC agn8700_GetChannelName (ViSession vi, ViInt32 index,
                                           ViInt32 bufferSize,
                                           ViChar name[])
{
    ViStatus         error = VI_SUCCESS;
    ViConstString    channelName;
    ViInt32          actualSize = 0;
    ViInt32			channelCount;

    checkErr( Ivi_LockSession (vi, VI_NULL));
    
    /* Check parameters */
    if (bufferSize != 0)
        {
        if (name == VI_NULL)
            {
            viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for Channel Name");
            }
        }
        
    checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, IVI_ATTR_CHANNEL_COUNT, 0, &channelCount));
    if ( (index<1) || (index>channelCount) )
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Channel Index");
    
    /* Get channel name */
    viCheckParm( Ivi_GetNthChannelString (vi, index, &channelName), 2, "Index");
    checkErr( Ivi_GetUserChannelName (vi, channelName, &channelName));
    
    actualSize = (ViInt32)strlen(channelName) + 1;
    
    /* Prepare return values */
    if (bufferSize == 0)
        {
        error = actualSize;
        }
    else 
        {
        if (bufferSize > 0)
            {
            if (actualSize > bufferSize)
                {
                error = actualSize;
                actualSize = bufferSize;
                }
            }
        memcpy(name, channelName, actualSize-1);
        name[actualSize-1] = '\0';
        }

Error:    
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: agn8700_ConfigureOutputEnabled                                   
 * Purpose:  This function controls whether or not the signal the power supply
 *           generates is present at the output terminal.
 *****************************************************************************/
ViStatus _VI_FUNC agn8700_ConfigureOutputEnabled (ViSession vi,
                                                   ViConstString channelName,
                                                   ViBoolean enabled)
{
    return Ivi_SetAttributeViBoolean (vi, channelName, AGN8700_ATTR_OUTPUT_ENABLED,
                                      IVI_VAL_DIRECT_USER_CALL, enabled);
}

/*****************************************************************************
 * Function: agn8700_ConfigureOutputRange                                   
 * Purpose:  This function determines the output range in which the power
 *           supply operates.
 *****************************************************************************/
static IviRangeTableEntry rangeTypeRangeTableEntries[] =
	{
		{AGN8700_VAL_RANGE_CURRENT, 0, 0, "CURR", AGN8700_ATTR_CURRENT_LIMIT},
		{AGN8700_VAL_RANGE_VOLTAGE, 0, 0, "VOLT", AGN8700_ATTR_VOLTAGE_LEVEL},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable rangeTypeRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        rangeTypeRangeTableEntries,
	};


ViStatus _VI_FUNC agn8700_ConfigureOutputRange (ViSession vi,
                                                 ViConstString channelName,
                                                 ViInt32 rangeType,
                                                 ViReal64 range)
{
    ViStatus error = VI_SUCCESS;

    checkErr( Ivi_LockSession(vi, VI_NULL));

    if (Ivi_RangeChecking(vi))
	{
        SInstrumentRangesPtr	pinstrument_model = VI_NULL;
        ViReal64 				maximum;
        ViReal64 				minimum;        

        viCheckParm( Ivi_GetViInt32EntryFromValue (rangeType,
                                        &rangeTypeRangeTable,
                                        VI_NULL, VI_NULL, VI_NULL,
                                        VI_NULL, VI_NULL, VI_NULL),
                                        3, "Range Type");

		checkErr( Ivi_GetAttributeViAddr( vi,
										  VI_NULL,
										  AGN8700_ATTR_INSTR_INSTRUMENT_MODULES_RANGES,
										  0, (ViAddr*)&pinstrument_model));

		if (rangeType ==  AGN8700_VAL_RANGE_VOLTAGE) {
			minimum = pinstrument_model->MinVoltage;
			maximum = pinstrument_model->MaxVoltage;
		} 
		else 
		{
			minimum = pinstrument_model->MinCurrent;
			maximum = pinstrument_model->MaxCurrent;
		}

		if ( range < minimum || range > maximum )
		{
			viCheckParm( IVI_ERROR_INVALID_VALUE, 4, "Range");
		}
	}


/*	// we don't support configure range, so no actual operation should happen.
	checkErr( Ivi_SetAttributeViReal64( vi,
			 						    channelName,
                                        (rangeType ==  AGN8700_VAL_RANGE_VOLTAGE) ?
			 						    AGN8700_ATTR_INSTR_VOLTAGE_RANGE :
                                        AGN8700_ATTR_INSTR_CURRENT_RANGE ,
			 						    0, range));

    checkErr( agn8700_CheckStatus (vi));  */

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: agn8700_ConfigureCurrentLimit                                   
 * Purpose:  This function configures the behavior and level of the
 *           current limit.
 *****************************************************************************/
ViStatus _VI_FUNC agn8700_ConfigureCurrentLimit (ViSession vi,
                                                  ViConstString channelName,
                                                  ViInt32 behavior,
                                                  ViReal64 limit)
{
    ViStatus error = VI_SUCCESS;
    ViBoolean behaviorFirst = VI_FALSE;

   checkErr( Ivi_LockSession (vi, VI_NULL));

    /*
     * If you are setting the instrument to trip, set the limit and then the behavior
     * to prevent a premature over-current condition.
     */

    behaviorFirst = (behavior != AGN8700_VAL_CURRENT_TRIP);

    if (behaviorFirst)
    {
        viCheckParm( Ivi_SetAttributeViInt32 (vi, channelName,
                                              AGN8700_ATTR_CURRENT_LIMIT_BEHAVIOR,
                                              0, behavior), 3,
                                              "Current Limit Behavior");

    }
    viCheckParm( Ivi_SetAttributeViReal64 (vi, channelName,
                                               AGN8700_ATTR_CURRENT_LIMIT,
                                               0, limit), 4,
                                               "Current Limit");
    if (!behaviorFirst)
    {
        viCheckParm( Ivi_SetAttributeViInt32 (vi, channelName,
                                              AGN8700_ATTR_CURRENT_LIMIT_BEHAVIOR,
                                              0, behavior), 3,
                                              "Current Limit Behavior");
    }

    checkErr( agn8700_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: agn8700_ConfigureOVP                                   
 * Purpose:  This function enables or disables over-voltage protection and
 *           configures the over-voltage limit.
 *****************************************************************************/
ViStatus _VI_FUNC agn8700_ConfigureOVP (ViSession vi,
                                         ViConstString channelName,
                                         ViBoolean enabled,
                                         ViReal64 limit)
{
    ViStatus error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

	/*-- N8700 doesn't support disable OVP, it's always on --*/
    viCheckParm( Ivi_SetAttributeViBoolean (vi, channelName, 
                                           AGN8700_ATTR_OVP_ENABLED,
                                           0, enabled), 3, "OVP Enabled");
	
    viCheckParm( Ivi_SetAttributeViReal64 (vi, channelName, 
                                           AGN8700_ATTR_OVP_LIMIT,
                                           0, limit), 4, "OVP Limit");

    checkErr( agn8700_CheckStatus (vi));

Error:    
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: agn8700_ConfigureVoltageLevel                                   
 * Purpose:  This function configures the voltage level the power supply
 *           attempts to generate.
 *****************************************************************************/
ViStatus _VI_FUNC agn8700_ConfigureVoltageLevel (ViSession vi,
                                                  ViConstString channelName,
                                                  ViReal64 level)
{
    return Ivi_SetAttributeViReal64 (vi, channelName, AGN8700_ATTR_VOLTAGE_LEVEL,
                                     IVI_VAL_DIRECT_USER_CALL, level);
}

/*****************************************************************************
 * Function: agn8700_QueryOutputState                                   
 * Purpose:  This function returns a value indicating if the power supply
 *           is in a particular output state.
 *****************************************************************************/
static IviRangeTableEntry outputStateRangeTableEntries[] =
	{
		{AGN8700_VAL_OUTPUT_CONSTANT_VOLTAGE, 0, 0, "OPER", 8},
		{AGN8700_VAL_OUTPUT_CONSTANT_CURRENT, 0, 0, "OPER", 10},
		{AGN8700_VAL_OUTPUT_OVER_VOLTAGE, 0, 0, "QUES", 0},
		{AGN8700_VAL_OUTPUT_OVER_CURRENT, 0, 0, "QUES", 1},
		{AGN8700_VAL_OUTPUT_OVER_TEMPERATURE, 0, 0, "QUES", 4},
		{AGN8700_VAL_OUTPUT_UNREGULATED, 0, 0, "QUES", 10},
		{AGN8700_VAL_WAIT_TRANS_TRIG, 0, 0, "OPER", 5},
		{AGN8700_VAL_INHIBIT, 0, 0, "QUES", 9},
		{AGN8700_VAL_POWER_FAILED, 0, 0, "QUES", 2},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};
static IviRangeTable outputStateRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_FALSE,
        VI_FALSE,
        VI_NULL,
        outputStateRangeTableEntries,
	};

ViStatus _VI_FUNC agn8700_QueryOutputState (ViSession vi,
                                             ViConstString channelName,
                                             ViInt32 outputState,
                                             ViBoolean* inState)
{
    ViStatus	error = VI_SUCCESS;
    ViString	cmd;
    ViBoolean	in_state = VI_FALSE;
    ViInt32		bit, value = 1;

    checkErr( Ivi_LockSession(vi, VI_NULL));

    if (inState == VI_NULL)
        viCheckParm (IVI_ERROR_INVALID_PARAMETER, 4, "Null value for inState.");

    viCheckParm( Ivi_GetViInt32EntryFromValue( outputState,
                                               &outputStateRangeTable,
                                               VI_NULL, VI_NULL, VI_NULL,
                                               VI_NULL, &cmd, &bit),
                                               3, "Output State");

    if (!Ivi_Simulating (vi))
    {
        ViSession io = Ivi_IOSession(vi);

        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
        viCheckErr( viQueryf (io, ":STAT:%s:COND?", "%ld",
                                                        cmd,
                                                        &value));
    }


    in_state = ((value & (1<<bit)) != 0);
    checkErr( agn8700_CheckStatus (vi));

Error:
	if (inState) *inState = in_state;
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
	
}

/*****************************************************************************
 * Function: agn8700_QueryMaxCurrentLimit                  
 * Purpose:  This function returns the maximum programmable current limit
 *           that the power supply accepts for a particular voltage level on
 *           a channel for the output range to which the power supply is
 *           currently configured.
 *****************************************************************************/
ViStatus _VI_FUNC agn8700_QueryMaxCurrentLimit (ViSession vi,
                                                 ViConstString channelName,
                                                 ViReal64 voltageLevel,
                                                 ViReal64* maxCurrentLimit)
{
    ViStatus error = VI_SUCCESS;
    SInstrumentRangesPtr	pinstrument_model = VI_NULL;
    ViReal64	value = 0.0;
    ViReal64	voltage_range;
	ViInt32		range_index;
	ViInt32		result = 0;
	ViReal64	minimum;
	ViReal64	maximum;

    checkErr( Ivi_LockSession(vi, VI_NULL));

    if (maxCurrentLimit == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null value for maxCurrentLimit.");

    if (Ivi_RangeChecking(vi))
    {

    	checkErr( Ivi_GetAttributeViAddr( vi,
								  VI_NULL,
								  AGN8700_ATTR_INSTR_INSTRUMENT_MODULES_RANGES,
								  0, (ViAddr*)&pinstrument_model));
		
		minimum = pinstrument_model->MinVoltage;
		maximum = pinstrument_model->MaxVoltage;

		if ( voltageLevel < minimum || voltageLevel > maximum )
			viCheckParm( IVI_ERROR_INVALID_VALUE, 3, "Voltage Level");
    }

    if (!Ivi_Simulating (vi))
    {
  		ViSession   io = Ivi_IOSession(vi);

  		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
    	viCheckErr( viQueryf (io, "SOUR:CURR:LEV:IMM:AMPL? MAX", "%Le", &value));
	}
	else
	{
    	checkErr( Ivi_GetAttributeViAddr( vi,
								  VI_NULL,
								  AGN8700_ATTR_INSTR_INSTRUMENT_MODULES_RANGES,
								  0, (ViAddr*)&pinstrument_model));
		
		value = pinstrument_model->MaxCurrent;
	}

    checkErr( agn8700_CheckStatus (vi));

Error:
    if (maxCurrentLimit) *maxCurrentLimit = value;
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: agn8700_QueryMaxVoltageLevel                  
 * Purpose:  This function returns the maximum programmable voltage level
 *           that the power supply accepts for a particular current limit on
 *           a channel for the output range to which the power supply is
 *           currently configured.
 *****************************************************************************/
ViStatus _VI_FUNC agn8700_QueryMaxVoltageLevel (ViSession vi,
                                                 ViConstString channelName,
                                                 ViReal64 currentLimit,
                                                 ViReal64* maxVoltageLevel)
{
    ViStatus error = VI_SUCCESS;
    ViReal64 value = 0.0;
    ViConstString	pchannel_name = NULL;
    SInstrumentRangesPtr	pinstrument_model = VI_NULL;
	ViReal64	minimum;
	ViReal64	maximum;
	ViInt32		result = 0;
	ViReal64	current_range;
	ViInt32		range_index;

    checkErr( Ivi_LockSession(vi, VI_NULL));

    if (maxVoltageLevel == VI_NULL)
        viCheckParm (IVI_ERROR_INVALID_PARAMETER, 4, "Null value for maxVoltageLevel.");

    if (Ivi_RangeChecking(vi))
	{
		 checkErr( Ivi_GetAttributeViAddr( vi,
										  VI_NULL,
										  AGN8700_ATTR_INSTR_INSTRUMENT_MODULES_RANGES,
										  0, (ViAddr*)&pinstrument_model));

		minimum = pinstrument_model->MinCurrent;
		maximum = pinstrument_model->MaxCurrent;

		if ( currentLimit < minimum || currentLimit > maximum )
			viCheckParm( IVI_ERROR_INVALID_VALUE, 3, "Current Limit");
	}

    if (!Ivi_Simulating (vi))
    {
  		ViSession   io = Ivi_IOSession(vi);

  		checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
  		viCheckErr( viQueryf (io, "SOUR:VOLT:LEV:IMM:AMPL? MAX", "%Le", &value));
	}
	else
	{
		checkErr( Ivi_GetAttributeViAddr( vi,
										  VI_NULL,
										  AGN8700_ATTR_INSTR_INSTRUMENT_MODULES_RANGES,
										  0, (ViAddr*)&pinstrument_model));

		value = pinstrument_model->MaxVoltage;
	}



    checkErr( agn8700_CheckStatus (vi));

Error:
    if (maxVoltageLevel) *maxVoltageLevel = value;
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: agn8700_ResetOutputProtection                                   
 * Purpose:  This function resets the power supply's output protection.
 *****************************************************************************/
ViStatus _VI_FUNC agn8700_ResetOutputProtection (ViSession vi,
                                                  ViConstString channelName)
{
    ViStatus		error = VI_SUCCESS;

    checkErr( Ivi_LockSession(vi, VI_NULL));

    if (!Ivi_Simulating (vi))
    {
        ViSession io = Ivi_IOSession(vi);

        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
        viCheckErr( viPrintf (io, "OUTP:PROT:CLE"));
    }
    checkErr( agn8700_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: agn8700_ConfigureVoltageLimitLow                                   
 * Purpose:  This function configures the low voltage limit.
 *****************************************************************************/
ViStatus _VI_FUNC agn8700_ConfigureVoltageLimitLow (ViSession vi,
                                           ViConstString channelName, ViReal64 limit)
{
    return Ivi_SetAttributeViReal64 (vi, channelName, AGN8700_ATTR_VOLTAGE_LIMIT_LOW,
                                     IVI_VAL_DIRECT_USER_CALL, limit);
}

/*****************************************************************************
 * Function: agn8700_ConfigureTriggerSource                                   
 * Purpose:  This function configures the trigger source.
 *****************************************************************************/
ViStatus _VI_FUNC agn8700_ConfigureTriggerSource (ViSession vi,
                                                  ViConstString channelName,
                                                  ViInt32 source)
{
    return Ivi_SetAttributeViInt32 (vi, channelName, AGN8700_ATTR_TRIGGER_SOURCE,
                                    IVI_VAL_DIRECT_USER_CALL, source);
}

/*****************************************************************************
 * Function: agn8700_ConfigureTriggeredVoltageLevel                                   
 * Purpose:  This function configures the voltage level the power supply
 *           attempts to generate after it receives a trigger.
 *****************************************************************************/
ViStatus _VI_FUNC agn8700_ConfigureTriggeredVoltageLevel (ViSession vi,
                                                           ViConstString channelName,
                                                           ViReal64 level)
{
    return Ivi_SetAttributeViReal64 (vi, channelName,
                                     AGN8700_ATTR_TRIGGERED_VOLTAGE_LEVEL,
                                     IVI_VAL_DIRECT_USER_CALL, level);
}

/*****************************************************************************
 * Function: agn8700_ConfigureTriggeredCurrentLimit                                   
 * Purpose:  This function configures the current limit the power supply
 *           attempts to generate after it receives a trigger.
 *****************************************************************************/
ViStatus _VI_FUNC agn8700_ConfigureTriggeredCurrentLimit (ViSession vi,
                                                           ViConstString channelName,
                                                           ViReal64 limit)
{
    return Ivi_SetAttributeViReal64 (vi, channelName,
                                     AGN8700_ATTR_TRIGGERED_CURRENT_LIMIT,
                                     IVI_VAL_DIRECT_USER_CALL, limit);
}

/*****************************************************************************
 * Function: agn8700_Abort                                   
 * Purpose:  This function causes the instrument to no longer wait for a
 *           trigger.
 *****************************************************************************/
ViStatus _VI_FUNC agn8700_Abort (ViSession vi)
{
    ViStatus	error = VI_SUCCESS;

    checkErr( Ivi_LockSession(vi, VI_NULL));

    checkErr( Ivi_InvalidateAttribute (vi, IVI_VAL_ALL_CHANNELS,
                                       AGN8700_ATTR_TRIGGERED_VOLTAGE_LEVEL));
    checkErr( Ivi_InvalidateAttribute (vi, IVI_VAL_ALL_CHANNELS,
                                       AGN8700_ATTR_TRIGGERED_CURRENT_LIMIT));
    if (!Ivi_Simulating (vi))
    {
        ViSession io = Ivi_IOSession(vi);

        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

        viCheckErr( viPrintf (io, "ABOR"));
	}
    /*
        Do not invoke the agn8700_CheckStatus function here.  It
        is invoked by the high-level driver functions when this function is
        used internally.  After the user calls this function, the user can check
        for errors by calling the agn8700_error_query function.
    */

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: agn8700_Initiate                                   
 * Purpose:  This function causes the instrument to wait for a trigger from
 *           the trigger source.  After it receives a trigger, it updates the
 *           triggered current limit and triggered voltage level.
 *****************************************************************************/
ViStatus _VI_FUNC agn8700_Initiate (ViSession vi)
{
    ViStatus	error = VI_SUCCESS;

    checkErr( Ivi_LockSession(vi, VI_NULL));

    if (!Ivi_Simulating (vi))
	{
        ViSession io = Ivi_IOSession(vi);
        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

		viCheckErr( viPrintf (io, "INIT:IMM:TRAN"));
	}

    /*
        Do not invoke the agn8700_CheckStatus function here.  It
        is invoked by the high-level driver functions when this function is
        used internally.  After the user calls this function, the user can check
        for errors by calling the agn8700_error_query function.
    */

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: agn8700_SendSoftwareTrigger                                   
 * Purpose:  This function sends a software trigger to the instrument.  If the
 *           instrument is not configured to receive a software trigger, this
 *           function returns an error.
 *****************************************************************************/
ViStatus _VI_FUNC agn8700_SendSoftwareTrigger (ViSession vi)
{
	ViStatus error = VI_SUCCESS;

    checkErr( Ivi_LockSession(vi, VI_NULL));

    if (Ivi_RangeChecking (vi))
    {
        ViBoolean isSoftware = VI_FALSE;
        ViInt32 numChans;
        ViString channel;
        ViInt32 i;
        ViInt32 source;

        checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL,
                                           AGN8700_ATTR_CHANNEL_COUNT,
                                           0, &numChans));
        for (i = 1; i <= numChans && !isSoftware; i++)
            {
            checkErr( Ivi_GetNthChannelString (vi, i, &channel));
            checkErr( Ivi_GetAttributeViInt32 (vi, channel,
                                               AGN8700_ATTR_TRIGGER_SOURCE,
                                               0, &source));
            isSoftware = (source == AGN8700_VAL_SOFTWARE_TRIG);
            }
        if (!isSoftware)
            {
            viCheckErr (AGN8700_ERROR_TRIGGER_NOT_SOFTWARE);
            }
    }

    if (!Ivi_Simulating (vi))
        {
        ViSession io = Ivi_IOSession(vi);

        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

        viCheckErr( viPrintf (io, "*TRG"));
        }


    /*
        Do not invoke the agn8700_CheckStatus function here.  It
        is invoked by the high-level driver functions when this function is
        used internally.  After the user calls this function, the user can check
        for errors by calling the agn8700_error_query function.
    */

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
	
}


/*****************************************************************************
 * Function: agn8700_Measure                                   
 * Purpose:  This function measures either the current or voltage that the
 *           power supply is generating.
 *****************************************************************************/
static IviRangeTableEntry measurementTypeRangeTableEntries[] = 
    {   
        {AGN8700_VAL_MEASURE_CURRENT, 0, 0, "CURR", AGN8700_ATTR_CURRENT_LIMIT},
        {AGN8700_VAL_MEASURE_VOLTAGE, 0, 0, "VOLT", AGN8700_ATTR_VOLTAGE_LEVEL},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };
static IviRangeTable measurementTypeRangeTable = 
    {
        IVI_VAL_DISCRETE,
        VI_FALSE,
        VI_FALSE,
        VI_NULL,
        measurementTypeRangeTableEntries
    };

ViStatus _VI_FUNC agn8700_Measure (ViSession vi,
                                   ViConstString channelName,
                                   ViInt32 measurementType,
                                   ViReal64* measurement)
{
    ViStatus		error = VI_SUCCESS;
    ViReal64 		value = 0.0;
    ViAttr   		attrId;
    ViString 		cmd;

    checkErr( Ivi_LockSession(vi, VI_NULL));

    if (measurement == VI_NULL)
        viCheckParm (IVI_ERROR_INVALID_PARAMETER, 4, "Null value for measurement.");

    viCheckParm( Ivi_GetViInt32EntryFromValue (measurementType,
                                            &measurementTypeRangeTable,
                                            VI_NULL, VI_NULL, VI_NULL,
                                            VI_NULL, &cmd, &attrId),
                                            3, "Measurement Type");
    if (!Ivi_Simulating (vi))
    {
		ViSession io = Ivi_IOSession(vi);
        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

		viCheckErr( viQueryf (io, ":MEAS:SCAL:%s?", "%Le", cmd, &value));
	}
    else
	{
        checkErr( Ivi_GetAttributeViReal64 (vi, channelName, attrId, 0, &value));
        value *= 0.95;
	}

    checkErr( agn8700_CheckStatus (vi));

Error:
    if (measurement) *measurement = value;
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
            
}

/*****************************************************************************
 * Function: agn8700_GetAttribute<type> Functions                                    
 * Purpose:  These functions enable the instrument driver user to get 
 *           attribute values directly.  There are typesafe versions for 
 *           ViInt32, ViReal64, ViString, ViBoolean, and ViSession attributes.                                         
 *****************************************************************************/
ViStatus _VI_FUNC agn8700_GetAttributeViInt32 (ViSession vi, ViConstString channelName, 
                                                ViAttr attributeId, ViInt32 *value)
{                                                                                                           
    return Ivi_GetAttributeViInt32 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                    value);
}                                                                                                           
ViStatus _VI_FUNC agn8700_GetAttributeViReal64 (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViReal64 *value)
{                                                                                                           
    return Ivi_GetAttributeViReal64 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     value);
}                                                                                                           
ViStatus _VI_FUNC agn8700_GetAttributeViString (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViInt32 bufSize, 
                                                 ViChar value[]) 
{   
    return Ivi_GetAttributeViString (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     bufSize, value);
}   
ViStatus _VI_FUNC agn8700_GetAttributeViBoolean (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViBoolean *value)
{                                                                                                           
    return Ivi_GetAttributeViBoolean (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           
ViStatus _VI_FUNC agn8700_GetAttributeViSession (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViSession *value)
{                                                                                                           
    return Ivi_GetAttributeViSession (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           

/*****************************************************************************
 * Function: agn8700_SetAttribute<type> Functions                                    
 * Purpose:  These functions enable the instrument driver user to set 
 *           attribute values directly.  There are typesafe versions for 
 *           ViInt32, ViReal64, ViString, ViBoolean, and ViSession datatypes.                                         
 *****************************************************************************/
ViStatus _VI_FUNC agn8700_SetAttributeViInt32 (ViSession vi, ViConstString channelName, 
                                                ViAttr attributeId, ViInt32 value)
{                                                                                                           
    return Ivi_SetAttributeViInt32 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                    value);
}                                                                                                           
ViStatus _VI_FUNC agn8700_SetAttributeViReal64 (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViReal64 value)
{                                                                                                           
    return Ivi_SetAttributeViReal64 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     value);
}                                                                                                           
ViStatus _VI_FUNC agn8700_SetAttributeViString (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViConstString value) 
{   
    return Ivi_SetAttributeViString (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     value);
}   
ViStatus _VI_FUNC agn8700_SetAttributeViBoolean (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViBoolean value)
{                                                                                                           
    return Ivi_SetAttributeViBoolean (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           
ViStatus _VI_FUNC agn8700_SetAttributeViSession (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViSession value)
{                                                                                                           
    return Ivi_SetAttributeViSession (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           

/*****************************************************************************
 * Function: agn8700_CheckAttribute<type> Functions                                  
 * Purpose:  These functions enable the instrument driver user to check  
 *           attribute values directly.  These functions check the value you
 *           specify even if you set the AGN8700_ATTR_RANGE_CHECK 
 *           attribute to VI_FALSE.  There are typesafe versions for ViInt32, 
 *           ViReal64, ViString, ViBoolean, and ViSession datatypes.                         
 *****************************************************************************/
ViStatus _VI_FUNC agn8700_CheckAttributeViInt32 (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViInt32 value)
{                                                                                                           
    return Ivi_CheckAttributeViInt32 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}
ViStatus _VI_FUNC agn8700_CheckAttributeViReal64 (ViSession vi, ViConstString channelName, 
                                                   ViAttr attributeId, ViReal64 value)
{                                                                                                           
    return Ivi_CheckAttributeViReal64 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                       value);
}                                                                                                           
ViStatus _VI_FUNC agn8700_CheckAttributeViString (ViSession vi, ViConstString channelName, 
                                                   ViAttr attributeId, ViConstString value)  
{   
    return Ivi_CheckAttributeViString (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                       value);
}   
ViStatus _VI_FUNC agn8700_CheckAttributeViBoolean (ViSession vi, ViConstString channelName, 
                                                    ViAttr attributeId, ViBoolean value)
{                                                                                                           
    return Ivi_CheckAttributeViBoolean (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                        value);
}                                                                                                           
ViStatus _VI_FUNC agn8700_CheckAttributeViSession (ViSession vi, ViConstString channelName, 
                                                    ViAttr attributeId, ViSession value)
{                                                                                                           
    return Ivi_CheckAttributeViSession (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                        value);
}                                                                                                           


/*****************************************************************************
 * Function: WriteInstrData and ReadInstrData Functions                      
 * Purpose:  These functions enable the instrument driver user to  
 *           write and read commands directly to and from the instrument.            
 *                                                                           
 *           Note:  These functions bypass the IVI attribute state caching.  
 *                  WriteInstrData invalidates the cached values for all 
 *                  attributes.
 *****************************************************************************/
ViStatus _VI_FUNC agn8700_WriteInstrData (ViSession vi, ViConstString writeBuffer)   
{   
    return Ivi_WriteInstrData (vi, writeBuffer);    
}   
ViStatus _VI_FUNC agn8700_ReadInstrData (ViSession vi, ViInt32 numBytes, 
                                          ViChar rdBuf[], ViInt32 *bytesRead)  
{   
    return Ivi_ReadInstrData (vi, numBytes, rdBuf, bytesRead);   
} 

/*****************************************************************************
 *-------------------- Utility Functions (Not Exported) ---------------------*
 *****************************************************************************/

/*****************************************************************************
 *  Function: agn8700_GetDriverSetupOption
 *  Purpose:  Get the value of a DriverSetup option. The driver setup string
 *            is part of the option string the user passes to the
 *            agesa_InitWithOptions function. Specifically it is the part
 *            that follows "DriverSetup=". This function assumes the
 *            DriverSetup string has the following format:
 *              "Model : OptionValue"
 *****************************************************************************/
static ViStatus agn8700_GetDriverSetupOption(
	ViSession	vi,
	ViInt32*	pInstaledModel	/* Returns Model Type */
)
{
    ViStatus	error = VI_SUCCESS;
	ViChar*		ptag;
	ViChar		setup[BUFFER_SIZE],
                model_buffer[256],
                module_tag[256];
	ViInt32		module_type;

	/* Initialize output variables */
	*pInstaledModel = NOT_SUPPORTED;

    checkErr( Ivi_GetAttributeViString (vi,
										VI_NULL,
										IVI_ATTR_DRIVER_SETUP,
										0, BUFFER_SIZE,
										setup));

	sprintf(module_tag, "Model");
	module_type = NOT_SUPPORTED;

	if ((ptag = strstr (setup, module_tag)) != VI_NULL)
	{
        sscanf(ptag, "%*[^:]:%256[^;]", model_buffer);
		agn8700_TrimString(model_buffer);
   		if (Ivi_GetValueFromTable (agn8700_ModuleTable, model_buffer, &module_type) < VI_SUCCESS)
		{
            ViChar message[BUFFER_SIZE];
            sprintf (message, "Bad value for %s option in DriverSetup: %s",
						 module_tag,
						 model_buffer);

	        viCheckErrElab (IVI_ERROR_BAD_OPTION_VALUE, message);
		}
	}

	*pInstaledModel = module_type;

Error:
    return error;
}


/*****************************************************************************
 *  Function: agn8700_GetInstrumentSetupOption
 *  Purpose:  Reads information from instrument, which contains installed
 *            modules.
 *****************************************************************************/
static ViStatus agn8700_GetInstrumentSetupOption(
	ViSession	vi,
	ViSession	io,
	ViInt32*	pInstaledModel	/* Returns Model Type */
)
{
    ViStatus	error = VI_SUCCESS;
	ViChar		module_buffer[256];
	ViInt32	    i,
		        module_type;

	/* Initialize output variables */
	*pInstaledModel = NOT_SUPPORTED;

	viCheckErr( viQueryf(io, "*IDN?", "%*[^,],%[^,]", module_buffer));
	if (Ivi_GetValueFromTable (agn8700_ModuleTable, module_buffer, &module_type) == VI_SUCCESS)
	{
			*pInstaledModel = module_type;
	}
	
Error:
    return error;
}

/*******************************************************************************
 * Function:  agn8700_TrimString
 * Purpose:   Remove Surrounding WhiteSpace 
*******************************************************************************/
static void CVIFUNC agn8700_TrimString(char str[]){
	int len = (int)strlen(str);
	int fpos = 0;
	int epos = len-1;

	int i;
	for(i=0;i<len;i++){
		if(' '== str[i]) {
			++fpos;
		}else{
			break;
		}				
	}

	for(i=len-1;i>=0;i--){
		if(' '== str[i]) {
			--epos;
		}else{
			break;
		}				
	}

	i = 0;
	while(fpos<=epos){
		str[i++] = str[fpos++];
	}
	str[i]='\0';
}


/*****************************************************************************
 * Function: agn8700_IviInit                                                       
 * Purpose:  This function is called by agn8700_InitWithOptions  
 *           or by an IVI class driver.  This function initializes the I/O 
 *           interface, optionally resets the device, optionally performs an
 *           ID query, and sends a default setup to the instrument.                
 *****************************************************************************/
static ViStatus agn8700_IviInit (ViRsrc resourceName, ViBoolean IDQuery,
                                    ViBoolean reset, ViSession vi)
{
    ViStatus    error = VI_SUCCESS;
    ViSession   io = VI_NULL;
	ViInt32		installed_model = NOT_SUPPORTED;
    ViInt32		i;
	ViChar		channel_list[BUFFER_SIZE];
	sprintf(channel_list, "%s", CHANNEL_LIST);	        
	
    if (!Ivi_Simulating(vi))
	{
        ViSession   rmSession = VI_NULL;

        /* Open instrument session */
        checkErr( Ivi_GetAttributeViSession (vi, VI_NULL, IVI_ATTR_VISA_RM_SESSION, 0,
                                             &rmSession));
        viCheckErr( viOpen (rmSession, resourceName, VI_NULL, VI_NULL, &io));
        /* io session owned by driver now */
        checkErr( Ivi_SetAttributeViSession (vi, VI_NULL, IVI_ATTR_IO_SESSION, 0, io));

		/* Configure VISA Formatted I/O */
		viCheckErr( viSetAttribute (io, VI_ATTR_TMO_VALUE, 5000 ));
		viCheckErr( viSetBuf (io, VI_READ_BUF | VI_WRITE_BUF, 4000));
		viCheckErr( viSetAttribute (io, VI_ATTR_WR_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS));
		viCheckErr( viSetAttribute (io, VI_ATTR_RD_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS));

		/*
			If ID Query is enabled & the driver is not in simulated mode, the driver 
			should get the instrument module from IDN response by sending "*IDN?" and 
			then get the response.
		*/   
		if(IDQuery){
			viCheckErr( agn8700_GetInstrumentSetupOption (vi, io, &installed_model));
			
			/* model not supported */
			if ( installed_model == NOT_SUPPORTED ) {
				viCheckErr( VI_ERROR_FAIL_ID_QUERY);	 
			}
		}
	}
	
	if(Ivi_Simulating(vi)||(!IDQuery)){
		checkErr( agn8700_GetDriverSetupOption (vi, &installed_model));
	}

	/* use default model */
	if ( installed_model == NOT_SUPPORTED )
	{
#if defined( DEFAUL_MODEL )
		 installed_model = DEFAUL_MODEL;
#endif
	} 
	
    checkErr( Ivi_BuildChannelTable (vi, channel_list, VI_FALSE, VI_NULL));	

     /* Add attributes */
    checkErr( agn8700_InitAttributes (vi,installed_model));

    /*- Reset instrument ----------------------------------------------------*/
    if (reset) 
        checkErr( agn8700_reset (vi));
    else  /*- Send Default Instrument Setup ---------------------------------*/
        checkErr( agn8700_DefaultInstrSetup (vi));
	
	/*- Identification Query ------------------------------------------------*/
	if (IDQuery)                               
	    {
	    ViChar rdBuffer[BUFFER_SIZE];
	
	    #define VALID_RESPONSE_STRING_START        "Agilent Technologies,N87"
	
	    checkErr( Ivi_GetAttributeViString (vi, VI_NULL, AGN8700_ATTR_ID_QUERY_RESPONSE, 
	                                        0, BUFFER_SIZE, rdBuffer));
	    
	    if (strncmp (rdBuffer, VALID_RESPONSE_STRING_START, 
	                 strlen(VALID_RESPONSE_STRING_START)) != 0)
	        {             
	        viCheckErr( VI_ERROR_FAIL_ID_QUERY);
	        }
	    }

    checkErr( agn8700_CheckStatus (vi));

Error:
    if (error < VI_SUCCESS)
        {
        if (!Ivi_Simulating (vi) && io)
            viClose (io);
        }
    return error;
}

/*****************************************************************************
 * Function: agn8700_IviClose                                                        
 * Purpose:  This function performs all of the drivers clean-up operations   
 *           except for closing the IVI session.  This function is called by 
 *           agn8700_close or by an IVI class driver. 
 *
 *           Note:  This function must close the I/O session and set 
 *           IVI_ATTR_IO_SESSION to 0.
 *****************************************************************************/
static ViStatus agn8700_IviClose (ViSession vi)
{
    ViStatus error = VI_SUCCESS;
    ViSession io = VI_NULL;

        /* Do not lock here.  The caller manages the lock. */

    checkErr( Ivi_GetAttributeViSession (vi, VI_NULL, IVI_ATTR_IO_SESSION, 0, &io));

Error:
    Ivi_SetAttributeViSession (vi, VI_NULL, IVI_ATTR_IO_SESSION, 0, VI_NULL);
    if(io)                                                      
        {
        viClose (io);
        }
    return error;   
}

/*****************************************************************************
 * Function: agn8700_CheckStatus                                                 
 * Purpose:  This function checks the status of the instrument to detect 
 *           whether the instrument has encountered an error.  This function  
 *           is called at the end of most exported driver functions.  If the    
 *           instrument reports an error, this function returns      
 *           IVI_ERROR_INSTRUMENT_SPECIFIC.  The user can set the 
 *           AGN8700_ATTR_QUERY_INSTRUMENT_STATUS attribute to VI_FALSE to disable this 
 *           check and increase execution speed.                                   
 *
 *           Note:  Call this function only when the session is locked.
 *****************************************************************************/
static ViStatus agn8700_CheckStatus (ViSession vi)
{
    ViStatus    error = VI_SUCCESS;

    if (Ivi_QueryInstrStatus (vi) && Ivi_NeedToCheckStatus (vi) && !Ivi_Simulating (vi))
        {
        checkErr( agn8700_CheckStatusCallback (vi, Ivi_IOSession(vi)));
        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_FALSE));
        }
        
Error:
    return error;
}

/*****************************************************************************
 * Function: agn8700_WaitForOPC                                                  
 * Purpose:  This function waits for the instrument to complete the      
 *           execution of all pending operations.  This function is a        
 *           wrapper for the WaitForOPCCallback.  It can be called from 
 *           other instrument driver functions. 
 *
 *           The maxTime parameter specifies the maximum time to wait for
 *           operation complete in milliseconds.
 *
 *           Note:  Call this function only when the session is locked.
 *****************************************************************************/
static ViStatus agn8700_WaitForOPC (ViSession vi, ViInt32 maxTime) 
{
    ViStatus    error = VI_SUCCESS;

    if (!Ivi_Simulating(vi))
        {
        ViInt32  oldOPCTimeout; 
        
        checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, AGN8700_ATTR_OPC_TIMEOUT, 
                                           0, &oldOPCTimeout));
        Ivi_SetAttributeViInt32 (vi, VI_NULL, AGN8700_ATTR_OPC_TIMEOUT,        
                                 0, maxTime);

        error = agn8700_WaitForOPCCallback (vi, Ivi_IOSession(vi));

        Ivi_SetAttributeViInt32 (vi, VI_NULL, AGN8700_ATTR_OPC_TIMEOUT, 
                                 0, oldOPCTimeout);
        viCheckErr( error);
        }
Error:
    return error;
}

/*****************************************************************************
 * Function: agn8700_DefaultInstrSetup                                               
 * Purpose:  This function sends a default setup to the instrument.  The    
 *           agn8700_reset function calls this function.  The 
 *           agn8700_IviInit function calls this function when the
 *           user passes VI_FALSE for the reset parameter.  This function is 
 *           useful for configuring settings that other instrument driver 
 *           functions require.    
 *
 *           Note:  Call this function only when the session is locked.
 *****************************************************************************/
static ViStatus agn8700_DefaultInstrSetup (ViSession vi)
{
    ViStatus    error = VI_SUCCESS;
        
    /* Invalidate all attributes */
    checkErr( Ivi_InvalidateAllAttributes (vi));
    
    if (!Ivi_Simulating(vi))
    {
        ViSession   io = Ivi_IOSession(vi); /* call only when locked */

        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
		
		viCheckErr( viPrintf (io, "*CLS;*ESE 1;*SRE 32"));  
    }

Error:
    return error;
}


/*****************************************************************************
 * Function: ReadToFile and WriteFromFile Functions                          
 * Purpose:  Functions for instrument driver developers to read/write        
 *           instrument data to/from a file.                                 
 *****************************************************************************/
static ViStatus agn8700_ReadToFile (ViSession vi, ViConstString filename, 
                                     ViInt32 maxBytesToRead, ViInt32 fileAction, 
                                     ViInt32 *totalBytesWritten)  
{   
    return Ivi_ReadToFile (vi, filename, maxBytesToRead, fileAction, totalBytesWritten);  
}   
static ViStatus agn8700_WriteFromFile (ViSession vi, ViConstString filename, 
                                        ViInt32 maxBytesToWrite, ViInt32 byteOffset, 
                                        ViInt32 *totalBytesWritten) 
{   
    return Ivi_WriteFromFile (vi, filename, maxBytesToWrite, byteOffset, totalBytesWritten); 
}

/*****************************************************************************
 *------------------------ Global Session Callbacks -------------------------*
 *****************************************************************************/

/*****************************************************************************
 * Function: agn8700_CheckStatusCallback                                               
 * Purpose:  This function queries the instrument to determine if it has 
 *           encountered an error.  If the instrument has encountered an 
 *           error, this function returns the IVI_ERROR_INSTRUMENT_SPECIFIC 
 *           error code.  This function is called by the 
 *           agn8700_CheckStatus utility function.  The 
 *           Ivi_SetAttribute and Ivi_GetAttribute functions invoke this 
 *           function when the optionFlags parameter includes the
 *           IVI_VAL_DIRECT_USER_CALL flag.
 *           
 *           The user can disable calls to this function by setting the 
 *           AGN8700_QUERY_INSTRUMENT_STATUS attribute to VI_FALSE.  The driver can 
 *           disable the check status callback for a particular attribute by 
 *           setting the IVI_VAL_DONT_CHECK_STATUS flag.
 *****************************************************************************/
static ViStatus _VI_FUNC agn8700_CheckStatusCallback (ViSession vi, ViSession io)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     esr = 0;

    viCheckErr( viQueryf (io, "*ESR?", "%ld", &esr));
    if (esr & IEEE_488_2_ERROR_MASK)
	{
        viCheckErr( IVI_ERROR_INSTR_SPECIFIC);
	}
	
Error:
    return error;
}


/*****************************************************************************
 * Function: agn8700ViBoolean_WriteCallback
 * Purpose:  This function sets the bool value of the ViBoolean attribute.
 *****************************************************************************/
static ViStatus _VI_FUNC agn8700ViBoolean_WriteCallback (ViSession vi,
                                                         ViSession io,
                                                         ViConstString channelName,
                                                         ViAttr attributeId,
                                                         ViBoolean value)
{
    ViStatus error = VI_SUCCESS;
	ViString 	command;

	viCheckErr( Ivi_GetStringFromTable( agn8700ViBoolean_commands,
											attributeId,
											&command));
	if (!Ivi_Simulating (vi))
	{
		viCheckErr( viPrintf (io, "%s %s", command, value ? "ON" : "OFF"));
	}
    
Error:
    return error;
}


/*****************************************************************************
 * Function: agn8700ViBoolean_ReadCallback
 * Purpose:  This function reads the bool value of the ViBoolean attribute.
 *****************************************************************************/
static ViStatus _VI_FUNC agn8700ViBoolean_ReadCallback (ViSession vi,
                                                        ViSession io,
                                                        ViConstString channelName,
                                                        ViAttr attributeId,
                                                        ViBoolean *value)
{
    ViStatus error = VI_SUCCESS;
	ViString 	command;

	viCheckErr( Ivi_GetStringFromTable (agn8700ViBoolean_commands, attributeId, &command));

	if (!Ivi_Simulating (vi))
	{
		viCheckErr( viQueryf (io, "%s?", "%hd", command, value));
	}
	
    
Error:
    return error;
}


/*****************************************************************************
 * Function: agn8700ViReal64_WriteCallback
 * Purpose:  This function sets the bool value of the ViReal64 attribute.
 *****************************************************************************/
static ViStatus _VI_FUNC agn8700ViReal64_WriteCallback (
	ViSession		vi,
	ViSession		io,
	ViConstString	channelName,
	ViAttr			attributeId,
	ViReal64		value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;

	viCheckErr( Ivi_GetStringFromTable (agn8700ViReal64_commands, attributeId, &command));

	if (!Ivi_Simulating (vi))
	{
			viCheckErr( viPrintf (io, "%s %Le", command, value));
	}

Error:
	return error;
}

/*****************************************************************************
 * Function: agn8700ViReal64_ReadCallback
 * Purpose:  This function reads the bool value of the ViReal64 attribute.
 *****************************************************************************/
static ViStatus _VI_FUNC agn8700ViReal64_ReadCallback (
	ViSession		vi,
	ViSession		io,
	ViConstString	channelName,
	ViAttr 			attributeId,
	ViReal64*		value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	command;

	viCheckErr( Ivi_GetStringFromTable (agn8700ViReal64_commands, attributeId, &command));

	if (!Ivi_Simulating (vi))
	{

		viCheckErr( viQueryf (io, "%s?", "%Le", command, value));
	}

Error:
	return error;
}

/*****************************************************************************
 * Function: agn8700Enum_WriteCallback
 * Purpose:  This function sets the enum value of the ViInt32 attribute.
 *****************************************************************************/
static ViStatus _VI_FUNC agn8700Enum_WriteCallback (
	ViSession      vi,
	ViSession      io,
	ViConstString  channelName,
	ViAttr         attributeId,
	ViInt32        value
)
{
	ViStatus			error = VI_SUCCESS;
	ViString 			command,
						commandValue;
	IviRangeTablePtr 	table;

	viCheckErr( Ivi_GetStringFromTable (agn8700Enum_commands, attributeId, &command));
	checkErr( Ivi_GetAttrRangeTable (vi, channelName, attributeId, &table));
	viCheckErr( Ivi_GetViInt32EntryFromValue (value, table, VI_NULL, VI_NULL,
											  VI_NULL, VI_NULL, &commandValue,
											  VI_NULL));

	if (!Ivi_Simulating (vi))								
	{
		viCheckErr( viPrintf (io, "%s %s", command, commandValue));
	}


Error:
	return error;
}

/*****************************************************************************
 * Function: agn8700Enum_ReadCallback
 * Purpose:  This function reads the enum value of the ViInt32 attribute.
 *****************************************************************************/
static ViStatus _VI_FUNC agn8700Enum_ReadCallback (
	ViSession vi,
	ViSession io,
	ViConstString channelName,
	ViAttr attributeId,
	ViInt32 *value
)
{
	ViStatus			error = VI_SUCCESS;
	ViString 			command;
	IviRangeTablePtr 	table;

	viCheckErr( Ivi_GetStringFromTable (agn8700Enum_commands, attributeId, &command));
	checkErr( Ivi_GetAttrRangeTable (vi, channelName, attributeId, &table));

	if (!Ivi_Simulating (vi))
	{
		ViChar	buffer[BUFFER_SIZE];

		viCheckErr( viQueryf (io, "%s?", "%s", command, buffer));
		
		viCheckErr( Ivi_GetViInt32EntryFromString (buffer, table, value, VI_NULL,
												   VI_NULL, VI_NULL, VI_NULL));
	}
Error:
	return error;
}

/*****************************************************************************
 * Function: agn8700_WaitForOPCCallback                                               
 * Purpose:  This function waits until the instrument has finished processing 
 *           all pending operations.  This function is called by the 
 *           agn8700_WaitForOPC utility function.  The IVI engine invokes
 *           this function in the following two cases:
 *           - Before invoking the read callback for attributes for which the 
 *             IVI_VAL_WAIT_FOR_OPC_BEFORE_READS flag is set.
 *           - After invoking the write callback for attributes for which the 
 *             IVI_VAL_WAIT_FOR_OPC_AFTER_WRITES flag is set.
 *****************************************************************************/
static ViStatus _VI_FUNC agn8700_WaitForOPCCallback (ViSession vi, ViSession io)
{
    ViStatus    error = VI_SUCCESS;

    return error;
}

/*****************************************************************************
 *----------------- Attribute Range Tables and Callbacks --------------------*
 *****************************************************************************/

/*- AGN8700_ATTR_ID_QUERY_RESPONSE -*/

static ViStatus _VI_FUNC agn8700AttrIdQueryResponse_ReadCallback (ViSession vi, 
                                                                   ViSession io,
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId,
                                                                   const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];
    ViUInt32    retCnt;
    
    viCheckErr( viPrintf (io, "*IDN?"));
    viCheckErr( viRead (io, rdBuffer, BUFFER_SIZE-1, &retCnt));
    rdBuffer[retCnt] = 0;

    checkErr( Ivi_SetValInStringCallback (vi, attributeId, rdBuffer));
    
Error:
    return error;
}
    
/*- AGN8700_ATTR_SPECIFIC_DRIVER_REVISION -*/

static ViStatus _VI_FUNC agn8700AttrDriverRevision_ReadCallback (ViSession vi, 
                                                                  ViSession io,
                                                                  ViConstString channelName, 
                                                                  ViAttr attributeId,
                                                                  const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      driverRevision[256];
    
    
    sprintf (driverRevision, 
             "Driver: agn8700 %d.%d, Compiler: %s %3.2f, "
             "Components: IVIEngine %.2f, VISA-Spec %.2f",
             AGN8700_MAJOR_VERSION, AGN8700_MINOR_VERSION, 
             IVI_COMPILER_NAME, IVI_COMPILER_VER_NUM, 
             IVI_ENGINE_MAJOR_VERSION + IVI_ENGINE_MINOR_VERSION/1000.0, 
             Ivi_ConvertVISAVer(VI_SPEC_VERSION));

    checkErr( Ivi_SetValInStringCallback (vi, attributeId, driverRevision));    
Error:
    return error;
}


/*- AGN8700_ATTR_INSTRUMENT_FIRMWARE_REVISION -*/

static ViStatus _VI_FUNC agn8700AttrFirmwareRevision_ReadCallback (ViSession vi, 
                                                                    ViSession io,
                                                                    ViConstString channelName, 
                                                                    ViAttr attributeId,
                                                                    const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      idQ[BUFFER_SIZE], rdBuffer[BUFFER_SIZE];
    
    checkErr( Ivi_GetAttributeViString (vi, "", AGN8700_ATTR_ID_QUERY_RESPONSE,
                                        0, BUFFER_SIZE, idQ));

	sscanf (idQ, "%*[^,],%*[^,],%*[^,],%256[^\n]", rdBuffer);

    checkErr( Ivi_SetValInStringCallback (vi, attributeId, rdBuffer));
    
Error:
    return error;
}

/*- AGN8700_ATTR_INSTRUMENT_MANUFACTURER -*/

static ViStatus _VI_FUNC agn8700AttrInstrumentManufacturer_ReadCallback (ViSession vi, 
                                                                          ViSession io,
                                                                          ViConstString channelName, 
                                                                          ViAttr attributeId,
                                                                          const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];
    ViChar      idQ[BUFFER_SIZE];
    
    checkErr( Ivi_GetAttributeViString (vi, "", AGN8700_ATTR_ID_QUERY_RESPONSE,
                                        0, BUFFER_SIZE, idQ));
    sscanf (idQ, "%256[^,]", rdBuffer);

    checkErr( Ivi_SetValInStringCallback (vi, attributeId, rdBuffer));
    
Error:
    return error;
}

    

/*- AGN8700_ATTR_INSTRUMENT_MODEL -*/

static ViStatus _VI_FUNC agn8700AttrInstrumentModel_ReadCallback (ViSession vi, 
                                                                   ViSession io,
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId,
                                                                   const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];
    ViChar      idQ[BUFFER_SIZE];
    
    checkErr( Ivi_GetAttributeViString (vi, "", AGN8700_ATTR_ID_QUERY_RESPONSE,
                                        0, BUFFER_SIZE, idQ));
    sscanf (idQ, "%*[^,],%256[^,]", rdBuffer);

    checkErr( Ivi_SetValInStringCallback (vi, attributeId, rdBuffer));
    
Error:
    return error;
}

/*- AGN8700_ATTR_CURRENT_LIMIT_BEHAVIOR -*/

static IviRangeTableEntry attrCurrentLimitBehaviorRangeTableEntries[] =
	{
		{AGN8700_VAL_CURRENT_REGULATE, 0, 0, "0", 0},
		{AGN8700_VAL_CURRENT_TRIP, 0, 0, "1", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};
static IviRangeTable attrCurrentLimitBehaviorRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_FALSE,
        VI_FALSE,
        VI_NULL,
        attrCurrentLimitBehaviorRangeTableEntries,
	};


/*- AGN8700_ATTR_TRIGGER_SOURCE -*/

static IviRangeTableEntry attrTriggerSourceRangeTableEntries[] =
	{
		{AGN8700_VAL_SOFTWARE_TRIG, 0, 0, "BUS", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};
static IviRangeTable attrTriggerSourceRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_FALSE,
        VI_FALSE,
        VI_NULL,
        attrTriggerSourceRangeTableEntries,
	};
    

/********************************************************************/


/*****************************************************************************
 * Function: ag_CreateModelTable
 * Purpose:  This function create the model table.
 *****************************************************************************/
static ViStatus ag_CreateModelTable(
	ViSession vi,
	ViInt32		pInstaledModel,
	SInstrumentRangesPtr* pInstrumentModel
)
{
	ViStatus				error = VI_SUCCESS;
	SInstrumentRangesPtr	pinstrument_model = NULL;
	ViInt32					i = 0;

	checkErr( Ivi_Alloc( vi, (ViInt32)sizeof(SInstrumentRanges), (ViAddr*)&pinstrument_model));
	memset(pinstrument_model, 0, sizeof(SInstrumentRanges));

		switch ( pInstaledModel )
		{
			case N8731A:
				pinstrument_model = &gs_N8731Ranges;
				break;
			case N8732A:
				pinstrument_model = &gs_N8732Ranges;
				break;
			case N8733A:
				pinstrument_model = &gs_N8733Ranges;
				break;
			case N8734A:
				pinstrument_model = &gs_N8734Ranges;
				break;
			case N8735A:
				pinstrument_model = &gs_N8735Ranges;
				break;									 
			case N8736A:
				pinstrument_model = &gs_N8736Ranges;
				break;
			case N8737A:
				pinstrument_model = &gs_N8737Ranges;
				break;
			case N8738A:
				pinstrument_model = &gs_N8738Ranges;
				break;
			case N8739A:
				pinstrument_model = &gs_N8739Ranges;
				break;
			case N8740A:
				pinstrument_model = &gs_N8740Ranges;
				break;
			case N8741A:
				pinstrument_model = &gs_N8741Ranges;
				break;
			case N8742A:
				pinstrument_model = &gs_N8742Ranges;
				break;
			case N8754A:
				pinstrument_model = &gs_N8754Ranges;
				break;
			case N8755A:
				pinstrument_model = &gs_N8755Ranges;
				break;
			case N8756A:
				pinstrument_model = &gs_N8756Ranges;
				break;
			case N8757A:
				pinstrument_model = &gs_N8757Ranges;
				break;
			case N8758A:
				pinstrument_model = &gs_N8758Ranges;
				break;
			case N8759A:
				pinstrument_model = &gs_N8759Ranges;
				break;
			case N8760A:
				pinstrument_model = &gs_N8760Ranges;
				break;
			case N8761A:
				pinstrument_model = &gs_N8761Ranges;
				break;
			case N8762A:
				pinstrument_model = &gs_N8762Ranges;
				break;
		}
	
Error:
	*pInstrumentModel = pinstrument_model;
	return error;
}


/*- AGN8700_ATTR_OVP_LIMIT -*/
/*- Shared Write callback agn8700ViReal64_WriteCallback -*/
/*- Shared Read callback agn8700ViReal64_ReadCallback -*/
static ViStatus _VI_FUNC agn8700AttrOvpLimit_CheckCallback (ViSession vi,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	SInstrumentRangesPtr	pinstrument_model = VI_NULL;
	ViReal64	maximum;
	ViReal64	minimum = 0.0;

	checkErr( Ivi_GetAttributeViAddr( vi,
									  VI_NULL,
									  AGN8700_ATTR_INSTR_INSTRUMENT_MODULES_RANGES,
									  0, (ViAddr*)&pinstrument_model));

	checkErr( Ivi_GetAttributeViReal64( vi,
									  VI_NULL,
									  AGN8700_ATTR_VOLTAGE_LEVEL,
									  0, &minimum));
	
	minimum = minimum/1.05; 
	
	minimum = minimum > pinstrument_model->MinOVP ? minimum : pinstrument_model->MinOVP;           	
	maximum = pinstrument_model->MaxOVP;

	if ( value < minimum || value > maximum )
		viCheckErr( IVI_ERROR_INVALID_VALUE);

Error:
	return error;
}

/*- AGN8700_ATTR_CURRENT_LIMIT -*/
/*- Shared Write callback agn8700ViReal64_WriteCallback -*/
/*- Shared Read callback agn8700ViReal64_ReadCallback -*/
static ViStatus _VI_FUNC agn8700AttrCurrentLimit_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	SInstrumentRangesPtr	pinstrument_model = VI_NULL;
	ViReal64	minimum;
	ViReal64	maximum;

	checkErr( Ivi_GetAttributeViAddr( vi,
									  VI_NULL,
									  AGN8700_ATTR_INSTR_INSTRUMENT_MODULES_RANGES,
									  0, (ViAddr*)&pinstrument_model));

	minimum = pinstrument_model->MinCurrent;
	maximum = pinstrument_model->MaxCurrent;

	if ( value < minimum || value > maximum )
		viCheckErr( IVI_ERROR_INVALID_VALUE);

Error:
	return error;
}

/*- AGN8700_ATTR_VOLTAGE_LEVEL -*/
/*- Shared Write callback agn8700ViReal64_WriteCallback -*/
/*- Shared Read callback agn8700ViReal64_ReadCallback -*/
static ViStatus _VI_FUNC agn8700AttrVoltageLevel_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	SInstrumentRangesPtr	pinstrument_model = VI_NULL;
	ViReal64	minimum;
	ViReal64	maximum;

	checkErr( Ivi_GetAttributeViAddr( vi,
									  VI_NULL,
									  AGN8700_ATTR_INSTR_INSTRUMENT_MODULES_RANGES,
									  0, (ViAddr*)&pinstrument_model));

	checkErr( Ivi_GetAttributeViReal64( vi,
									  VI_NULL,
									  AGN8700_ATTR_VOLTAGE_LIMIT_LOW,
									  0, &minimum));
	
	checkErr( Ivi_GetAttributeViReal64( vi,
									  VI_NULL,
									  AGN8700_ATTR_OVP_LIMIT,
									  0, &maximum));

	minimum = minimum/0.95;
	maximum = maximum/1.05;
	
	
	minimum = minimum > pinstrument_model->MinVoltage ? minimum : pinstrument_model->MinVoltage;
	maximum = maximum < pinstrument_model->MaxVoltage ? maximum : pinstrument_model->MaxVoltage;

	if ( value < minimum || value > maximum )
		viCheckErr( IVI_ERROR_INVALID_VALUE);

Error:
	return error;
}

/*- AGN8700_ATTR_VOLTAGE_LIMIT_LOW -*/
/*- Shared Write callback agn8700ViReal64_WriteCallback -*/
/*- Shared Read callback agn8700ViReal64_ReadCallback -*/
static ViStatus _VI_FUNC agn8700AttrVoltageLimitLow_CheckCallback (ViSession vi,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	SInstrumentRangesPtr	pinstrument_model = VI_NULL;
	ViReal64	minimum;
	ViReal64	maximum;

	checkErr( Ivi_GetAttributeViAddr( vi,
									  VI_NULL,
									  AGN8700_ATTR_INSTR_INSTRUMENT_MODULES_RANGES,
									  0, (ViAddr*)&pinstrument_model));

	checkErr( Ivi_GetAttributeViReal64( vi,
									  VI_NULL,
									  AGN8700_ATTR_VOLTAGE_LEVEL,
									  0, &maximum));

	maximum = maximum/0.95;
	
	maximum = maximum < pinstrument_model->MaxLowVoltage ? maximum : pinstrument_model->MaxLowVoltage;
	minimum = pinstrument_model->MinLowVoltage;
	
	if ( value < minimum || value > maximum )
		viCheckErr( IVI_ERROR_INVALID_VALUE);

Error:
	return error;
}

/*- AGN8700_ATTR_TRIGGERED_CURRENT_LIMIT -*/
/*- Shared Write callback agn8700ViReal64_WriteCallback -*/
/*- Shared Read callback agn8700ViReal64_ReadCallback -*/
static ViStatus _VI_FUNC agn8700AttrTriggeredCurrentLimit_CheckCallback (ViSession vi,
                                                                         ViConstString channelName,
                                                                         ViAttr attributeId,
                                                                         ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	SInstrumentRangesPtr	pinstrument_model = VI_NULL;
	ViReal64	minimum;
	ViReal64	maximum;

	checkErr( Ivi_GetAttributeViAddr( vi,
									  VI_NULL,
									  AGN8700_ATTR_INSTR_INSTRUMENT_MODULES_RANGES,
									  0, (ViAddr*)&pinstrument_model));

	minimum = pinstrument_model->MinCurrent;
	maximum = pinstrument_model->MaxCurrent;

	if ( value < minimum || value > maximum )
		viCheckErr( IVI_ERROR_INVALID_VALUE);

Error:
	return error;
}

/*- AGN8700_ATTR_OVP_ENABLED -*/
static ViStatus _VI_FUNC agn8700AttrOvpEnabled_CheckCallback (ViSession vi,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViBoolean value)
{
	ViStatus	error = VI_SUCCESS;

	if(!value) {
		viCheckErr( IVI_ERROR_INVALID_VALUE); 
	}
	
Error:
	return error;
}

/*****************************************************************************
 * Function: agn8700_InitAttributes                                                  
 * Purpose:  This function adds attributes to the IVI session, initializes   
 *           instrument attributes, and sets attribute invalidation          
 *           dependencies.                                                   
 *****************************************************************************/
static ViStatus agn8700_InitAttributes (ViSession vi,ViInt32 pInstaledModel)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     flags = 0;
    SInstrumentRangesPtr	pinstrument_model = VI_NULL;

    checkErr( ag_CreateModelTable( vi, pInstaledModel,  &pinstrument_model));
 

        /*- Initialize instrument attributes --------------------------------*/
	
    checkErr( Ivi_SetAttrReadCallbackViString (vi, AGN8700_ATTR_SPECIFIC_DRIVER_REVISION,
                                               agn8700AttrDriverRevision_ReadCallback));
    checkErr( Ivi_SetAttributeViInt32 (vi, "", AGN8700_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION,
                                       0, AGN8700_CLASS_SPEC_MAJOR_VERSION));
    checkErr( Ivi_SetAttributeViInt32 (vi, "", AGN8700_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION,
                                       0, AGN8700_CLASS_SPEC_MINOR_VERSION));
    checkErr( Ivi_SetAttributeViString (vi, "", AGN8700_ATTR_SUPPORTED_INSTRUMENT_MODELS,
                                        0, AGN8700_SUPPORTED_INSTRUMENT_MODELS));


    checkErr( Ivi_GetAttributeFlags (vi, AGN8700_ATTR_INSTRUMENT_FIRMWARE_REVISION, &flags));
    checkErr( Ivi_SetAttributeFlags (vi, AGN8700_ATTR_INSTRUMENT_FIRMWARE_REVISION, 
                                     flags | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION));
    checkErr( Ivi_SetAttrReadCallbackViString (vi, AGN8700_ATTR_INSTRUMENT_FIRMWARE_REVISION,
                                               agn8700AttrFirmwareRevision_ReadCallback));

	checkErr( Ivi_GetAttributeFlags (vi, AGN8700_ATTR_INSTRUMENT_MANUFACTURER, &flags));
	checkErr( Ivi_SetAttributeFlags (vi, AGN8700_ATTR_INSTRUMENT_MANUFACTURER, 
	                                 flags | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION));
	checkErr( Ivi_SetAttrReadCallbackViString (vi, AGN8700_ATTR_INSTRUMENT_MANUFACTURER,
	                                           agn8700AttrInstrumentManufacturer_ReadCallback));

	checkErr( Ivi_GetAttributeFlags (vi, AGN8700_ATTR_INSTRUMENT_MODEL, &flags));
	checkErr( Ivi_SetAttributeFlags (vi, AGN8700_ATTR_INSTRUMENT_MODEL, 
	                                 flags | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION));
	checkErr( Ivi_SetAttrReadCallbackViString (vi, AGN8700_ATTR_INSTRUMENT_MODEL,
	                                           agn8700AttrInstrumentModel_ReadCallback));

    checkErr( Ivi_SetAttributeViString (vi, "", AGN8700_ATTR_SPECIFIC_DRIVER_VENDOR,
                                        0, AGN8700_DRIVER_VENDOR));
    checkErr( Ivi_SetAttributeViString (vi, "", AGN8700_ATTR_SPECIFIC_DRIVER_DESCRIPTION,
                                        0, AGN8700_DRIVER_DESCRIPTION));
    checkErr( Ivi_SetAttributeViAddr (vi, VI_NULL, IVI_ATTR_OPC_CALLBACK, 0,
                                      agn8700_WaitForOPCCallback));
    checkErr( Ivi_SetAttributeViAddr (vi, VI_NULL, IVI_ATTR_CHECK_STATUS_CALLBACK, 0,
                                      agn8700_CheckStatusCallback));

	checkErr( Ivi_SetAttributeViString (vi, "", AGN8700_ATTR_GROUP_CAPABILITIES, 0,
	                                    "IviDCPwrBase,"
	                                    "IviDCPwrTrigger,"
	                                    "IviDCPwrSoftwareTrigger,"
	                                    "IviDCPwrMeasurement"));
    
        /*- Add instrument-specific attributes ------------------------------*/            
	checkErr (Ivi_AddAttributeViBoolean (vi, AGN8700_ATTR_OUTPUT_ENABLED,
	                                     "AGN8700_ATTR_OUTPUT_ENABLED", VI_FALSE,
	                                     IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE,
	                                     agn8700ViBoolean_ReadCallback,
	                                     agn8700ViBoolean_WriteCallback));
	
	checkErr (Ivi_AddAttributeViReal64 (vi, AGN8700_ATTR_VOLTAGE_LEVEL,
	                                    "AGN8700_ATTR_VOLTAGE_LEVEL", 0.02,
	                                    IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE,
	                                    agn8700ViReal64_ReadCallback,
	                                    agn8700ViReal64_WriteCallback, VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, AGN8700_ATTR_VOLTAGE_LEVEL,
	                                            agn8700AttrVoltageLevel_CheckCallback));
	                                     
	checkErr (Ivi_AddAttributeViBoolean (vi, AGN8700_ATTR_OVP_ENABLED,
	                                     "AGN8700_ATTR_OVP_ENABLED", VI_TRUE,
	                                     IVI_VAL_MULTI_CHANNEL, VI_NULL, VI_NULL));
	checkErr (Ivi_SetAttrCheckCallbackViBoolean (vi, AGN8700_ATTR_OVP_ENABLED,
	                                             agn8700AttrOvpEnabled_CheckCallback));
	
	checkErr (Ivi_AddAttributeViReal64 (vi, AGN8700_ATTR_OVP_LIMIT,
	                                    "AGN8700_ATTR_OVP_LIMIT", 5.0,
	                                    IVI_VAL_MULTI_CHANNEL,
	                                    agn8700ViReal64_ReadCallback,
	                                    agn8700ViReal64_WriteCallback, VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, AGN8700_ATTR_OVP_LIMIT,
	                                            agn8700AttrOvpLimit_CheckCallback));
	
	checkErr (Ivi_AddAttributeViInt32 (vi, AGN8700_ATTR_CURRENT_LIMIT_BEHAVIOR,
	                                   "AGN8700_ATTR_CURRENT_LIMIT_BEHAVIOR",
	                                   AGN8700_VAL_CURRENT_REGULATE,
	                                   IVI_VAL_MULTI_CHANNEL,
	                                   agn8700Enum_ReadCallback,
	                                   agn8700Enum_WriteCallback,
	                                   &attrCurrentLimitBehaviorRangeTable));
	
	checkErr (Ivi_AddAttributeViReal64 (vi, AGN8700_ATTR_CURRENT_LIMIT,
	                                    "AGN8700_ATTR_CURRENT_LIMIT", 0.0,
	                                    IVI_VAL_MULTI_CHANNEL | IVI_VAL_NEVER_CACHE,
	                                    agn8700ViReal64_ReadCallback,
	                                    agn8700ViReal64_WriteCallback, VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, AGN8700_ATTR_CURRENT_LIMIT,
	                                            agn8700AttrCurrentLimit_CheckCallback));
	
	checkErr (Ivi_AddAttributeViInt32 (vi, AGN8700_ATTR_TRIGGER_SOURCE,
	                                   "AGN8700_ATTR_TRIGGER_SOURCE",
	                                   AGN8700_VAL_SOFTWARE_TRIG,
	                                   IVI_VAL_MULTI_CHANNEL,
	                                   agn8700Enum_ReadCallback,
	                                   agn8700Enum_WriteCallback,
	                                   &attrTriggerSourceRangeTable));
	  
	checkErr (Ivi_AddAttributeViReal64 (vi, AGN8700_ATTR_TRIGGERED_VOLTAGE_LEVEL,
	                                    "AGN8700_ATTR_TRIGGERED_VOLTAGE_LEVEL", 0.02,
	                                    IVI_VAL_MULTI_CHANNEL,
	                                    agn8700ViReal64_ReadCallback,
	                                    agn8700ViReal64_WriteCallback, VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi,
	                                            AGN8700_ATTR_TRIGGERED_VOLTAGE_LEVEL,
	                                            agn8700AttrVoltageLevel_CheckCallback));
	
	checkErr (Ivi_AddAttributeViReal64 (vi, AGN8700_ATTR_TRIGGERED_CURRENT_LIMIT,
	                                    "AGN8700_ATTR_TRIGGERED_CURRENT_LIMIT", 0.0,
	                                    IVI_VAL_MULTI_CHANNEL,
	                                    agn8700ViReal64_ReadCallback,
	                                    agn8700ViReal64_WriteCallback, VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi,
	                                            AGN8700_ATTR_TRIGGERED_CURRENT_LIMIT,
	                                            agn8700AttrTriggeredCurrentLimit_CheckCallback));
	
	checkErr (Ivi_AddAttributeViString (vi, AGN8700_ATTR_ID_QUERY_RESPONSE,
	                                    "AGN8700_ATTR_ID_QUERY_RESPONSE",
	                                    "Agilent Technologies,N8731A,US09A02321,A.04.00,REV:AP1",
	                                    IVI_VAL_NOT_USER_WRITABLE,
	                                    agn8700AttrIdQueryResponse_ReadCallback,
	                                    VI_NULL));
	                                           
    checkErr( Ivi_AddAttributeViInt32 (vi, AGN8700_ATTR_OPC_TIMEOUT,
                                       "AGN8700_ATTR_OPC_TIMEOUT",
                                       5000, IVI_VAL_HIDDEN | IVI_VAL_DONT_CHECK_STATUS,
                                       VI_NULL, VI_NULL, VI_NULL)); 
    checkErr( Ivi_AddAttributeViInt32 (vi,
                                       AGN8700_ATTR_VISA_RM_SESSION,
                                       "AGN8700_ATTR_VISA_RM_SESSION",
                                       VI_NULL,
                                       IVI_VAL_HIDDEN | IVI_VAL_DONT_CHECK_STATUS,
                                       VI_NULL, VI_NULL, VI_NULL));
    checkErr( Ivi_AddAttributeViInt32 (vi, AGN8700_ATTR_OPC_CALLBACK,
                                       "AGN8700_ATTR_OPC_CALLBACK",
                                       VI_NULL,
                                       IVI_VAL_HIDDEN | IVI_VAL_DONT_CHECK_STATUS,
                                       VI_NULL, VI_NULL, VI_NULL));
    checkErr( Ivi_AddAttributeViInt32 (vi,
                                       AGN8700_ATTR_CHECK_STATUS_CALLBACK,
                                       "AGN8700_ATTR_CHECK_STATUS_CALLBACK",
                                       VI_NULL,
                                       IVI_VAL_HIDDEN | IVI_VAL_DONT_CHECK_STATUS,
                                       VI_NULL, VI_NULL, VI_NULL));
    checkErr( Ivi_AddAttributeViInt32 (vi,
                                       AGN8700_ATTR_USER_INTERCHANGE_CHECK_CALLBACK,
                                       "AGN8700_ATTR_USER_INTERCHANGE_CHECK_CALLBACK",
                                       VI_NULL,
                                       IVI_VAL_HIDDEN | IVI_VAL_DONT_CHECK_STATUS,
                                       VI_NULL, VI_NULL, VI_NULL));
	checkErr (Ivi_AddAttributeViBoolean (vi, AGN8700_ATTR_INITIATE_CONTINUOUS,
	                                     "AGN8700_ATTR_INITIATE_CONTINUOUS",
	                                     VI_FALSE, IVI_VAL_MULTI_CHANNEL,
	                                     agn8700ViBoolean_ReadCallback,
	                                     agn8700ViBoolean_WriteCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, AGN8700_ATTR_VOLTAGE_LIMIT_LOW,
	                                    "AGN8700_ATTR_VOLTAGE_LIMIT_LOW", 0.0,
	                                    IVI_VAL_MULTI_CHANNEL,
	                                    agn8700ViReal64_ReadCallback,
	                                    agn8700ViReal64_WriteCallback, VI_NULL, 0));
	checkErr (Ivi_AddAttributeViAddr (vi,
	                                  AGN8700_ATTR_INSTR_INSTRUMENT_MODULES_RANGES,
	                                  "AGN8700_ATTR_INSTR_INSTRUMENT_MODULES_RANGES",
	                                  pinstrument_model, IVI_VAL_HIDDEN, VI_NULL,
	                                  VI_NULL));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, AGN8700_ATTR_VOLTAGE_LIMIT_LOW,
	                                            agn8700AttrVoltageLimitLow_CheckCallback));
                                       
        /*- Setup attribute invalidations -----------------------------------*/            
	 

Error:
    return error;
}

/*****************************************************************************
 *------------------- End Instrument Driver Source Code ---------------------*
 *****************************************************************************/
