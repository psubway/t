/***************************************************************************** 
 *		    Copyright 2015, ChromaATE.  All Rights Reserved.    			 * 
 *****************************************************************************/
/*****************************************************************************
 *  Chroma 63200A Instrument Driver
 *  LabWindows/CVI Instrument Driver
 *  Original Release: 
 *  By: Ed, ChromaATE
 *      PH. +8863-327-9999   Fax
 *
 *  Modification History:
 *
 *		2015/06/01 - Instrument Driver Created. 
 *
 *		2015/07/07 - [Ed]Fix range of CZ Mode.
 *
 *		2015/07/24 - [Ed]Combined Load and Load Status functions.
 *
 *
 *
 *****************************************************************************/
#include <utility.h>
#include <ansi_c.h>
#include "chr63200A.h"
#include <string.h>
#include <stdio.h>  
#include <formatio.h>

/*****************************************************************************
 *--------------------- Hidden Attribute Declarations -----------------------*
 *****************************************************************************/

#define CHR63200A_ATTR_OPC_TIMEOUT      				(IVI_SPECIFIC_PRIVATE_ATTR_BASE + 1L)   /* ViInt32 */
#define CHR63200A_ATTR_USE_SPECIFIC_SIMULATION  		(IVI_INHERENT_ATTR_BASE + 23L) 			/* ViBoolean */
#define CHR63200A_ATTR_IO_SESSION   					(IVI_INHERENT_ATTR_BASE + 322L)         /* ViSession */
#define CHR63200A_ATTR_OPC_CALLBACK     				(IVI_SPECIFIC_PRIVATE_ATTR_BASE + 6L)   /* ViAddr */
#define CHR63200A_ATTR_CHECK_STATUS_CALLBACK    		(IVI_SPECIFIC_PRIVATE_ATTR_BASE + 7L)   /* ViAddr */
#define CHR63200A_ATTR_USER_INTERCHANGE_CHECK_CALLBACK  (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 8L)   /* ViAddr */

#define CHR63200A_ATTR_PROG_TYPE            (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 109L)				/* ViInt32 */
#define CHR63200A_ATTR_PROG_CHAIN           (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 110L)				/* ViInt32 */
#define CHR63200A_ATTR_PROG_REPEAT          (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 111L)				/* ViInt32 */
#define CHR63200A_ATTR_PROG_SEQUENCE_NUM    (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 112L)				/* ViInt32 */
#define CHR63200A_ATTR_PROG_LIST_VALUE      (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 113L)				/* ViReal64 */
#define CHR63200A_ATTR_PROG_SEQUENCE_INDEX  (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 114L)				/* ViInt32 */
#define CHR63200A_ATTR_PROG_STEP_START      (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 115L)				/* ViReal64 */
#define CHR63200A_ATTR_PROG_STEP_END        (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 116L)				/* ViReal64 */
#define CHR63200A_ATTR_PROG_RISE            (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 117L)				/* ViReal64 */
#define CHR63200A_ATTR_PROG_FALL            (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 118L)				/* ViReal64 */
#define CHR63200A_ATTR_PROG_DWELL           (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 119L)				/* ViReal64 */
#define CHR63200A_ATTR_PROG_VH              (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 120L)				/* ViReal64 */
#define CHR63200A_ATTR_PROG_VL              (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 121L)				/* ViReal64 */
#define CHR63200A_ATTR_PROG_IH      		(IVI_SPECIFIC_PRIVATE_ATTR_BASE + 122L)				/* ViReal64 */
#define CHR63200A_ATTR_PROG_IL      		(IVI_SPECIFIC_PRIVATE_ATTR_BASE + 123L)				/* ViReal64 */
#define CHR63200A_ATTR_PROG_PH              (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 124L)				/* ViReal64 */
#define CHR63200A_ATTR_PROG_PL              (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 125L)				/* ViReal64 */
#define CHR63200A_ATTR_PROG_DELAY           (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 126L)				/* ViReal64 */
#define CHR63200A_ATTR_PROG_TRIG_MODE       (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 127L)				/* ViInt32 */ 
#define CHR63200A_ATTR_PROG_RUN_MODE        (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 128L)				/* ViInt32 */ 
#define CHR63200A_ATTR_PROG_MODE_RANGE      (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 129L)				/* ViInt32 */ 


/*****************************************************************************
 *---------------------------- Useful Macros --------------------------------*
 *****************************************************************************/
    /*- I/O buffer size -----------------------------------------------------*/
#define BUFFER_SIZE                             512L        
#define BUFFER_SIZE_Max                         480000L
#define BUFFER_SIZE_Digi                        508192L
#define DELAY_TIME								0.05
    /*- 488.2 Event Status Register (ESR) Bits ------------------------------*/

#define IEEE_488_2_QUERY_ERROR_BIT              0x04
#define IEEE_488_2_DEVICE_DEPENDENT_ERROR_BIT   0x08
#define IEEE_488_2_EXECUTION_ERROR_BIT          0x10
#define IEEE_488_2_COMMAND_ERROR_BIT            0x20

	/*-	Time out(mSec)	-----------------------------------------------------*/
#define OPC_TIME                             	100
#define TIME_OUT_VALUE                          2000

static IviRangeTableEntry attrAdvUserWavCleRangeTableEntries[] =
	{
		{CHR63200A_VAL_WAV_CLE_OK, 0, 0, "OK", 0},
		{CHR63200A_VAL_WAV_CLE_ERR, 0, 0, "ERROR", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrAdvUserWavCleRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrAdvUserWavCleRangeTableEntries,
	};

static IviRangeTableEntry attrProgTrigModeRangeTableEntries[] =
	{
		{CHR63200A_VAL_ADV_PROG_SKIP, 0, 0, "SKIP", 0},
		{CHR63200A_VAL_ADV_PROG_AUTO, 0, 0, "AUTO", 0},
		{CHR63200A_VAL_ADV_PROG_MANUAL, 0, 0, "MANUAL", 0},
		{CHR63200A_VAL_ADV_PROG_EXTERNAL, 0, 0, "EXTERNAL", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrProgTrigModeRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrProgTrigModeRangeTableEntries,
	};

static IviRangeTableEntry attrProgRunModeRangeTableEntries[] =
	{
		{CHR63200A_VAL_ADV_PROG_CC, 0, 0, "CC", 0},
		{CHR63200A_VAL_ADV_PROG_CR, 0, 0, "CR", 0},
		{CHR63200A_VAL_ADV_PROG_CV, 0, 0, "CV", 0},
		{CHR63200A_VAL_ADV_PROG_CP, 0, 0, "CP", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrProgRunModeRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrProgRunModeRangeTableEntries,
	};

static IviRangeTableEntry attrProgModeRangeRangeTableEntries[] =
	{
		{CHR63200A_VAL_LOW, 0, 0, "LOW", 0},
		{CHR63200A_VAL_MIDDLE, 0, 0, "MIDDLE", 0},
		{CHR63200A_VAL_HIGH, 0, 0, "HIGH", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrProgModeRangeRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrProgModeRangeRangeTableEntries,
	};

static IviRangeTableEntry attrProgTypeRangeTableEntries[] =
	{
		{CHR63200A_VAL_ADV_PROG_TYPE_LIST, 0, 0, "LIST", 0},
		{CHR63200A_VAL_ADV_PROG_TYPE_STEP, 0, 0, "STEP", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrProgTypeRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrProgTypeRangeTableEntries,
	};

static IviRangeTableEntry attrProgChainRangeTableEntries[] =
	{
		{0, 10, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrProgChainRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrProgChainRangeTableEntries,
	};

static IviRangeTableEntry attrProgRepeatRangeTableEntries[] =
	{
		{0, 9999, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrProgRepeatRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrProgRepeatRangeTableEntries,
	};

static IviRangeTableEntry attrProgSequenceNumRangeTableEntries[] =
	{
		{1, 100, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrProgSequenceNumRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrProgSequenceNumRangeTableEntries,
	};

static IviRangeTableEntry attrAdvUserWavDataStatRangeTableEntries[] =
	{
		{CHR63200A_VAL_UDW_STAT_IDLE, 0, 0, "", 0},
		{CHR63200A_VAL_UDW_STAT_WAIT, 0, 0, "", 0},
		{CHR63200A_VAL_UDW_STAT_FIN, 0, 0, "", 0},
		{CHR63200A_VAL_UDW_STAT_FORMAT, 0, 0, "", 0},
		{CHR63200A_VAL_UDW_STAT_LEN, 0, 0, "", 0},
		{CHR63200A_VAL_UDW_STAT_LIM, 0, 0, "", 0},
		{CHR63200A_VAL_UDW_STAT_CHKSUM, 0, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrAdvUserWavDataStatRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrAdvUserWavDataStatRangeTableEntries,
	};

static IviRangeTableEntry attrAdvUserWavExeStatRangeTableEntries[] =
	{
		{CHR63200A_VAL_UDW_EXE_IDLE, 0, 0, "", 0},
		{CHR63200A_VAL_UDW_EXE_RUN, 0, 0, "", 0},
		{CHR63200A_VAL_UDW_EXE_FIN, 0, 0, "", 0},
		{CHR63200A_VAL_UDW_EXE_STOP, 0, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrAdvUserWavExeStatRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrAdvUserWavExeStatRangeTableEntries,
	};

static IviRangeTableEntry attrSpecUnitRangeTableEntries[] =
	{
		{CHR63200A_VAL_SPEC_PERCENT, 0, 0, "PERCENT", 0},
		{CHR63200A_VAL_SPEC_VAL, 0, 0, "VALUE", 1},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrSpecUnitRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrSpecUnitRangeTableEntries,
	};

static IviRangeTableEntry attrDigTrigRangeTableEntries[] =
	{
		{CHR63200A_VAL_DIGI_TRIG_OFF, 0, 0, "OFF", 0},
		{CHR63200A_VAL_DIGI_TRIG_ON, 0, 0, "ON", 1},
		{CHR63200A_VAL_DIGI_TRIG_IDLE, 0, 0, "IDLE", 101},
		{CHR63200A_VAL_DIGI_TRIG_PRE_TRIG, 0, 0, "PRE_TRIG", 102},
		{CHR63200A_VAL_DIGI_TRIG_WAIT_TRIG, 0, 0, "WAIT_TRIG", 103},
		{CHR63200A_VAL_DIGI_TRIG_POST_TRIG, 0, 0, "POST_TRIG", 104},
		{CHR63200A_VAL_DIGI_TRIG_FINISH_TRIG, 0, 0, "FINISH", 105},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrDigTrigRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrDigTrigRangeTableEntries,
	};

static IviRangeTableEntry attrDigTrigSourRangeTableEntries[] =
	{
		{CHR63200A_VAL_DIGI_TRIG_SOUR_LOADON, 0, 0, "LOADON", 0},
		{CHR63200A_VAL_DIGI_TRIG_SOUR_LOADOFF, 0, 0, "LOADOFF", 1},
		{CHR63200A_VAL_DIGI_TRIG_SOUR_TTL, 0, 0, "TTL", 2},
		{CHR63200A_VAL_DIGI_TRIG_SOUR_BUS, 0, 0, "BUS", 3},
		{CHR63200A_VAL_DIGI_TRIG_SOUR_MANUAL, 0, 0, "MANUAL", 4},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrDigTrigSourRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrDigTrigSourRangeTableEntries,
	};

static IviRangeTableEntry attrAdvExtWavModeRangeTableEntries[] =
	{
		{CHR63200A_VAL_EXTM_CC, 0, 0, "CC", 0},
		{CHR63200A_VAL_EXTM_CR, 0, 0, "CR", 1},
		{CHR63200A_VAL_EXTM_CV, 0, 0, "CV", 2},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrAdvExtWavModeRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrAdvExtWavModeRangeTableEntries,
	};

				  
static IviRangeTableEntry attrAdvBattModeRangeTableEntries[] =
	{
		{CHR63200A_VAL_BATT_CC, 0, 0, "CC", 0},
		{CHR63200A_VAL_BATT_CR, 0, 0, "CR", 1},
		{CHR63200A_VAL_BATT_CP, 0, 0, "CP", 2},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrAdvBattModeRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrAdvBattModeRangeTableEntries,
	};
 
static IviRangeTableEntry attrVoltStatResRangeTableEntries[] =
	{
		{CHR63200A_VAL_SLOW, 0, 0, "SLOW", 0},
		{CHR63200A_VAL_NORMAL, 0, 0, "NORMAL", 1},
		{CHR63200A_VAL_FAST, 0, 0, "FAST", 2},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrVoltStatResRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrVoltStatResRangeTableEntries,
	};

static IviRangeTableEntry attrCurrDynVrngRangeTableEntries[] =
	{
		{CHR63200A_VAL_LOW, 0, 0, "LOW", 0},
		{CHR63200A_VAL_MIDDLE, 0, 0, "MIDDLE", 1},
		{CHR63200A_VAL_HIGH, 0, 0, "HIGH", 2},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrCurrDynVrngRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrCurrDynVrngRangeTableEntries,
	};


static IviRangeTableEntry attrCurrStatVrngRangeTableEntries[] =
	{
		{CHR63200A_VAL_LOW, 0, 0, "LOW", 0},
		{CHR63200A_VAL_MIDDLE, 0, 0, "MIDDLE", 1},
		{CHR63200A_VAL_HIGH, 0, 0, "HIGH", 2},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrCurrStatVrngRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrCurrStatVrngRangeTableEntries,
	};

static IviRangeTableEntry attrDioOut1RangeTableEntries[] =
	{
		{CHR63200A_VAL_OFF, 0, 0, "OFF", 0},
		{CHR63200A_VAL_ON, 0, 0, "ON", 1},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrDioOut1RangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrDioOut1RangeTableEntries,
	};

static IviRangeTableEntry attrConfDioOut1RangeTableEntries[] =
	{
		{0, 7, CHR63200A_VAL_NONE, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrConfDioOut1RangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrConfDioOut1RangeTableEntries,
	};

static IviRangeTableEntry attrConfSounRangeTableEntries[] =
	{
		{CHR63200A_VAL_OFF, 0, 0, "OFF", 0},
		{CHR63200A_VAL_ON, 0, 0, "ON", 1},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrConfSounRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrConfSounRangeTableEntries,
	};

static IviRangeTableEntry attrConfDioIn1RangeTableEntries[] =
	{
		{CHR63200A_VAL_NONE, 0, 0, "0", 0},
		{CHR63200A_VAL_EXT, 0, 0, "1", 1},
		{CHR63200A_VAL_REM, 0, 0, "2", 2},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrConfDioIn1RangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrConfDioIn1RangeTableEntries,
	};

static IviRangeTableEntry attrConfShorKeyRangeTableEntries[] =
	{
		{CHR63200A_VAL_HOLD, 0, 0, "HOLD", 0},
		{CHR63200A_VAL_TOGGLE, 0, 0, "TOGGLE", 1},
		{CHR63200A_VAL_DISABLE, 0, 0, "DISABLE", 2},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrConfShorKeyRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrConfShorKeyRangeTableEntries,
	};

static IviRangeTableEntry attrConfEntKeyRangeTableEntries[] =
	{
		{CHR63200A_VAL_NEXT, 0, 0, "NEXT", 0},
		{CHR63200A_VAL_FIXED, 0, 0, "FIXED", 1},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrConfEntKeyRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrConfEntKeyRangeTableEntries,
	};

static IviRangeTableEntry attrConfAutoOnRangeTableEntries[] =
	{
		{CHR63200A_VAL_OFF, 0, 0, "OFF", 0},
		{CHR63200A_VAL_ON, 0, 0, "ON", 1},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrConfAutoOnRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrConfAutoOnRangeTableEntries,
	};

static IviRangeTableEntry attrConfParaModeRangeTableEntries[] =
	{
		{CHR63200A_VAL_NONE, 0, 0, "NONE", 0},
		{CHR63200A_VAL_MASTER, 0, 0, "MASTER", 1},
		{CHR63200A_VAL_SLAVE, 0, 0, "SLAVE", 2},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrConfParaModeRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrConfParaModeRangeTableEntries,
	};

static IviRangeTableEntry attrConfParaInitRangeTableEntries[] =
	{
		{CHR63200A_VAL_OFF, 0, 0, "OFF", 0},
		{CHR63200A_VAL_ON, 0, 0, "ON", 1},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrConfParaInitRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrConfParaInitRangeTableEntries,
	};

static IviRangeTableEntry attrConfSyncModeRangeTableEntries[] =
	{
		{CHR63200A_VAL_NONE, 0, 0, "NONE", 0},
		{CHR63200A_VAL_MASTER, 0, 0, "MASTER", 1},
		{CHR63200A_VAL_SLAVE, 0, 0, "SLAVE", 2},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrConfSyncModeRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrConfSyncModeRangeTableEntries,
	};
	  
static IviRangeTableEntry attrConfVoltSignRangeTableEntries[] =
	{
		{CHR63200A_VAL_PLUS, 0, 0, "PLUS", 0},
		{CHR63200A_VAL_MINUS, 0, 0, "MINUS", 1},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrConfVoltSignRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrConfVoltSignRangeTableEntries,
	};

static IviRangeTableEntry attrConfVoltLatcRangeTableEntries[] =
	{
		{CHR63200A_VAL_OFF, 0, 0, "OFF", 0},
		{CHR63200A_VAL_ON, 0, 0, "ON", 1},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrConfVoltLatcRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrConfVoltLatcRangeTableEntries,
	};
			
static IviRangeTableEntry attrConfVoltRangRangeTableEntries[] =
	{
		{CHR63200A_VAL_LOW, 0, 0, "LOW", 0},
		{CHR63200A_VAL_MIDDLE, 0, 0, "MIDDLE", 1},
		{CHR63200A_VAL_HIGH, 0, 0, "HIGH", 2},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrConfVoltRangRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrConfVoltRangRangeTableEntries,
	};

static IviRangeTableEntry attrLoadShorRangeTableEntries[] =
	{
		{CHR63200A_VAL_OFF, 0, 0, "OFF", 0},
		{CHR63200A_VAL_ON, 0, 0, "ON", 1},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrLoadShorRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrLoadShorRangeTableEntries,
	};

static IviRangeTableEntry attrLoadProtRangeTableEntries[] =
	{
		{0, 65535, 0, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrLoadProtRangeTable =
	{
		IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrLoadProtRangeTableEntries,
	};

static IviRangeTableEntry attrLoadRangeTableEntries[] =
	{
		{CHR63200A_VAL_OFF, 0, 0, "OFF", 0},
		{CHR63200A_VAL_ON, 0, 0, "ON", 1},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable attrLoadRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrLoadRangeTableEntries,
	};
   
static IviRangeTableEntry UDWDRangeTableEntries[] =
	{
		{1, 10, CHR63200A_VAL_UDWD_WAVEFORM, "", 0},
		{0.00001, 20, CHR63200A_VAL_UDWD_INTERVAL, "", 0},
		{0, 100000, CHR63200A_VAL_UDWD_REPEAT, "", 0},
		{0, 10, CHR63200A_VAL_UDWD_CHAIN, "", 0},
		{0, 1, CHR63200A_VAL_UDWD_INTERPOLATION, "", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable UDWDRangeTable =
	{
		IVI_VAL_COERCED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        UDWDRangeTableEntries,
	};

static IviRangeTableEntry OperationalModeRangeTableEntries[] =
	{
		{CHR63200A_VAL_MOD_CCL, 0, 0, "CCL", 0},
		{CHR63200A_VAL_MOD_CCM, 0, 0, "CCM", 0},
		{CHR63200A_VAL_MOD_CCH, 0, 0, "CCH", 0},
		{CHR63200A_VAL_MOD_CRL, 0, 0, "CRL", 0},
		{CHR63200A_VAL_MOD_CRM, 0, 0, "CRM", 0},
		{CHR63200A_VAL_MOD_CRH, 0, 0, "CRH", 0},
		{CHR63200A_VAL_MOD_CVL, 0, 0, "CVL", 0},
		{CHR63200A_VAL_MOD_CVM, 0, 0, "CVM", 0},
		{CHR63200A_VAL_MOD_CVH, 0, 0, "CVH", 0},
		{CHR63200A_VAL_MOD_CPL, 0, 0, "CPL", 0},
		{CHR63200A_VAL_MOD_CPM, 0, 0, "CPM", 0},
		{CHR63200A_VAL_MOD_CPH, 0, 0, "CPH", 0},
		{CHR63200A_VAL_MOD_CCDL, 0, 0, "CCDL", 0},
		{CHR63200A_VAL_MOD_CCDM, 0, 0, "CCDM", 0},
		{CHR63200A_VAL_MOD_CCDH, 0, 0, "CCDH", 0},
		{CHR63200A_VAL_MOD_CRDL, 0, 0, "CRDL", 0},
		{CHR63200A_VAL_MOD_CRDM, 0, 0, "CRDM", 0},
		{CHR63200A_VAL_MOD_CRDH, 0, 0, "CRDH", 0},
		{CHR63200A_VAL_MOD_BATL, 0, 0, "BATL", 0},
		{CHR63200A_VAL_MOD_BATM, 0, 0, "BATM", 0},
		{CHR63200A_VAL_MOD_BATH, 0, 0, "BATH", 0},
		{CHR63200A_VAL_MOD_SWDL, 0, 0, "SWDL", 0},
		{CHR63200A_VAL_MOD_SWDM, 0, 0, "SWDM", 0},
		{CHR63200A_VAL_MOD_SWDH, 0, 0, "SWDH", 0},
		{CHR63200A_VAL_MOD_OCPL, 0, 0, "OCPL", 0},
		{CHR63200A_VAL_MOD_OCPM, 0, 0, "OCPM", 0},
		{CHR63200A_VAL_MOD_OCPH, 0, 0, "OCPH", 0},
		{CHR63200A_VAL_MOD_CCSL, 0, 0, "CCSL", 0},
		{CHR63200A_VAL_MOD_CCSM, 0, 0, "CCSM", 0},
		{CHR63200A_VAL_MOD_CCSH, 0, 0, "CCSH", 0},
		{CHR63200A_VAL_MOD_CZL, 0, 0, "CZL", 0},
		{CHR63200A_VAL_MOD_CZM, 0, 0, "CZM", 0},
		{CHR63200A_VAL_MOD_CZH, 0, 0, "CZH", 0},
		{CHR63200A_VAL_MOD_CVCC, 0, 0, "CVCC", 0},
		{CHR63200A_VAL_MOD_CRCC, 0, 0, "CRCC", 0},
		{CHR63200A_VAL_MOD_CVCR, 0, 0, "CVCR", 0},
		{CHR63200A_VAL_MOD_AUTO, 0, 0, "AUTO", 0},
		{CHR63200A_VAL_MOD_PROG, 0, 0, "PROG", 0},
		{CHR63200A_VAL_MOD_MPPTL, 0, 0, "MPPTL", 0},
		{CHR63200A_VAL_MOD_MPPTM, 0, 0, "MPPTM", 0},
		{CHR63200A_VAL_MOD_MPPTH, 0, 0, "MPPTH", 0},
		{CHR63200A_VAL_MOD_UDWL, 0, 0, "UDWL", 0},
		{CHR63200A_VAL_MOD_UDWM, 0, 0, "UDWM", 0},
		{CHR63200A_VAL_MOD_UDWH, 0, 0, "UDWH", 0},
		{CHR63200A_VAL_MOD_EXTL, 0, 0, "EXTL", 0},
		{CHR63200A_VAL_MOD_EXTM, 0, 0, "EXTM", 0},
		{CHR63200A_VAL_MOD_EXTH, 0, 0, "EXTH", 0},
		{IVI_RANGE_TABLE_LAST_ENTRY}
	};

static IviRangeTable OperationalModeRangeTable =
	{
		IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        OperationalModeRangeTableEntries,
	};

void chr63200A_ConvertWFD(ViReal64 fLimit,ViChar szValue[],ViUInt16 uiWFDRtn[], ViUInt32 *iPointCnt)
{
	double	fDataVal=0.0;
	char 	szTmp[32];
	int 	i=0;
	
	while(szValue!=VI_NULL)
	{
		Scan(szValue,"%s>%s[xt44]",szTmp);
		fDataVal = atof(szTmp);
		uiWFDRtn[i] = (ViUInt16)RoundRealToNearestInteger (((pow(2, 16) - 1) * (fDataVal)) / fLimit); 
		i++;
		if (strstr(szValue,",") !=VI_NULL)
			Scan(szValue,"%s>%s[dxt44]%s",szValue);
		else
			break;
	}
	
	*iPointCnt = (ViUInt32)i;
	
	
	return ;
}

/*****************************************************************************
 *-------------- Utility Function Declarations (Non-Exported) ---------------*
 *****************************************************************************/
static ViStatus chr63200A_IviInit (ViRsrc resourceName, ViBoolean IDQuery, 
                                     ViBoolean reset, ViSession vi);
static ViStatus chr63200A_IviClose (ViSession vi);   

static ViStatus chr63200A_InitAttributes (ViSession vi);
static ViStatus chr63200A_DefaultInstrSetup (ViSession openInstrSession);
static ViStatus chr63200A_CheckStatus (ViSession vi);
static ViStatus chr63200A_WaitForOPC (ViSession vi, ViInt32 maxTime);

/*****************************************************************************
 *----------------- Callback Declarations (Non-Exported) --------------------*
 *****************************************************************************/
    /*- Global Session Callbacks --------------------------------------------*/
    
static ViStatus _VI_FUNC chr63200A_CheckStatusCallback (ViSession vi, ViSession io);
static ViStatus _VI_FUNC chr63200A_WaitForOPCCallback (ViSession vi, ViSession io);

    /*- Attribute callbacks -------------------------------------------------*/

static ViStatus _VI_FUNC chr63200AAttrDriverRevision_ReadCallback (ViSession vi,
                                                                  ViSession io, 
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId, 
                                                                  const ViConstString cacheValue);
static ViStatus _VI_FUNC chr63200AAttrInstrumentManufacturer_ReadCallback (ViSession vi, 
                                                                          ViSession io,
                                                                          ViConstString channelName, 
                                                                          ViAttr attributeId,
                                                                          const ViConstString cacheValue);
static ViStatus _VI_FUNC chr63200AAttrInstrumentModel_ReadCallback (ViSession vi, 
                                                                   ViSession io,
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId,
                                                                   const ViConstString cacheValue);

static ViStatus _VI_FUNC chr63200AAttrFirmwareRevision_ReadCallback (ViSession vi, 
                                                                    ViSession io,
                                                                    ViConstString channelName, 
                                                                    ViAttr attributeId,
                                                                    const ViConstString cacheValue);
static ViStatus _VI_FUNC chr63200AAttrIdQueryResponse_ReadCallback (ViSession vi,
                                                                   ViSession io, 
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId, 
                                                                   const ViConstString cacheValue);

static ViStatus _VI_FUNC chr63200AAttrAdvAutoPset_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvAutoPset_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvAutoRset_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvAutoRset_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);
																  
static ViStatus _VI_FUNC chr63200AAttrAdvAutoVset_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvAutoVset_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvBattEndv_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvBattEndv_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvBattFall_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvBattFall_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvBattMode_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvBattMode_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrAdvBattRise_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvBattRise_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);
																	   
static ViStatus _VI_FUNC chr63200AAttrAdvBattTout_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvBattTout_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvBattVal_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvBattVal_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvCrccIset_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvCrccIset_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvCrccRset_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvCrccRset_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);
																			  
static ViStatus _VI_FUNC chr63200AAttrAdvCvccIset_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvCvccIset_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvCvccRes_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvCvccRes_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value);
																	
static ViStatus _VI_FUNC chr63200AAttrAdvCvccVset_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvCvccVset_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);
																	 
static ViStatus _VI_FUNC chr63200AAttrAdvCvcrRset_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvCvcrRset_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);
																	   
static ViStatus _VI_FUNC chr63200AAttrAdvCvcrVset_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvCvcrVset_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvExtWavMode_ReadCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvExtWavMode_WriteCallback (ViSession vi,
                                                                   ViSession io,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrAdvExtWavRng_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvExtWavRng_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrAdvOcpDwel_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvOcpDwel_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvOcpEnd_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvOcpEnd_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvOcpLatc_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvOcpLatc_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrAdvOcpRes_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              const ViConstString cacheValue);
																	  
static ViStatus _VI_FUNC chr63200AAttrAdvOcpSpecH_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvOcpSpecH_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvOcpSpecL_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvOcpSpecL_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvOcpStar_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvOcpStar_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvOcpStep_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvOcpStep_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrAdvOcpTrigVolt_ReadCallback (ViSession vi,
                                                                   ViSession io,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvOcpTrigVolt_WriteCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvOppDwel_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvOppDwel_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvOppEnd_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvOppEnd_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvOppLatc_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvOppLatc_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrAdvOppRes_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              const ViConstString cacheValue);
																 
static ViStatus _VI_FUNC chr63200AAttrAdvOppSpecH_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvOppSpecH_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvOppSpecL_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvOppSpecL_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvOppStar_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvOppStar_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvOppStep_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvOppStep_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrAdvOppTrigVolt_ReadCallback (ViSession vi,
                                                                   ViSession io,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvOppTrigVolt_WriteCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvSineFreq_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvSineFreq_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvSineIac_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvSineIac_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvSineIdc_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvSineIdc_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);
	 
static ViStatus _VI_FUNC chr63200AAttrAdvUserWavCle_ReadCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 *value);
																  
static ViStatus _VI_FUNC chr63200AAttrAdvUserWavDataPoin_ReadCallback (ViSession vi,
                                                                       ViSession io,
                                                                       ViConstString channelName,
                                                                       ViAttr attributeId,
                                                                       const ViConstString cacheValue);

static ViStatus _VI_FUNC chr63200AAttrAdvUserWavDataPoin_WriteCallback (ViSession vi,
                                                                        ViSession io,
                                                                        ViConstString channelName,
                                                                        ViAttr attributeId,
                                                                        ViConstString value);

static ViStatus _VI_FUNC chr63200AAttrAdvUserWavDataStat_ReadCallback (ViSession vi,
                                                                       ViSession io,
                                                                       ViConstString channelName,
                                                                       ViAttr attributeId,
                                                                       ViInt32 *value);
																   
static ViStatus _VI_FUNC chr63200AAttrAdvUserWavData_ReadCallback (ViSession vi,
                                                                   ViSession io,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   const ViConstString cacheValue);

static ViStatus _VI_FUNC chr63200AAttrAdvUserWavData_WriteCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViConstString value);

static ViStatus _VI_FUNC chr63200AAttrAdvUserWavExeStat_ReadCallback (ViSession vi,
                                                                      ViSession io,
                                                                      ViConstString channelName,
                                                                      ViAttr attributeId,
                                                                      ViInt32 *value);
																	 
static ViStatus _VI_FUNC chr63200AAttrAdvUserWavNsel_ReadCallback (ViSession vi,
                                                                   ViSession io,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvUserWavNsel_WriteCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrAdvUserWavRem_ReadCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvAutoIset_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvAutoIset_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrConfAutoOn_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrConfAutoOn_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrConfDioIn1_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrConfDioIn1_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrConfDioIn2_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrConfDioIn2_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrConfDioOut1_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrConfDioOut1_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrConfDioOut2_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrConfDioOut2_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrConfEntKey_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrConfEntKey_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrConfParaInit_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrConfParaInit_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrConfParaMode_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrConfParaMode_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrConfShorKey_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrConfShorKey_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrConfSoun_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrConfSoun_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrConfSyncMode_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrConfSyncMode_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrConfVoltLatc_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrConfVoltLatc_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrConfVoltOff_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrConfVoltOff_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrConfVoltOn_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrConfVoltOn_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);
																	
static ViStatus _VI_FUNC chr63200AAttrConfVoltSign_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrConfVoltSign_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrConfWind_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrConfWind_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrDynFall_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrCurrDynFall_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrDynL1_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrCurrDynL1_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrDynL2_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrCurrDynL2_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrDynRep_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrCurrDynRep_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrCurrDynRise_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrCurrDynRise_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrDynT1_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrCurrDynT1_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrDynT2_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrCurrDynT2_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrDynVrng_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrCurrDynVrng_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrCurrStatFall_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrCurrStatFall_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrStatL1_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrCurrStatL1_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrStatL2_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrCurrStatL2_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrStatRise_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrCurrStatRise_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrStatVrng_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrCurrStatVrng_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrCurrSweDuty_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrCurrSweDuty_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrSweDwel_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrCurrSweDwel_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrSweFall_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrCurrSweFall_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrSweFend_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrCurrSweFend_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrSweFsta_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrCurrSweFsta_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrSweFstep_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrCurrSweFstep_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrSweImax_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrCurrSweImax_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrSweImin_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrCurrSweImin_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrSweRise_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrCurrSweRise_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrDigSampPoin_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrDigSampPoin_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrDigSampTime_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrDigSampTime_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrDigTrigPoin_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrDigTrigPoin_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrDigTrigSour_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrDigTrigSour_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrDigTrig_ReadCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrDigTrig_WriteCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrDigWavCap_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              const ViConstString cacheValue);

static ViStatus _VI_FUNC chr63200AAttrDioOut1_ReadCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrDioOut1_WriteCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrDioOut2_ReadCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrDioOut2_WriteCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrImpStatCl_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrImpStatCl_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrImpStatLs_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrImpStatLs_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrImpStatRl_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrImpStatRl_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrImpStatRs_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrImpStatRs_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);
																	   
static ViStatus _VI_FUNC chr63200AAttrLoadProt_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 *value);
																		
static ViStatus _VI_FUNC chr63200AAttrLoadShor_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrLoadShor_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrLoad_ReadCallback (ViSession vi,
                                                         ViSession io,
                                                         ViConstString channelName,
                                                         ViAttr attributeId,
                                                         ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrLoad_WriteCallback (ViSession vi,
                                                          ViSession io,
                                                          ViConstString channelName,
                                                          ViAttr attributeId,
                                                          ViInt32 value);
																		   
static ViStatus _VI_FUNC chr63200AAttrMode_ReadCallback (ViSession vi,
                                                         ViSession io,
                                                         ViConstString channelName,
                                                         ViAttr attributeId,
                                                         ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrMode_WriteCallback (ViSession vi,
                                                          ViSession io,
                                                          ViConstString channelName,
                                                          ViAttr attributeId,
                                                          ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrPowStatFall_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrPowStatFall_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrPowStatL1_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrPowStatL1_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrPowStatL2_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrPowStatL2_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrPowStatRise_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrPowStatRise_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrPowStatVrng_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrPowStatVrng_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrResDynFall_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrResDynFall_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrResDynIrng_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrResDynIrng_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrResDynL1_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrResDynL1_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrResDynL2_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrResDynL2_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrResDynRep_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrResDynRep_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrResDynRise_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrResDynRise_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrResDynT1_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrResDynT1_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrResDynT2_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrResDynT2_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrResStatFall_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrResStatFall_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrResStatIrng_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrResStatIrng_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrResStatL1_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrResStatL1_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrResStatL2_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrResStatL2_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrResStatRise_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrResStatRise_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);
																				  
static ViStatus _VI_FUNC chr63200AAttrSpecCurrH_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrSpecCurrH_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrSpecCurrL_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrSpecCurrL_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);
																		 
static ViStatus _VI_FUNC chr63200AAttrSpecPowC_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrSpecPowC_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrSpecPowH_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrSpecPowH_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrSpecPowL_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrSpecPowL_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value);
																		   
static ViStatus _VI_FUNC chr63200AAttrSpecTest_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrSpecTest_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrSpecUnit_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrSpecUnit_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrSpecVoltC_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrSpecVoltC_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrSpecVoltH_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrSpecVoltH_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrSpecVoltL_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrSpecVoltL_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrSyncRun_WriteCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 value);
																		
static ViStatus _VI_FUNC chr63200AAttrSyncType_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 value);
															  
static ViStatus _VI_FUNC chr63200AAttrVoltStatIlim_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrVoltStatIlim_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrVoltStatIrng_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrVoltStatIrng_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrVoltStatL1_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrVoltStatL1_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrVoltStatL2_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrVoltStatL2_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrVoltStatRes_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrVoltStatRes_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 value);


static ViStatus _VI_FUNC chr63200AAttrSpecCurrC_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value);

static ViStatus _VI_FUNC chr63200AAttrSpecCurrC_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);


static ViStatus _VI_FUNC chr63200AAttrAdvUserWavDataPoin_CheckCallback (ViSession vi,
                                                                        ViConstString channelName,
                                                                        ViAttr attributeId,
                                                                        ViConstString value);

static ViStatus _VI_FUNC chr63200AAttrAdvUserWavData_CheckCallback (ViSession vi,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViConstString value);

static ViStatus _VI_FUNC chr63200AAttrLoadProtCle_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViBoolean value);

static ViStatus _VI_FUNC chr63200AAttrProgNum_ReadCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrProgNum_WriteCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrProgNum_CheckCallback (ViSession vi,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 value);
															
static ViStatus _VI_FUNC chr63200AAttrProgSequenceNum_CheckCallback (ViSession vi,
                                                                     ViConstString channelName,
                                                                     ViAttr attributeId,
                                                                     ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrProgDataValue_CheckCallback (ViSession vi,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrProgSequenceIndex_CheckCallback (ViSession vi,
                                                                       ViConstString channelName,
                                                                       ViAttr attributeId,
                                                                       ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrAdvBattRng_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvBattRng_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrAdvSwdRng_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrAdvSwdRng_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrCurrSweRng_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 *value);

static ViStatus _VI_FUNC chr63200AAttrCurrSweRng_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrAdvAutoIset_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvAutoPset_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvAutoRset_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvAutoVset_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvBattEndv_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvBattFall_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvBattRise_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvBattTout_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvBattVal_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvCrccIset_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvCrccRset_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvCvccIset_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvCvccVset_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvCvcrRset_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvCvcrVset_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvOcpDwel_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvOcpEnd_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvOcpSpecH_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvOcpSpecL_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvOcpStar_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvOcpStep_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrAdvOcpTrigVolt_CheckCallback (ViSession vi,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvOppDwel_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvOppEnd_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvOppSpecH_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvOppSpecL_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvOppStar_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvOppStep_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrAdvOppTrigVolt_CheckCallback (ViSession vi,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvSineFreq_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvSineIac_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvSineIdc_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrAdvUserWavNsel_CheckCallback (ViSession vi,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrConfVoltOff_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrConfVoltOn_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrConfWind_CheckCallback (ViSession vi,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrDynFall_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrDynL1_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrDynL2_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrDynRep_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrCurrDynRise_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrDynT1_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrDynT2_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrStatFall_CheckCallback (ViSession vi,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrStatL1_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrStatL2_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrStatRise_CheckCallback (ViSession vi,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrSweDuty_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrSweDwel_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrSweFall_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrSweFend_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrSweFsta_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrSweFstep_CheckCallback (ViSession vi,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrSweImax_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrSweImin_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrCurrSweRise_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrDigSampPoin_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrDigSampTime_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrDigTrigPoin_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrImpStatCl_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrImpStatLs_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrImpStatRl_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrImpStatRs_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrPowStatFall_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrPowStatL1_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrPowStatL2_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrPowStatRise_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrResDynFall_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrResDynL1_CheckCallback (ViSession vi,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrResDynL2_CheckCallback (ViSession vi,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrResDynRep_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 value);

static ViStatus _VI_FUNC chr63200AAttrResDynRise_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrResDynT1_CheckCallback (ViSession vi,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrResDynT2_CheckCallback (ViSession vi,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrResStatFall_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrResStatL1_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrResStatL2_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrResStatRise_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrSpecCurrC_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrSpecCurrH_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrSpecCurrL_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrSpecPowC_CheckCallback (ViSession vi,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrSpecPowH_CheckCallback (ViSession vi,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrSpecPowL_CheckCallback (ViSession vi,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrSpecVoltC_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrSpecVoltH_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrSpecVoltL_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrVoltStatIlim_CheckCallback (ViSession vi,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrVoltStatL1_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);

static ViStatus _VI_FUNC chr63200AAttrVoltStatL2_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);
																		  
/*****************************************************************************
 *------------ User-Callable Functions (Exportable Functions) ---------------*
 *****************************************************************************/

/*****************************************************************************
 * Function: chr63200A_init   
 * Purpose:  VXIplug&play required function.  Calls the   
 *           chr63200A_InitWithOptions function.   
 *****************************************************************************/
ViStatus _VI_FUNC chr63200A_init (	ViRsrc 		resourceName, 
									ViBoolean 	IDQuery,
                                 	ViBoolean 	resetDevice, 
									ViSession 	*newVi)
{   
    ViStatus    error = VI_SUCCESS;

    if (newVi == VI_NULL)
        {
        Ivi_SetErrorInfo (VI_NULL, VI_FALSE, IVI_ERROR_INVALID_PARAMETER, 
                          VI_ERROR_PARAMETER4, "Null address for Instrument Handle");
        checkErr( IVI_ERROR_INVALID_PARAMETER);
        }

    checkErr( chr63200A_InitWithOptions (resourceName, IDQuery, resetDevice, 
                                        "", newVi));

Error:  
    return error;
}

/*****************************************************************************
 * Function: chr63200A_InitWithOptions                                       
 * Purpose:  This function creates a new IVI session and calls the 
 *           IviInit function.                                     
 *****************************************************************************/
ViStatus _VI_FUNC chr63200A_InitWithOptions(ViRsrc      resourceName,
										    ViBoolean   IDQuery,
										    ViBoolean   resetDevice,
										    ViString   	optionString,
										    ViSession   *newVi)
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
    checkErr( Ivi_SpecificDriverNew ("chr63200A", newOptionString, &vi));  
    if (!isLogicalName)
    {
        ViInt32 oldFlag = 0;
        
        checkErr (Ivi_GetAttributeFlags (vi, IVI_ATTR_IO_RESOURCE_DESCRIPTOR, &oldFlag));
        checkErr (Ivi_SetAttributeFlags (vi, IVI_ATTR_IO_RESOURCE_DESCRIPTOR, oldFlag & 0xfffb | 0x0010));
        checkErr (Ivi_SetAttributeViString (vi, "", IVI_ATTR_IO_RESOURCE_DESCRIPTOR, 0, newResourceName));
    }
        /* init the driver */
    checkErr( chr63200A_IviInit (newResourceName, IDQuery, resetDevice, vi)); 
    if (isLogicalName)
        checkErr( Ivi_ApplyDefaultSetup (vi));
    *newVi = vi;
    
Error:
    if (error < VI_SUCCESS) 
        Ivi_Dispose (vi);
        
    return error;
}

/*****************************************************************************
 * Function: chr63200A_close                                                           
 * Purpose:  This function closes the instrument.                            
 *
 *           Note:  This function must unlock the session before calling
 *           Ivi_Dispose.
 *****************************************************************************/
ViStatus _VI_FUNC chr63200A_close (ViSession vi)
{
    ViStatus    error = VI_SUCCESS;
    
    checkErr( Ivi_LockSession (vi, VI_NULL));
    
    checkErr( chr63200A_IviClose (vi));

Error:    
    Ivi_UnlockSession (vi, VI_NULL);
    Ivi_Dispose (vi);  

    return error;
}

/*****************************************************************************
 * Function: chr63200A_reset                                                         
 * Purpose:  This function resets the instrument.                          
 *****************************************************************************/
ViStatus _VI_FUNC chr63200A_reset (ViSession vi)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

	if (!Ivi_Simulating(vi))                /* call only when locked */
	    {
	    ViSession   io = Ivi_IOSession(vi); /* call only when locked */
	    
	    checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
	    viCheckErr( viPrintf (io, "*RST\n"));
		Delay(1);
	    }
	
	checkErr( chr63200A_DefaultInstrSetup (vi));                                
	
	checkErr( chr63200A_CheckStatus (vi));                                      

Error:
    Ivi_UnlockSession (vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: chr63200A_ResetWithDefaults
 * Purpose:  This function resets the instrument and applies default settings
 *           from the IVI Configuration Store based on the logical name
 *           from which the session was created.
 *****************************************************************************/
ViStatus _VI_FUNC chr63200A_ResetWithDefaults (ViSession vi)
{
    ViStatus error = VI_SUCCESS;
    
    checkErr( Ivi_LockSession (vi, VI_NULL));
    checkErr( chr63200A_reset(vi));
    checkErr( Ivi_ApplyDefaultSetup(vi));
    
Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/**************************************************************************** 
 *  Function: chr63200A_Disable
 *  Purpose:  This function places the instrument in a quiescent state as 
 *            quickly as possible.
 ****************************************************************************/
ViStatus _VI_FUNC chr63200A_Disable (ViSession vi)
{
    ViStatus error = VI_SUCCESS;
    checkErr( Ivi_LockSession (vi, VI_NULL));
	
Error:
    Ivi_UnlockSession (vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: chr63200A_self_test                                                       
 * Purpose:  This function executes the instrument self-test and returns the 
 *           result.                                                         
 *****************************************************************************/
ViStatus _VI_FUNC chr63200A_self_test (	ViSession 	vi, 
										ViInt16 	*testResult, 
                                      	ViChar 		testMessage[])
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
	    viCheckErr( viPrintf (io, "*TST?\n"));
	        
		viCheckErr( viScanf (io, "%hd,\"%256[^\"]", testResult, testMessage));
	
	    }
	else
	    {
	        /* Simulate Self Test */
	    *testResult = 0;
	    strcpy (testMessage, "No error.");
	    }
	
	checkErr( chr63200A_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: chr63200A_revision_query                                                  
 * Purpose:  This function returns the driver and instrument revisions.      
 *****************************************************************************/
ViStatus _VI_FUNC chr63200A_revision_query (ViSession 	vi, 
											ViChar 		driverRev[], 
                                           	ViChar 		instrRev[])
{
    ViStatus    error = VI_SUCCESS;
    
    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (driverRev == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Driver Revision");
    if (instrRev == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Instrument Revision");

    checkErr( Ivi_GetAttributeViString (vi, "", CHR63200A_ATTR_SPECIFIC_DRIVER_REVISION, 
                                        0, 256, driverRev));
    checkErr( Ivi_GetAttributeViString (vi, "", CHR63200A_ATTR_INSTRUMENT_FIRMWARE_REVISION, 
                                        0, 256, instrRev));
    checkErr( chr63200A_CheckStatus (vi));

Error:    
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: chr63200A_error_query                                                     
 * Purpose:  This function queries the instrument error queue and returns   
 *           the result.                                                     
 *****************************************************************************/
ViStatus _VI_FUNC chr63200A_error_query (	ViSession 	vi, 
											ViInt32 	*errCode, 
                                        	ViChar 		errMessage[])
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
	    viCheckErr( viPrintf(io, ":SYST:ERR?\n"));
	
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
 * Function: chr63200A_error_message                                                  
 * Purpose:  This function translates the error codes returned by this       
 *           instrument driver into user-readable strings.  
 *
 *           Note:  The caller can pass VI_NULL for the vi parameter.  This 
 *           is useful if one of the init functions fail.
 *****************************************************************************/
ViStatus _VI_FUNC chr63200A_error_message (	ViSession 	vi,
											ViStatus errorCode,
                                          	ViChar 	errorMessage[256])
{
    ViStatus    error = VI_SUCCESS;																			
    
    static      IviStringValueTable errorTable = 
        {
			{CHR63200A_ERROR_MODE,						"Invalid Value, Mode Setting value: 1 to 47. Please Refer Function Help for more info."	},
			{CHR63200A_ERROR_COMMAND_TYPE,				"Doesn't support querying/setting syntax. "												},
			{CHR63200A_ERROR_PROGRAM_RUN_TYPE,			"Please set type parameter in program data function before setting list/step."			},
			{CHR63200A_ERROR_PROGRAM_LIST_NUMBER,		"Program number out of Range(1-10)."													},
			{CHR63200A_ERROR_PROGRAM_LIST_SEQUENCE,		"Program sequence count is out of range."												},
			{CHR63200A_ERROR_PROGRAM_LIST_TRIGGER_MODE,	"Program trigger mode parameter is invalid, Valid Range is 0 to 3."						},
			{CHR63200A_ERROR_PROGRAM_LIST_RUN_MODE,		"Program run mode parameter is invalid, Valid Range is 0 to 3."							},
			{CHR63200A_ERROR_PROGRAM_LIST_MODE_RANGE,	"Program mode range parameter is invalid, Valid Range is 0 to 2."						},
			{CHR63200A_ERROR_PROGRAM_LIST_LOAD_VALUE,	"Program load value parameter is invalid."												},
			{CHR63200A_ERROR_PROGRAM_LIST_FALLING,		"Program falling slew parameter is invalid."											},
			{CHR63200A_ERROR_PROGRAM_LIST_RISING,		"Program rising slew parameter is invalid."												},
			{CHR63200A_ERROR_PROGRAM_LIST_DWELL,		"Program dwell time parameter is invalid, Valid Range is 0.001 to 30."					},
			{CHR63200A_ERROR_PROGRAM_LIST_VOLTAGE_HIGH,	"Program voltage specific(high) parameter is invalid."									},
			{CHR63200A_ERROR_PROGRAM_LIST_VOLTAGE_LOW,	"Program voltage specific(low) parameter is invalid."									},
			{CHR63200A_ERROR_PROGRAM_LIST_CURRENT_HIGH,	"Program current specific(high) parameter is invalid."									},
			{CHR63200A_ERROR_PROGRAM_LIST_CURRENT_LOW,	"Program current specific(low) parameter is invalid."									},
			{CHR63200A_ERROR_PROGRAM_LIST_POWER_HIGH,	"Program power specific(high) parameter is invalid."									},
			{CHR63200A_ERROR_PROGRAM_LIST_POWER_LOW,	"Program power specific(low) parameter is invalid."										},
			{CHR63200A_ERROR_PROGRAM_LIST_DELAY,		"Program delay time parameter is invalid, Valid Range is 0 to 30."						},
			{CHR63200A_ERROR_PROGRAM_STEP_NUMBER,		"Program number out of Range(1-10)."													},
			{CHR63200A_ERROR_PROGRAM_STEP_TRIGGER_MODE,	"Program trigger mode parameter is invalid, Valid Range is 0 to 3."						},
			{CHR63200A_ERROR_PROGRAM_STEP_RUN_MODE,		"Program run mode parameter is invalid, Valid Range is 0 to 3."							},
			{CHR63200A_ERROR_PROGRAM_STEP_MODE_RANGE,	"Program mode range parameter is invalid, Valid Range is 0 to 2."						},
			{CHR63200A_ERROR_PROGRAM_STEP_START_VALUE,	"Program start value parameter is invalid."												},
			{CHR63200A_ERROR_PROGRAM_STEP_END_VALUE,	"Program end value parameter is invalid."												},
			{CHR63200A_ERROR_PROGRAM_STEP_FALLING,		"Program falling slew parameter is invalid."											},
			{CHR63200A_ERROR_PROGRAM_STEP_RISING,		"Program rising slew parameter is invalid."												},
			{CHR63200A_ERROR_PROGRAM_STEP_DWELL,		"Program dwell time parameter is invalid, Valid Range is 0.001 to 30."					},
			{CHR63200A_ERROR_PROGRAM_STEP_VOLTAGE_HIGH,	"Program voltage specific(high) parameter is invalid."									},
			{CHR63200A_ERROR_PROGRAM_STEP_VOLTAGE_LOW,	"Program voltage specific(low) parameter is invalid."									},
			{CHR63200A_ERROR_PROGRAM_STEP_CURRENT_HIGH,	"Program current specific(high) parameter is invalid."									},
			{CHR63200A_ERROR_PROGRAM_STEP_CURRENT_LOW,	"Program current specific(low) parameter is invalid."									},
			{CHR63200A_ERROR_PROGRAM_STEP_POWER_HIGH,	"Program power specific(high) parameter is invalid."									},
			{CHR63200A_ERROR_PROGRAM_STEP_POWER_LOW,	"Program power specific(low) parameter is invalid."										},
			{CHR63200A_ERROR_PROGRAM_STEP_DELAY,		"Program delay time parameter is invalid, Valid Range is 0 to 30."						},
			{CHR63200A_ERROR_WFDATA_STATUS,				"UDW Data Error."																		},
			{CHR63200A_ERROR_QUERY_PARAMETER,			"Query Syntax Error."																	},
            {CHR63200A_ERROR_UDW_POINT_OUTOFRANGE,		"Waveform Point out of range."															},
			{VI_NULL,                               	VI_NULL																					}
			
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
 * Function: chr63200A_InvalidateAllAttributes
 * Purpose:  This function invalidates the cached value of all attributes.
 *****************************************************************************/
ViStatus _VI_FUNC chr63200A_InvalidateAllAttributes (ViSession vi)
{
    return Ivi_InvalidateAllAttributes(vi);
}

    /*- Obsolete Error Handling Functions ----------------------------*/
ViStatus _VI_FUNC chr63200A_GetErrorInfo (	ViSession 	vi, 
											ViStatus 	*primaryError, 
                                         	ViStatus 	*secondaryError, 
											ViChar 		errorElaboration[256])  
{                                                                                                           
    return Ivi_GetErrorInfo(vi, primaryError, secondaryError, errorElaboration);                                
}                                                                                                           
ViStatus _VI_FUNC chr63200A_ClearErrorInfo (ViSession vi)                                                        
{                                                                                                           
    return Ivi_ClearErrorInfo (vi);                                                                             
}

/*****************************************************************************
 * Function: chr63200A_GetError and chr63200A_ClearError Functions                       
 * Purpose:  These functions enable the instrument driver user to  
 *           get or clear the error information the driver associates with the
 *           IVI session.                                                        
 *****************************************************************************/
ViStatus _VI_FUNC chr63200A_GetError (	ViSession 	vi, 
                                     	ViStatus 	*errorCode, 
                                     	ViInt32 	bufferSize,
                                     	ViChar 		description[])  
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
        checkErr( chr63200A_error_message (vi, primary, msg));
        appendPoint += sprintf(appendPoint, "Primary Error: (Hex 0x%08X) %s\n", primary, msg);
        }
    
    if (secondary != VI_SUCCESS)
        {
        ViChar msg[256] = "";
        checkErr( chr63200A_error_message (vi, secondary, msg));
        appendPoint += sprintf(appendPoint, "Secondary Error: (Hex 0x%08X) %s\n", secondary, msg);
        }
    
    if (elaboration[0])
        {
        sprintf(appendPoint, "Elaboration: %s", elaboration);
        }
    
    actualSize = (ViInt32)(strlen(errorMessage) + 1);
    
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

ViStatus _VI_FUNC chr63200A_ClearError (ViSession vi)                                                        
{                                                                                                           
    return Ivi_ClearErrorInfo (vi);                                                                             
}

/*****************************************************************************
 * Function: chr63200A_GetNextCoercionRecord                        
 * Purpose:  This function enables the instrument driver user to obtain
 *           the coercion information associated with the IVI session.                                                              
 *           This function retrieves and clears the oldest instance in which 
 *           the instrument driver coerced a value the instrument driver user
 *           specified to another value.                     
 *****************************************************************************/
ViStatus _VI_FUNC chr63200A_GetNextCoercionRecord (	ViSession 	vi,
                                                  	ViInt32 	bufferSize,
                                                  	ViChar  	record[])
{
    return Ivi_GetNextCoercionString (vi, bufferSize, record);
}

/**************************************************************************** 
 *  Function: chr63200A_GetNextInterchangeWarning,
 *            chr63200A_ResetInterchangeCheck, and
 *            chr63200A_ClearInterchangeWarnings
 *  Purpose:  These functions allow the user to retrieve interchangeability
 *            warnings, reset the driver's interchangeability checking 
 *            state, and clear all previously logged interchangeability warnings.
 ****************************************************************************/
ViStatus _VI_FUNC chr63200A_GetNextInterchangeWarning (	ViSession 	vi, 
                                                      	ViInt32 	bufferSize, 
                                                      	ViChar 		warnString[])
{
    return Ivi_GetNextInterchangeCheckString (vi, bufferSize, warnString);
}

ViStatus _VI_FUNC chr63200A_ResetInterchangeCheck (ViSession vi)
{
    return Ivi_ResetInterchangeCheck (vi);
}

ViStatus _VI_FUNC chr63200A_ClearInterchangeWarnings (ViSession vi)
{
    return Ivi_ClearInterchangeWarnings (vi);
}

/*****************************************************************************
 * Function: chr63200A_LockSession and chr63200A_UnlockSession Functions                        
 * Purpose:  These functions enable the instrument driver user to lock the 
 *           session around a sequence of driver calls during which other
 *           execution threads must not disturb the instrument state.
 *                                                                          
 *           NOTE:  The callerHasLock parameter must be a local variable 
 *           initialized to VI_FALSE and passed by reference, or you can pass 
 *           VI_NULL.                     
 *****************************************************************************/
ViStatus _VI_FUNC chr63200A_LockSession (ViSession vi, ViBoolean *callerHasLock)  
{                                              
    return Ivi_LockSession(vi,callerHasLock);      
}                                              
ViStatus _VI_FUNC chr63200A_UnlockSession (ViSession vi, ViBoolean *callerHasLock) 
{                                              
    return Ivi_UnlockSession(vi,callerHasLock);    
}   
											 
/*****************************************************************************
 * Function: chr63200A_GetAttribute<type> Functions                                    
 * Purpose:  These functions enable the instrument driver user to get 
 *           attribute values directly.  There are typesafe versions for 
 *           ViInt32, ViReal64, ViString, ViBoolean, and ViSession 
 *			 attributes.                                         
 *****************************************************************************/
 
ViStatus _VI_FUNC chr63200A_GetAttributeViInt32 (	ViSession 		vi, 
													ViConstString 	channelName, 
                                                	ViAttr 			attributeId, 
													ViInt32 		*value)
{                                                                                                           
    return Ivi_GetAttributeViInt32 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, value);
}                                                                                                           
ViStatus _VI_FUNC chr63200A_GetAttributeViReal64 (	ViSession 		vi, 
													ViConstString 	channelName, 
                                                 	ViAttr 			attributeId, 
													ViReal64 		*value)
{                                                                                                           
    return Ivi_GetAttributeViReal64 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL,value);
}                                                                                                           
ViStatus _VI_FUNC chr63200A_GetAttributeViString (	ViSession 		vi, 
													ViConstString 	channelName, 
                                                 	ViAttr 			attributeId, 
													ViInt32 		bufSize, 
                                                 	ViChar 			value[]) 
{   
    return Ivi_GetAttributeViString (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL,bufSize, value);
}   
ViStatus _VI_FUNC chr63200A_GetAttributeViBoolean (	ViSession 		vi, 
													ViConstString 	channelName, 
                                                  	ViAttr 			attributeId, 
													ViBoolean 		*value)
{                                                                                                           
    return Ivi_GetAttributeViBoolean (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, value);
}                                                                                                           
ViStatus _VI_FUNC chr63200A_GetAttributeViSession (	ViSession 		vi, 
													ViConstString 	channelName, 
                                                  	ViAttr 			attributeId, 
													ViSession 		*value)
{                                                                                                           
    return Ivi_GetAttributeViSession (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, value);
}                                                                                                           

/*****************************************************************************
 * Function: chr63200A_SetAttribute<type> Functions                                    
 * Purpose:  These functions enable the instrument driver user to set 
 *           attribute values directly.  There are typesafe versions for 
 *           ViInt32, ViReal64, ViString, ViBoolean, and ViSession 
 *			 datatypes.                                         
 *****************************************************************************/

ViStatus _VI_FUNC chr63200A_SetAttributeViInt32 (	ViSession 		vi, 
													ViConstString 	channelName, 
                                                	ViAttr 			attributeId, 
													ViInt32 		value)
{                                                                                                           
    return Ivi_SetAttributeViInt32 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, value);
}                                                                                                           
ViStatus _VI_FUNC chr63200A_SetAttributeViReal64 (	ViSession 		vi, 
													ViConstString 	channelName, 
                                                 	ViAttr 			attributeId, 
													ViReal64 		value)
{                                                                                                           
    return Ivi_SetAttributeViReal64 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, value);
}                                                                                                           
ViStatus _VI_FUNC chr63200A_SetAttributeViString (	ViSession 		vi, 
													ViConstString 	channelName, 
                                                 	ViAttr 			attributeId, 
													ViConstString 	value) 
{   
    return Ivi_SetAttributeViString (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, value);
}   
ViStatus _VI_FUNC chr63200A_SetAttributeViBoolean (	ViSession 		vi, 
													ViConstString 	channelName, 
                                                  	ViAttr 			attributeId, 
													ViBoolean 		value)
{                                                                                                           
    return Ivi_SetAttributeViBoolean (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, value);
}                                                                                                           
ViStatus _VI_FUNC chr63200A_SetAttributeViSession (	ViSession 		vi, 
													ViConstString 	channelName, 
                                                  	ViAttr 			attributeId, 
													ViSession 		value)
{                                                                                                           
    return Ivi_SetAttributeViSession (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, value);
}                                                                                                           

/*****************************************************************************
 * Function: chr63200A_CheckAttribute<type> Functions                                  
 * Purpose:  These functions enable the instrument driver user to check  
 *           attribute values directly.  These functions check the value you
 *           specify even if you set the CHR63200A_ATTR_RANGE_CHECK 
 *           attribute to VI_FALSE.  There are typesafe versions for ViInt32, 
 *           ViReal64, ViString, ViBoolean, and ViSession datatypes.                         
 *****************************************************************************/
 
ViStatus _VI_FUNC chr63200A_CheckAttributeViInt32 (		ViSession 		vi, 
														ViConstString 	channelName, 
                                                  		ViAttr 			attributeId, 
														ViInt32 		value)
{                                                                                                           
    return Ivi_CheckAttributeViInt32 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, value);
}
ViStatus _VI_FUNC chr63200A_CheckAttributeViReal64 (	ViSession 		vi, 
														ViConstString 	channelName, 
                                                   		ViAttr 			attributeId, 
														ViReal64 		value)
{                                                                                                           
    return Ivi_CheckAttributeViReal64 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, value);
}                                                                                                           
ViStatus _VI_FUNC chr63200A_CheckAttributeViString (	ViSession 		vi, 
														ViConstString 	channelName, 
	                                                   	ViAttr 			attributeId, 
														ViConstString 	value)  
{   
    return Ivi_CheckAttributeViString (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, value);
}   
ViStatus _VI_FUNC chr63200A_CheckAttributeViBoolean (	ViSession 		vi, 
														ViConstString 	channelName, 
                                                    	ViAttr 			attributeId, 
														ViBoolean 		value)
{                                                                                                           
    return Ivi_CheckAttributeViBoolean (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, value);
}                                                                                                           
ViStatus _VI_FUNC chr63200A_CheckAttributeViSession (	ViSession 		vi, 
														ViConstString 	channelName, 
														ViAttr 			attributeId, 
														ViSession 		value)
{                                                                                                           
    return Ivi_CheckAttributeViSession (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, value);
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
ViStatus _VI_FUNC chr63200A_WriteInstrData (ViSession 	vi, 
											ViConstString writeBuffer)
{   
    return Ivi_WriteInstrData (vi, writeBuffer);    
}   
ViStatus _VI_FUNC chr63200A_ReadInstrData (	ViSession 	vi, 
											ViInt32 	numBytes, 
                                          	ViChar 		rdBuf[], 
											ViInt32 	*bytesRead)  
{   
    return Ivi_ReadInstrData (vi, numBytes, rdBuf, bytesRead);   
} 

/*****************************************************************************
 *-------------------- Utility Functions (Not Exported) ---------------------*
 *****************************************************************************/

/*****************************************************************************
 * Function: chr63200A_IviInit                                                       
 * Purpose:  This function is called by chr63200A_InitWithOptions  
 *           or by an IVI class driver.  This function initializes the I/O 
 *           interface, optionally resets the device, optionally performs an
 *           ID query, and sends a default setup to the instrument.                
 *****************************************************************************/
static ViStatus chr63200A_IviInit (	ViRsrc 		resourceName, 
									ViBoolean 	IDQuery,
                                    ViBoolean 	reset, 
									ViSession 	vi)
{
    ViStatus    error = VI_SUCCESS;
    ViSession   io = VI_NULL;
    

     /* Add attributes */
    checkErr( chr63200A_InitAttributes (vi));

    if (!Ivi_Simulating(vi))
        {
        ViSession   rmSession = VI_NULL;

        /* Open instrument session */
        checkErr( Ivi_GetAttributeViSession (vi, VI_NULL, IVI_ATTR_VISA_RM_SESSION, 0,&rmSession)); 
        viCheckErr( viOpen (rmSession, resourceName, VI_NULL, VI_NULL, &io));
		
        /* io session owned by driver now */
        checkErr( Ivi_SetAttributeViSession (vi, VI_NULL, IVI_ATTR_IO_SESSION, 0, io));  

		/* Configure VISA Formatted I/O */
		viCheckErr( viSetAttribute (io, VI_ATTR_TMO_VALUE, TIME_OUT_VALUE ));
		viCheckErr( viSetBuf (io, VI_READ_BUF | VI_WRITE_BUF, 4000));
		viCheckErr( viSetAttribute (io, VI_ATTR_WR_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS));
		viCheckErr( viSetAttribute (io, VI_ATTR_RD_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS));
		
		
	    //if ( interface_type==VI_INTF_TCPIP )
			{
			viCheckErr( viSetAttribute (io, VI_ATTR_TERMCHAR, 0x0A));  //set termination character to 0x0A
			viCheckErr( viSetAttribute (io, VI_ATTR_TERMCHAR_EN, VI_TRUE));
			}
		
        }
        
    /*- Reset instrument ----------------------------------------------------*/
    if (reset) 
        checkErr( chr63200A_reset (vi));
    else  /*- Send Default Instrument Setup ---------------------------------*/
        checkErr( chr63200A_DefaultInstrSetup (vi));
	
	/*- Identification Query ------------------------------------------------*/
	if (IDQuery)                               
	    {
		    ViChar rdBuffer[BUFFER_SIZE];
			if (!Ivi_Simulating (vi))
			{
		    	checkErr( Ivi_GetAttributeViString (vi, VI_NULL, CHR63200A_ATTR_INSTRUMENT_MODEL,0, BUFFER_SIZE, rdBuffer));
			}
			else
			{
				strcpy(rdBuffer,"63205A-150-500");
			}
		
			checkErr(Ivi_CheckAttributeViString (vi,VI_NULL,CHR63200A_ATTR_SUPPORTED_INSTRUMENT_MODELS,0,rdBuffer));
	    }
	
    checkErr( chr63200A_CheckStatus (vi));
	
Error:
    if (error < VI_SUCCESS)
        {
        if (!Ivi_Simulating (vi) && io)
            viClose (io);
        }
    return error;
}

/*****************************************************************************
 * Function: chr63200A_IviClose                                                        
 * Purpose:  This function performs all of the drivers clean-up operations   
 *           except for closing the IVI session.  This function is called by 
 *           chr63200A_close or by an IVI class driver. 
 *
 *           Note:  This function must close the I/O session and set 
 *           IVI_ATTR_IO_SESSION to 0.
 *****************************************************************************/
static ViStatus chr63200A_IviClose (ViSession vi)
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
 * Function: chr63200A_CheckStatus                                                 
 * Purpose:  This function checks the status of the instrument to detect 
 *           whether the instrument has encountered an error.  This function  
 *           is called at the end of most exported driver functions.  If the    
 *           instrument reports an error, this function returns      
 *           IVI_ERROR_INSTRUMENT_SPECIFIC.  The user can set the 
 *           CHR63200A_ATTR_QUERY_INSTRUMENT_STATUS attribute to VI_FALSE to disable this 
 *           check and increase execution speed.                                   
 *
 *           Note:  Call this function only when the session is locked.
 *****************************************************************************/
static ViStatus chr63200A_CheckStatus (ViSession vi)
{
    ViStatus    error = VI_SUCCESS;
	
    if (Ivi_QueryInstrStatus (vi) && Ivi_NeedToCheckStatus (vi) && !Ivi_Simulating (vi))
        {
        checkErr( chr63200A_CheckStatusCallback (vi, Ivi_IOSession(vi)));
        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_FALSE));
        }
        
Error:
    return error;
}

/*****************************************************************************
 * Function: chr63200A_WaitForOPC                                                  
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
static ViStatus chr63200A_WaitForOPC (	ViSession 	vi, 
										ViInt32 	maxTime) 
{
    ViStatus    error = VI_SUCCESS;

    if (!Ivi_Simulating(vi))
    {
        ViUInt16    interface;
        ViInt32     oldOPCTimeout;
        ViInt32     oldVisaTimeout;
        ViSession   io = Ivi_IOSession(vi);

        viCheckErr( viGetAttribute (io, VI_ATTR_INTF_TYPE, &interface));

        viCheckErr( viGetAttribute (io, VI_ATTR_TMO_VALUE, &oldVisaTimeout));
        viCheckErr( viSetAttribute (io, VI_ATTR_TMO_VALUE, maxTime));

        checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, CHR63200A_ATTR_OPC_TIMEOUT, 0, &oldOPCTimeout));
        checkErr( Ivi_SetAttributeViInt32 (vi, VI_NULL, CHR63200A_ATTR_OPC_TIMEOUT, 0, maxTime));

        if (interface == VI_INTF_GPIB)
        {
            error = chr63200A_WaitForOPCCallback (vi, io);
        }
        else
        {
            ViBoolean opcDone = VI_FALSE;
            ViInt16   statusByte = 0;
            viCheckErr( viPrintf( io, "*OPC;"));
            while (!opcDone)
            {
                viCheckErr (viQueryf (io, "*OPC?", "%hd", &statusByte));
                opcDone = ( statusByte & 1);
                Delay(DELAY_TIME);   
            }
        }

        checkErr( Ivi_SetAttributeViInt32 (vi, VI_NULL, CHR63200A_ATTR_OPC_TIMEOUT, 0, oldOPCTimeout));
        viCheckErr( viSetAttribute(io, VI_ATTR_TMO_VALUE, oldVisaTimeout));

        viCheckErr( error);
    }
Error:
    return error;
}

/*****************************************************************************
 * Function: chr63200A_DefaultInstrSetup                                               
 * Purpose:  This function sends a default setup to the instrument.  The    
 *           chr63200A_reset function calls this function.  The 
 *           chr63200A_IviInit function calls this function when the
 *           user passes VI_FALSE for the reset parameter.  This function is 
 *           useful for configuring settings that other instrument driver 
 *           functions require.    
 *
 *           Note:  Call this function only when the session is locked.
 *****************************************************************************/

static ViStatus chr63200A_DefaultInstrSetup (ViSession vi)
{
    ViStatus    error = VI_SUCCESS;
        
    /* Invalidate all attributes */
    checkErr( Ivi_InvalidateAllAttributes (vi));
	
    if (!Ivi_Simulating(vi))
        {
        ViSession   io = Ivi_IOSession(vi); /* call only when locked */

        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
		
		viCheckErr( viPrintf (io, "*CLS;*ESE 1;*SRE 32\n"));
		
        }
Error:
    return error;
}

/*****************************************************************************
 * Function:chr63200A_Abort  
 * Purpose:	Set all electronic loads as "OFF".  
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_Abort (ViSession vi)
{
	ViStatus	error = VI_SUCCESS;
	
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
    if (!Ivi_Simulating(vi))
        {
		ViSession	io = Ivi_IOSession(vi);
        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
		viCheckErr( viPrintf (io, "ABOR\n"));
		}
	
	checkErr(chr63200A_CheckStatus(vi));
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function:chr63200A_Fetch  
 * Purpose: Returns the value measured. 
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_Fetch (ViSession 	vi, 
									ViInt32 	item,
                          			ViReal64 	*result)
{
	ViStatus	error = VI_SUCCESS;
	
	
	checkErr( Ivi_LockSession(vi,VI_NULL));
	if (!Ivi_Simulating(vi))			
    {
		ViSession	io = Ivi_IOSession(vi);
		ViChar		szItem[16];
	
		switch (item)
		{
			default:
			
				break;
			case 0:
				strcpy(szItem,"AH");
				break;
			case 1:
				strcpy(szItem,"CURR");
				break;
			case 2:
				strcpy(szItem,"CURR:PEAK+");
				break;
			case 3:
				strcpy(szItem,"FREQ");
				break;
			case 4:
				strcpy(szItem,"POW");
				break;
			case 5:
				strcpy(szItem,"STAT");
				break;
			case 6:
				strcpy(szItem,"TIME");
				break;
			case 7:
				strcpy(szItem,"WH");
				break;
			case 8:
				strcpy(szItem,"VOLT");
				break;
			case 9:
				strcpy(szItem,"VOLT:MAX");
				break;
			case 10:
				strcpy(szItem,"VOLT:MIN");
				break;
			case 11:
				strcpy(szItem,"VOLT:PEAK+");
				break;
			case 12:
				strcpy(szItem,"VOLT:PEAK+:FREQ");
				break;
			case 13:
				strcpy(szItem,"VOLT:PEAK-");
				break;
			case 14:
				strcpy(szItem,"VOLT:PEAK-:FREQ");
				break;
		}
		
		viQueryf(io,"FETC:%s?\n","%Lf",szItem,result);

	}
	else
	{
		*result = ((ViReal64)rand() / (ViReal64)RAND_MAX); 
	}
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function:chr63200A_ElectronicLoadState  
 * Purpose: Set Electronic Load State and query back status
 *****************************************************************************/

ViStatus _VI_FUNC  chr63200A_ElectronicLoadState (	ViSession vi,
                                        			ViBoolean trigger, 
													ViBoolean shortState,
                                        			ViBoolean loadStatus[], 
													ViBoolean clear)
{
	ViStatus	error = VI_SUCCESS;
	ViInt32		response = 0;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_LOAD,(ViInt32)trigger),2,"Trigger");
	viCheckParm(chr63200A_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_LOAD_SHOR,(ViInt32)shortState),3,"Short State");

	viCheckParm(chr63200A_SetAttributeViBoolean	(vi,VI_NULL, CHR63200A_ATTR_LOAD_PROT_CLE,clear),5,"clear");
	viCheckParm(chr63200A_GetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_LOAD_PROT,&response),4,"loadStatus");
	
	/*-----------------------------------------------------------------------------------------------------------------------------
	  15		14		13		12		11		10		9		8		7		6		5		4		3		2		1		0
	 ------------------------------------------------------------------------------------------------------------------------------
				MAX   	RMT		VCC		FAN		SYNC	OTP		OPP3	OPP2	OPP1	OCP3	OCP2	OCP1	REV		OV2		OV1
				_LIM	_INH
	 -----------------------------------------------------------------------------------------------------------------------------*/
	
	for(int i=0 ; i<=15; i++)
	{
		loadStatus[i] = (ViBoolean)((response & (int)pow(2,i)) / (int)pow(2,i));
	}
	
	
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function:chr63200A_SynchronousRun  
 * Purpose: Set all electronic loads to ��ON�� in sync. parallel run.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_SynchronousRun (ViSession vi, ViBoolean run)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	viCheckParm(chr63200A_SetAttributeViInt32(vi,VI_NULL, CHR63200A_ATTR_SYNC_RUN,(ViInt32)run),2,"Invalid Value");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}
				  
/*****************************************************************************
 * Function:chr63200A_Measure  
 * Purpose: Returns the real time value measured at the load module input. 
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_Measure (	ViSession 	vi, 
										ViInt32 	inputPort,
                            			ViInt32 	item, 
										ViReal64 	*response)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	if (!Ivi_Simulating(vi))			
    {
		ViSession	io = Ivi_IOSession(vi);
		ViChar		szItem[16];
	
		viPrintf(io,"MEAS:INP %d\n",inputPort);

		
		switch (item)
		{
			case 0:
				strcpy(szItem,":CURR");
				break;
			case 1:
				strcpy(szItem,":POW");
				break;
			case 2:
				strcpy(szItem,":VOLT");
				break;
			default:
				viCheckParm(IVI_ERROR_INVALID_VALUE,3,"Item");
				break;
		}
		checkErr(viQueryf(io,"MEAS%s?\n","%Lf",szItem,response));
	}

Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}
/*****************************************************************************
 * Function:chr63200A_ProtectionResult  
 * Purpose: The OCP (or OPP) provides ramped up current (or power) for the
 *			Load to test the UUT voltage whether has reached trigger voltage
 *			level and to judge if the protection is acting normally or not.  
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ProtectionTestResult (	ViSession 	vi,
													ViInt32 	*testResult, 
													ViReal64 	*testValue,
													ViReal64 	*maximumPower,
													ViInt32		item)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	ViChar	szReadBuffer[BUFFER_SIZE];
	
	checkErr(chr63200A_GetAttributeViString(vi,VI_NULL,(!item ? CHR63200A_ATTR_ADV_OCP_RES : CHR63200A_ATTR_ADV_OPP_RES),
											BUFFER_SIZE,szReadBuffer));
	
	Scan(szReadBuffer,"%s>%d[x]%f[x]%f",testResult,testValue,maximumPower);
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function:chr63200A_Mode  
 * Purpose: This command sets the operational mode for the electronic load.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_OperatingMode (ViSession 	vi, 
											ViInt32 	mode, 
											ViInt32 	level)
{
	ViStatus	error = VI_SUCCESS;

	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	switch(mode)
	{
		case	CHR63200A_VAL_MOD_CCL  :
		case	CHR63200A_VAL_MOD_CRL  :
		case	CHR63200A_VAL_MOD_CVL  :
		case	CHR63200A_VAL_MOD_CPL  :
		case	CHR63200A_VAL_MOD_CCDL :
		case	CHR63200A_VAL_MOD_CRDL :
		case	CHR63200A_VAL_MOD_BATL :
		case	CHR63200A_VAL_MOD_SWDL :
		case	CHR63200A_VAL_MOD_OCPL :
		case	CHR63200A_VAL_MOD_CCSL :
		case	CHR63200A_VAL_MOD_CZL  :
		case	CHR63200A_VAL_MOD_CVCC :
		case	CHR63200A_VAL_MOD_CRCC :
		case	CHR63200A_VAL_MOD_CVCR :
		case	CHR63200A_VAL_MOD_AUTO :
		case	CHR63200A_VAL_MOD_PROG :
		case	CHR63200A_VAL_MOD_MPPTL:
		case	CHR63200A_VAL_MOD_UDWL :
		case	CHR63200A_VAL_MOD_EXTL :
			viCheckParm(chr63200A_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_MODE,mode + level),2,"Mode"); 	
		break;
		
		default:						
			viCheckParm(chr63200A_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_MODE,mode),2,"Mode"); 		    
		break;
	}

Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function:chr63200A_SpecificationJudgment  
 * Purpose:Request GO-NG result reference to specifications.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_SpecificationJudgment (ViSession 	vi,
                                           			ViInt32 	item, 
													ViChar 		testResult[])
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	if (!Ivi_Simulating(vi))			
    {
		ViSession	io = Ivi_IOSession(vi);
		ViChar		szItem[16];
	
		switch (item)
		{
			default:
			
				break;
			case 0:
				strcpy(szItem,"");
				break;
			case 1:
				strcpy(szItem,":CURR");
				break;
			case 2:
				strcpy(szItem,":POW");
				break;
			case 3:
				strcpy(szItem,":VOLT");
				break;
		}
		viQueryf(io,"SPEC%s?\n","%s",szItem,testResult);
	}

Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function:chr63200A_DigitizingInitiate  
 * Purpose:Init digitizing function.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_DigitizingInitiate (ViSession vi)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	if (!Ivi_Simulating(vi))			
    {
		ViSession	io = Ivi_IOSession(vi);
		viPrintf(io,"DIG:INIT\n");
	}
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function:chr63200A_DigitizingAbort  
 * Purpose:Abort digitizing function.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_DigitizingAbort (ViSession vi)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	if (!Ivi_Simulating(vi))			
    {
		ViSession	io = Ivi_IOSession(vi);
		viPrintf(io,"DIG:ABOR\n");
	}
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function:chr63200A_DigitizingSamplingTime  
 * Purpose: Set the sampling Time
 *****************************************************************************/

ViStatus _VI_FUNC chr63200A_DigitizingSamplingTime (ViSession vi, ViReal64 time)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));    
	
	viCheckParm(chr63200A_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_DIG_SAMP_TIME,time),2,"time");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);   	
	return error;
}

/*****************************************************************************
 * Function:chr63200A_DigitizingSamplingPoint  
 * Purpose: Set the sampling Point
 *****************************************************************************/

ViStatus _VI_FUNC chr63200A_DigitizingSamplingPoint (ViSession vi, ViInt32 point)
{
	ViStatus	error = VI_SUCCESS;

	checkErr( Ivi_LockSession(vi,VI_NULL));

	viCheckParm(chr63200A_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_DIG_SAMP_POIN,point),2,"point");
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function:chr63200A_DigitizingTrigger  
 * Purpose: Set the trigger state
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_DigitizingTrigger (ViSession vi, ViInt32 state)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));

	viCheckParm(chr63200A_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_DIG_TRIG,state),2,"state");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function:chr63200A_DigitizingTriggerPoint  
 * Purpose: Set the trigger point
 *****************************************************************************/

ViStatus _VI_FUNC chr63200A_DigitizingTriggerPoint (ViSession vi, ViInt32 point)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));    

	viCheckParm(chr63200A_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_DIG_TRIG_POIN,point),2,"Point");     
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);            
	return error;
}

/*****************************************************************************
 * Function:chr63200A_DigitizingTriggerSource  
 * Purpose: Set the trigger source
 *****************************************************************************/

ViStatus _VI_FUNC chr63200A_DigitizingTriggerSource (ViSession vi, ViInt32 source)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));                                                                   
	
	viCheckParm(chr63200A_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_DIG_TRIG_SOUR,source),2,"Source");   

Error:
	Ivi_UnlockSession(vi,VI_NULL);            
	return error;
}

/*****************************************************************************
 * Function:chr63200A_DigitizingTriggerStatus  
 * Purpose: Query the trigger status
 *****************************************************************************/

ViStatus _VI_FUNC chr63200A_DigitizingTriggerStatus (ViSession vi, ViInt32 *status)
{
	ViStatus	error = VI_SUCCESS;

	checkErr( Ivi_LockSession(vi,VI_NULL));                                                                   
	
	viCheckParm(Ivi_GetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_DIG_TRIG,VI_NULL,status),2,"Status");   

Error:
	Ivi_UnlockSession(vi,VI_NULL);  
	return error;
}

/*****************************************************************************
 * Function:chr63200A_DigitizingTrigger  
 * Purpose: This query returns voltage or current waveform data from the DC
 *			Electronic Load in binary format. The waveform either voltage or
 * 			current are consist of number points correspond to sampling
 * 			points that user specified in format of 32bits float point.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_DigitizingWaveform (	ViSession 	vi,
                                       				ViReal64 	dataResponse[],
                                       				ViInt32 	returnItem)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szWriteBuffer[16],szCaptureStatus[8],*rawData=VI_NULL,buffer[4],szLen[7],szDataLen[10];
	ViInt32		iSampPoin,iLen,i,retCount;															  //szLen is header to data lenth.
	float		*Point=VI_NULL;

	checkErr( Ivi_LockSession(vi,VI_NULL));
	
    if (!Ivi_Simulating(vi))			
    {
		ViSession	io = Ivi_IOSession(vi);
		time_t timeStart,timePass;
		time(&timeStart);
		
		do
		{
			Ivi_SetNeedToCheckStatus (vi, VI_TRUE);
			checkErr(Ivi_GetAttributeViString(vi,VI_NULL,CHR63200A_ATTR_DIG_WAV_CAP,VI_NULL,BUFFER_SIZE,szCaptureStatus));
			checkErr(chr63200A_CheckStatus(vi));
			
			if(strcmp(szCaptureStatus,"ERROR")==0)									
				viCheckErrElab(CHR63200A_ERROR_DIG_CAP_WF_FAIL,"Capture fail");
			
			else if(strcmp(szCaptureStatus,"WAIT")==0)
			{
				time(&timePass);
				if(difftime (timePass,timeStart) > 5*(TIME_OUT_VALUE/1000))				
				viCheckErrElab(VI_ERROR_TMO,"Time out");								
																					
			}
			
			Delay(DELAY_TIME);
		}while( strcmp(szCaptureStatus,"OK")!=0);		
		
		checkErr(Ivi_GetAttributeViInt32(vi, VI_NULL, CHR63200A_ATTR_DIG_SAMP_POIN, VI_NULL, &iSampPoin));

		strcpy(szWriteBuffer,(returnItem != 1 ? "DIG:WAV:DATA? V\n" : "DIG:WAV:DATA? I\n"));
		viCheckErr(viWrite(io, szWriteBuffer,16,&retCount));
		viCheckErr(viRead(io, szLen,7,&retCount));
	    sscanf (szLen, "%*[#]%1[0-9]%5[0-9]", buffer,szDataLen);  
		
		Scan(szDataLen,"%s>%i",&iLen);
		
		rawData = calloc(iLen,sizeof(ViChar)); 
		Point = calloc(iLen/4,sizeof(float));
		
		viCheckErr(viRead(io, rawData,iLen,&retCount));  
	
		memcpy(Point, rawData , (iLen));

		for (i=0;i<(iLen/4);i++)
		{
			dataResponse[i] = Point[i];
		}
	
	}
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	if (!Ivi_Simulating(vi))			
	{
		if(Point)
			free(Point);
		if(rawData)
			free(rawData);
	}
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureCCModeMeasureRange
 * Purpose:  Configure measure range.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureCCModeMeasureRange (	ViSession 	vi,
                                                			ViInt32 	measureRange)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViInt32(vi,VI_NULL,CHR63200A_ATTR_CURR_STAT_VRNG,measureRange),2,"Range");  
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureCCModeLoadValue
 * Purpose:  Configure load value.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureCCModeLoadValue (	ViSession 	vi,
                                             			ViReal64 	loadValue)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL,CHR63200A_ATTR_CURR_STAT_L1,loadValue),2,"Load Value"); 
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureCCModeSlewRate
 * Purpose:  Configure current slew rate.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureCCModeSlewRate (	ViSession 	vi,
                                            			ViReal64 	fallingSlewRate,
                                            			ViReal64 	risingSlewRate)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL,CHR63200A_ATTR_CURR_STAT_FALL,fallingSlewRate),2,"Fall"); 
	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL,CHR63200A_ATTR_CURR_STAT_RISE,risingSlewRate),3,"Rise"); 			

	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureCRModeMeasureRange
 * Purpose:  Configure measure range.   
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureCRModeMeasureRange (	ViSession 	vi,
                                                			ViInt32 	measureRange)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViInt32(vi,VI_NULL, CHR63200A_ATTR_RES_STAT_IRNG,measureRange),2,"Range");  
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureCRModeLoadValue
 * Purpose:  Configure load value.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureCRModeLoadValue (	ViSession 	vi,
                                             			ViReal64 	loadValue)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL, CHR63200A_ATTR_RES_STAT_L1,loadValue),2,"Load Value"); 
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureCRModeSlewRate
 * Purpose:  Configure measure range.   
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureCRModeSlewRate (	ViSession 	vi,
                                            			ViReal64 	fallingSlewRate,
                                            			ViReal64 	risingSlewRate)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL,CHR63200A_ATTR_RES_STAT_FALL,fallingSlewRate),2,"Fall"); 
	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL,CHR63200A_ATTR_RES_STAT_RISE,risingSlewRate),3,"Rise"); 			

	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureCVModeMeasureRange
 * Purpose:  Configure measure range.   
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureCVModeMeasureRange (	ViSession 	vi,
                                                			ViInt32 	measureRange)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViInt32(vi,VI_NULL, CHR63200A_ATTR_VOLT_STAT_IRNG,measureRange),2,"Range");  
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureCVModeLoadValue
 * Purpose:  Configure load value.   
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureCVModeLoadValue (	ViSession 	vi,
                                             			ViReal64 	loadValue)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL, CHR63200A_ATTR_VOLT_STAT_L1,loadValue),2,"Load Value"); 
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureCVModeCurrentLimit
 * Purpose:  Configure current limit.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureCVModeCurrentLimit (	ViSession 	vi,
                                                			ViReal64 	currentLimit)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL, CHR63200A_ATTR_VOLT_STAT_ILIM,currentLimit),2,"Load Value"); 
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureCVModeResponseSpeed
 * Purpose:  Configure response speed.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureCVModeResponseSpeed (	ViSession 	vi,
                                                 			ViInt32 	responseSpeed)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViInt32(vi,VI_NULL, CHR63200A_ATTR_VOLT_STAT_RES,responseSpeed),2,"Response Speed");  
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureCPModeMeasureRange
 * Purpose:  Configure measure range.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureCPModeMeasureRange (	ViSession 	vi,
                                          					ViInt32 	measureRange)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViInt32(vi,VI_NULL, CHR63200A_ATTR_POW_STAT_VRNG,measureRange),2,"Range");  
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureCPModeLoadValue
 * Purpose:  Configure load value.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureCPModeLoadValue (	ViSession 	vi,
                                             			ViReal64 	loadValue)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL, CHR63200A_ATTR_POW_STAT_L1,loadValue),2,"Load Value"); 
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureCPModeSlewRate
 * Purpose:  Configure current slew rate.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureCPModeSlewRate (	ViSession 	vi,
                                            			ViReal64 	fallingSlewRate,
                                            			ViReal64 	risingSlewRate)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViReal64(vi, VI_NULL, CHR63200A_ATTR_POW_STAT_FALL,fallingSlewRate),2,"Fall"); 
	viCheckParm(chr63200A_SetAttributeViReal64(vi, VI_NULL, CHR63200A_ATTR_POW_STAT_RISE,risingSlewRate),3,"Rise"); 			

	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureCCDModeMeasureRange
 * Purpose:  Configure measure range.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureCCDModeMeasureRange (	ViSession 	vi,
                                                 			ViInt32 	measureRange)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViInt32(vi,VI_NULL, CHR63200A_ATTR_CURR_DYN_VRNG,measureRange),2,"Range");  
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureCCDModeRepeat
 * Purpose:  Configure repeat times.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureCCDModeRepeat (	ViSession 	vi,
                                           				ViInt32 	repeat)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViInt32(vi,VI_NULL, CHR63200A_ATTR_CURR_DYN_REP,repeat),2,"repeat");  
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureCCDModeSlewRate
 * Purpose:  Configure current slew rete
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureCCDModeSlewRate (	ViSession 	vi,
                                             			ViReal64 	fallingSlewRate,
                                             			ViReal64 	risingSlewRate)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViReal64(vi, VI_NULL, CHR63200A_ATTR_CURR_DYN_FALL,fallingSlewRate),2,"Fall"); 
	viCheckParm(chr63200A_SetAttributeViReal64(vi, VI_NULL, CHR63200A_ATTR_CURR_DYN_RISE,risingSlewRate),3,"Rise"); 			

	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureCCDModeLoadValue
 * Purpose:  Configure load value.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureCCDModeLoadValue (ViSession 	vi,
                                              			ViReal64 	l1, 
														ViReal64 	l2)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL, CHR63200A_ATTR_CURR_DYN_L1,l1),2,"Load Value 1"); 
	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL, CHR63200A_ATTR_CURR_DYN_L2,l2),3,"Load Value 2"); 
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureCCDModeTime
 * Purpose:  Configure load time.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureCCDModeTime (	ViSession 	vi, 
													ViReal64 	t1,
                                         			ViReal64 	t2)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL, CHR63200A_ATTR_CURR_DYN_T1,t1),2,"Time 1"); 
	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL, CHR63200A_ATTR_CURR_DYN_T2,t2),3,"Time 2"); 
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureCRDModeMeasureRange 
 * Purpose:  Configure measure range.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureCRDModeMeasureRange (	ViSession 	vi,
                                                 			ViInt32 	measureRange)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViInt32(vi,VI_NULL, CHR63200A_ATTR_RES_DYN_IRNG,measureRange),2,"Range");  
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureCRDModeRepeat
 * Purpose:  Configure repeat times.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureCRDModeRepeat (	ViSession 	vi,
                                           				ViInt32 	repeat)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViInt32(vi,VI_NULL, CHR63200A_ATTR_RES_DYN_REP,repeat),2,"repeat");  
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureCRDModeSlewRate 
 * Purpose:  Configure current slew rate
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureCRDModeSlewRate (	ViSession 	vi,
                                             			ViReal64 	fallingSlewRate,
                                             			ViReal64 	risingSlewRate)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViReal64(vi, VI_NULL, CHR63200A_ATTR_RES_DYN_FALL,fallingSlewRate),2,"Fall"); 
	viCheckParm(chr63200A_SetAttributeViReal64(vi, VI_NULL, CHR63200A_ATTR_RES_DYN_RISE,risingSlewRate),3,"Rise"); 			

	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureCRDModeLoadValue 
 * Purpose:  Configure load value.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureCRDModeLoadValue (ViSession 	vi,
                                              			ViReal64 	l1, 
														ViReal64 	l2)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL, CHR63200A_ATTR_RES_DYN_L1,l1),2,"Load Value 1"); 
	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL, CHR63200A_ATTR_RES_DYN_L2,l2),3,"Load Value 2"); 
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureCRDModeTime 
 * Purpose:  Configure load time.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureCRDModeTime (	ViSession 	vi, 
													ViReal64 	t1,
                                         			ViReal64 	t2)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL, CHR63200A_ATTR_RES_DYN_T1,t1),2,"Time 1"); 
	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL, CHR63200A_ATTR_RES_DYN_T2,t2),3,"Time 2"); 
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureAutoModeVoltage 
 * Purpose:  Configure voltage
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureAutoModeVoltage (	ViSession 	vi,
                                             			ViReal64 	voltage)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_ADV_AUTO_VSET,voltage),2,"voltage"); 
	
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureAutoModeResistance 
 * Purpose:  Configure Resistance
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureAutoModeResistance (	ViSession 	vi,
                                                			ViReal64 	resistance)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_ADV_AUTO_RSET,resistance),2,"resistance");
	
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureAutoModeCurrent 
 * Purpose:  Configure current
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureAutoModeCurrent (	ViSession 	vi,
                                             			ViReal64 	current)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_ADV_AUTO_ISET,current),2,"current"); 			
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureAutoModePower 
 * Purpose:  Configure power
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureAutoModePower (	ViSession 	vi,
                                           				ViReal64 	power)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_ADV_AUTO_PSET,power),2,"power");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureBatteryModeRunMode 
 * Purpose:  Configure run mode
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureBatteryModeRunMode (	ViSession 	vi,
                                                			ViInt32 	mode)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));

	viCheckParm(chr63200A_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_ADV_BATT_MODE,mode),2,"mode"); 

Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureBatteryModeLoadValue 
 * Purpose:  Configure load value
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureBatteryModeLoadValue (ViSession 	vi,
                                                  			ViReal64 	loadValue)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_ADV_BATT_VAL,loadValue),3,"loadValue");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureBatteryModeSlewRate 
 * Purpose:  Configure current slew rate
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureBatteryModeSlewRate (	ViSession 	vi,
                                                 			ViReal64 	fallingSlewRate,
                                                 			ViReal64 	risingSlewRate)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_ADV_BATT_FALL,fallingSlewRate),2,"fallingSlewRate");
	viCheckParm(chr63200A_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_ADV_BATT_RISE,risingSlewRate),3,"risingSlewRate");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureBatteryModeEndVoltage 
 * Purpose:  Configure end voltage
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureBatteryModeEndVoltage (	ViSession 	vi,
                                                   				ViReal64 	endVoltage)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_ADV_BATT_ENDV,endVoltage),3,"endVoltage");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureBatteryModeTimeout 
 * Purpose:  Configure timeout value
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureBatteryModeTimeout (	ViSession 	vi,
                                                			ViReal64 	timeout)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_ADV_BATT_TOUT,timeout),3,"timeout");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_SelectProgram 
 * Purpose:  Select the program. 
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_SelectProgram (ViSession 	vi,
                                  			ViInt32 	programNumber)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_PROG_NUM,programNumber),2,"programNumber");

	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureProgramType 
 * Purpose:  Configure type of program and set the program parameters.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureProgramType (	ViSession 	vi,
                                         			ViInt32 	type, 
													ViInt32 	chain,
                                         			ViInt32 	repeat, 
													ViInt32 	sequence)
{
	ViStatus	error = VI_SUCCESS;
	ViInt32	    programNumber=-1;
	
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(Ivi_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_PROG_TYPE,0,type),2,"type");
	viCheckParm(Ivi_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_PROG_CHAIN,0,chain),3,"chain");
	viCheckParm(Ivi_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_PROG_REPEAT,0,repeat),4,"repeat");
	viCheckParm(Ivi_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_PROG_SEQUENCE_NUM,0,sequence),5,"sequence");
	
	if (!Ivi_Simulating(vi))			
    {
		ViSession	io = Ivi_IOSession(vi);
		
		Ivi_GetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_PROG_NUM,0,&programNumber);
		viPrintf(io,"PROG:DATA %d,%d,%d,%d,%d\n",programNumber,type,chain,repeat,sequence);
		
	}

	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_SelectSequence 
 * Purpose:  Select the sequence (for list type)
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_SelectSequence (	ViSession 	vi,
                                   				ViInt32 	sequenceIndex)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(Ivi_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_PROG_SEQUENCE_INDEX,0,sequenceIndex),2,"sequenceIndex");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureProgramTriggerMode 
 * Purpose:  Configure trigger mode.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureProgramTriggerMode (	ViSession 	vi,
                                                			ViInt32 	triggerMode)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(Ivi_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_PROG_TRIG_MODE,0,triggerMode),2,"triggerMode");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureProgramRunMode 
 * Purpose:  Configure run mode
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureProgramRunMode (	ViSession 	vi,
                                            			ViInt32 	runMode, 
														ViInt32 	modeRange)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(Ivi_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_PROG_RUN_MODE,0,runMode),2,"runMode");
	viCheckParm(Ivi_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_PROG_MODE_RANGE,0,modeRange),3,"modeRange");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureProgramListValue 
 * Purpose:	 Configure load value for list type.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureProgramListValue (ViSession 	vi,
                                              			ViReal64 	loadValue)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(Ivi_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_PROG_LIST_VALUE,0,loadValue),2,"loadValue");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureProgramStepValue 
 * Purpose:  Configure start and end value for step type.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureProgramStepValue (ViSession 	vi,
                                              			ViReal64 	startValue,
                                              			ViReal64 	endValue)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(Ivi_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_PROG_STEP_START,0,startValue),2,"startValue");
	viCheckParm(Ivi_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_PROG_STEP_END,0,endValue),3,"endValue");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureProgramSlewRate 
 * Purpose:  configure current slew rate.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureProgramSlewRate (	ViSession 	vi,
                                             			ViReal64 	risingSlewRate,
                                             			ViReal64 	fallingSlewRate)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(Ivi_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_PROG_RISE,0,risingSlewRate),2,"risingSlewRate");
	viCheckParm(Ivi_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_PROG_FALL,0,fallingSlewRate),3,"fallingSlewRate");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureProgramDwellTime 
 * Purpose:  Configure dwell time.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureProgramDwellTime (ViSession 	vi,
                                              			ViReal64 	dwellTime)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(Ivi_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_PROG_DWELL,0,dwellTime),2,"dwellTime");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureProgramSpecification 
 * Purpose:  Configure specific.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureProgramSpecification (ViSession 	vi,
                                                  			ViReal64 	highLevelVoltage, 
															ViReal64 	lowLevelVoltage,
                                                  			ViReal64 	highLevelCurrent, 
															ViReal64 	lowLevelCurrent,
                                                  			ViReal64 	highLevelPower, 
															ViReal64 	lowLevelPower)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(Ivi_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_PROG_VH,0,highLevelVoltage	),2,"High Level Voltage");
	viCheckParm(Ivi_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_PROG_VL,0,lowLevelVoltage	),3,"Low Level Voltage"	);
	viCheckParm(Ivi_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_PROG_IH,0,highLevelCurrent	),4,"High Level Current");
	viCheckParm(Ivi_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_PROG_IL,0,lowLevelCurrent	),5,"Low Level Current"	);
	viCheckParm(Ivi_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_PROG_PH,0,highLevelPower	),6,"High Level Power"	);
	viCheckParm(Ivi_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_PROG_PL,0,lowLevelPower		),7,"low Level Power"	);
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureProgramDelayTime 
 * Purpose:  Configure delay time.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureProgramDelayTime (ViSession 	vi,
                                              			ViReal64 	delayTime)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(Ivi_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_PROG_DELAY,0,delayTime),2,"delayTime");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_SaveProgram 
 * Purpose:  Send program parameters to device and save the program.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_SaveProgram (ViSession vi)
{
	ViStatus	error = VI_SUCCESS;

	checkErr( Ivi_LockSession(vi,VI_NULL));

	if (!Ivi_Simulating(vi))			
    {
		ViSession	io = Ivi_IOSession(vi);
		
		ViInt32		iProgramNumber	 	= -1;
		ViInt32		iProgramType 		= -1;
		ViInt32		iProgramTrigger		= -1;
		ViInt32		iProgramMode		= -1;
		ViInt32		iProgramRange		= -1;
		ViInt32		iSequenceIndex		= -1;
		ViReal64	fProgramListValue	= -1;
		ViReal64	fProgramStepStart	= -1;
		ViReal64	fProgramStepEnd		= -1;
		ViReal64	fProgramFall		= -1;
		ViReal64	fProgramRise		= -1;
		ViReal64	fProgramDwell		= -1;
		ViReal64	fProgramVH			= -1;
		ViReal64	fProgramVL			= -1;
		ViReal64	fProgramCH			= -1;
		ViReal64	fProgramCL			= -1;
		ViReal64	fProgramPH			= -1;
		ViReal64	fProgramPL			= -1;
		ViReal64	fProgramDelay		= -1;
		
		checkErr(Ivi_GetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_PROG_NUM,			0,&iProgramNumber		));
		checkErr(Ivi_GetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_PROG_TYPE,			0,&iProgramType			));
		checkErr(Ivi_GetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_PROG_TRIG_MODE,		0,&iProgramTrigger		));
		checkErr(Ivi_GetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_PROG_RUN_MODE,		0,&iProgramMode			));
		checkErr(Ivi_GetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_PROG_MODE_RANGE,	0,&iProgramRange		));
		checkErr(Ivi_GetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_PROG_SEQUENCE_INDEX,0,&iSequenceIndex		));
		checkErr(Ivi_GetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_PROG_LIST_VALUE,	0,&fProgramListValue	));
		checkErr(Ivi_GetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_PROG_STEP_START,	0,&fProgramStepStart	));
		checkErr(Ivi_GetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_PROG_STEP_END,		0,&fProgramStepEnd		));
		checkErr(Ivi_GetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_PROG_FALL,			0,&fProgramFall			));
		checkErr(Ivi_GetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_PROG_RISE,			0,&fProgramRise			));
		checkErr(Ivi_GetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_PROG_DWELL,			0,&fProgramDwell		));
		checkErr(Ivi_GetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_PROG_VH,			0,&fProgramVH			));
		checkErr(Ivi_GetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_PROG_VL,			0,&fProgramVL			));
		checkErr(Ivi_GetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_PROG_IH,			0,&fProgramCH			));
		checkErr(Ivi_GetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_PROG_IL,			0,&fProgramCL			));
		checkErr(Ivi_GetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_PROG_PH,			0,&fProgramPH			));
		checkErr(Ivi_GetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_PROG_PL,			0,&fProgramPL			));
		checkErr(Ivi_GetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_PROG_DELAY,			0,&fProgramDelay		));
		
		
		if(!iProgramType)
		{							  /*			05			   10			  15*/
			viPrintf(io,"PROG:DATA:LIST %d,%d,%d,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",
			/*01*/	 iProgramNumber,		/*ViInt32*/
			/*02*/	 iSequenceIndex,		/*ViInt32*/
			/*03*/	 iProgramTrigger,		/*ViInt32*/
			/*04*/	 iProgramMode,			/*ViInt32*/
			/*05*/	 iProgramRange,			/*ViInt32*/
			/*06*/	 fProgramListValue,		/*ViReal64*/
			/*07*/	 fProgramFall,			/*ViReal64*/
			/*08*/	 fProgramRise,			/*ViReal64*/
			/*09*/	 fProgramDwell,			/*ViReal64*/
			/*10*/	 fProgramVH,			/*ViReal64*/
			/*11*/	 fProgramVL,			/*ViReal64*/
			/*12*/	 fProgramCH,			/*ViReal64*/
			/*13*/	 fProgramCL,			/*ViReal64*/
			/*14*/	 fProgramPH,			/*ViReal64*/
			/*15*/	 fProgramPL,			/*ViReal64*/
			/*16*/	 fProgramDelay);		/*ViReal64*/
		}
		else
		{							  /*			05			   10			  15*/
			viPrintf(io,"PROG:DATA:STEP %d,%d,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",
			/*01*/	 iProgramNumber,		/*ViInt32*/  
			/*02*/	 iProgramTrigger,		/*ViInt32*/  
			/*03*/	 iProgramMode,			/*ViInt32*/  
			/*04*/	 iProgramRange,			/*ViInt32*/  
			/*05*/	 fProgramStepStart,		/*ViReal64*/  
			/*06*/	 fProgramStepEnd,		/*ViReal64*/ 
			/*07*/	 fProgramFall,			/*ViReal64*/ 
			/*08*/	 fProgramRise,			/*ViReal64*/ 
			/*09*/	 fProgramDwell,			/*ViReal64*/ 
			/*10*/	 fProgramVH,			/*ViReal64*/ 
			/*11*/	 fProgramVL,			/*ViReal64*/ 
			/*12*/	 fProgramCH,			/*ViReal64*/ 
			/*13*/	 fProgramCL,			/*ViReal64*/ 
			/*14*/	 fProgramPH,			/*ViReal64*/ 
			/*15*/	 fProgramPL,			/*ViReal64*/ 
			/*16*/	 fProgramDelay);		/*ViReal64*/ 
		}
		
		viCheckErr(	viPrintf(io,"PROG:SAV")); 
		chr63200A_WaitForOPC(vi,OPC_TIME);
		
	}
	
	
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_GetProgramStates 
 * Purpose:  Get program states.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_GetProgramStates (	ViSession 	vi,
                                     			ViInt32 	*programNumber,
                                     			ViInt32 	*sequnceNumber, 
												ViInt32 	*loadMode,
                                     			ViInt32 	*executionState)
{
	ViStatus	error = VI_SUCCESS;

	checkErr( Ivi_LockSession(vi,VI_NULL));

	if (!Ivi_Simulating(vi))			
    {
		ViSession	io = Ivi_IOSession(vi);
	
		viQueryf(io,"PROG:STAT?\n", "%d,%d,%d,%d",programNumber,sequnceNumber,loadMode,executionState);
	}
	else
	{
		*programNumber 	= 1;
		*sequnceNumber 	= 1;
		*loadMode		= 8;
		*executionState = 0;
	}
	
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ClearSequence 
 * Purpose:  Clear  sequences.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ClearSequence (ViSession vi)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	if (!Ivi_Simulating(vi))			
    {
		ViSession	io = Ivi_IOSession(vi);
		ViInt32		iProgramNumber	 	= -1;

		checkErr(Ivi_GetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_PROG_NUM,0,&iProgramNumber));
		
		viPrintf(io,"PROG:SEQ:CLE %d\n",iProgramNumber);
	}

Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_FailOfSequence 
 * Purpose:  Query fail.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_FailOfSequence (	ViSession 	vi, 
												ViChar 		result[])
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	if (!Ivi_Simulating(vi))			
    {
		ViSession	io = Ivi_IOSession(vi);
	
		viQueryf(io,"PROG:SEQ:FAIL?\n","%s",result);
	}

Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_RemainSequence 
 * Purpose:  Query all remain sequences.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_RemainSequence (	ViSession 	vi, 
												ViInt32 	*result)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	if (!Ivi_Simulating(vi))			
    {
		ViSession	io = Ivi_IOSession(vi);
	
		viQueryf(io,"PROG:SEQ:REM?\n","%d",result);
	}

Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_SpecificationOfSequence 
 * Purpose:  Specification Of Sequence 
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_SpecificationOfSequence (	ViSession 	vi,
                                            			ViReal64 	result[], 
														ViInt32 	parameter)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		*rawData=VI_NULL,buffer[BUFFER_SIZE];
	ViInt32		iLen,i;
	ViReal64	*Point=VI_NULL;

	checkErr( Ivi_LockSession(vi,VI_NULL));
	
    if (!Ivi_Simulating(vi))			
    {
		ViSession	io = Ivi_IOSession(vi);
		ViInt32		iProgramNumber	 	= -1;

		checkErr(Ivi_GetAttributeViInt32(vi,VI_NULL, CHR63200A_ATTR_PROG_NUM,0,&iProgramNumber));
		
		Point = calloc(100,sizeof(ViReal64));
		
		rawData = calloc(3200,sizeof(ViChar));
		
		viCheckErr( viQueryf (io, "PROG:SEQ:SPEC? %d,%d\n","%s",iProgramNumber,parameter,rawData));
		
		sscanf (rawData, "%*[#]%1[0-9]", buffer);  
		
		iLen = atoi(buffer);   
		
		memcpy(Point, rawData + iLen+2 , iLen);

		for (i=0;i<iLen/4;i++)
		{
			result[i] = Point[i];
		}
	}
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
   if (!Ivi_Simulating(vi))			
   {
		free(Point);
		free(rawData);
   }
	return error;
}

/*****************************************************************************
 * Function: chr63200A_StartSpecificationTest 
 * Purpose:  Start GO-NG test.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_StartSpecificationTest (	ViSession 	vi,
                                           				ViBoolean 	enable, 
														ViInt32 	unit)
{
	ViStatus	error = VI_SUCCESS;										   
	checkErr( Ivi_LockSession(vi,VI_NULL));

	viCheckParm(chr63200A_SetAttributeViInt32(vi,VI_NULL, CHR63200A_ATTR_SPEC_TEST,(ViInt32)enable),2,"Enable");
	viCheckParm(chr63200A_SetAttributeViInt32(vi,VI_NULL, CHR63200A_ATTR_SPEC_UNIT,unit),3,"Unit"); 
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureBatteryModeMeasureRange 
 * Purpose:  configure measure range.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureBatteryModeMeasureRange (	ViSession 	vi,
                                                     			ViInt32 	measureRange)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViInt32(vi,VI_NULL, CHR63200A_ATTR_ADV_BATT_RNG,measureRange),2,"Range");  
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureCVCCModeResponse
 * Purpose:  Configure Response speed.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureCVCCModeResponse (ViSession 	vi,
                                              			ViInt32 	response)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_ADV_CVCC_RES,response),2,"response"); 
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureCVCCModeVoltage 
 * Purpose:  Configure voltage.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureCVCCModeVoltage (	ViSession 	vi,
                                             			ViReal64 	voltage)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_ADV_CVCC_VSET,voltage),2,"voltage");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureCVCCModeCurrent 
 * Purpose:  Configure current.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureCVCCModeCurrent (	ViSession 	vi,
                                             			ViReal64 	current)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_ADV_CVCC_ISET,current),2,"current"); 
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureCRCCModeResistance 
 * Purpose:  Configure resistance.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureCRCCModeResistance (	ViSession 	vi,
                                                			ViReal64 	resistance)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_ADV_CRCC_RSET,resistance),2,"voltage");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureCRCCModeCurrent 
 * Purpose:  Configure current.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureCRCCModeCurrent (	ViSession 	vi,
                                             			ViReal64 	current)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_ADV_CRCC_ISET,current),2,"voltage");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureCVCRModeResistance 
 * Purpose:  Configure resistance.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureCVCRModeResistance (	ViSession 	vi,
                                                			ViReal64 	resistance)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_ADV_CVCR_RSET,resistance),2,"resistance");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureCVCRModeVoltage 
 * Purpose:  Configure voltage.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureCVCRModeVoltage (	ViSession 	vi,
                                             			ViReal64 	voltage)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_ADV_CVCR_VSET,voltage),2,"voltage");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureOCPModeLatch    
 * Purpose:  Configure latch.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureOCPModeLatch (ViSession 	vi,
                                          			ViInt32 	latch)
{
	ViStatus	error = VI_SUCCESS;										   
	checkErr( Ivi_LockSession(vi,VI_NULL));

	viCheckParm(chr63200A_SetAttributeViInt32(vi,VI_NULL, CHR63200A_ATTR_ADV_OCP_LATC,latch),2,"latch");

Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureOCPModeStep
 * Purpose:  Configure Step.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureOCPModeStep (	ViSession 	vi,
                                         			ViInt32 	step)
{
	ViStatus	error = VI_SUCCESS;										   
	checkErr( Ivi_LockSession(vi,VI_NULL));

	viCheckParm(chr63200A_SetAttributeViInt32(vi,VI_NULL, CHR63200A_ATTR_ADV_OCP_STEP,step),2,"step");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureOCPModeLoadValue    
 * Purpose:  Configure load value.  
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureOCPModeLoadValue (ViSession 	vi,
                                              			ViReal64 	startValue,
                                              			ViReal64 	endValue)
{
	ViStatus	error = VI_SUCCESS;										   
	checkErr( Ivi_LockSession(vi,VI_NULL));

	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL, CHR63200A_ATTR_ADV_OCP_STAR,startValue),2,"startValue");
	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL, CHR63200A_ATTR_ADV_OCP_END,endValue),3,"endValue");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureOCPModeDwellTime     
 * Purpose:  Configure Dwell time  
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureOCPModeDwellTime (ViSession 	vi,
                                              			ViReal64 	dwellTime)
{
	ViStatus	error = VI_SUCCESS;										   
	checkErr( Ivi_LockSession(vi,VI_NULL));

	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL, CHR63200A_ATTR_ADV_OCP_DWEL,dwellTime),2,"dwellTime");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureOCPModeTriggerVoltage    
 * Purpose:  Configure trigger voltage  
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureOCPModeTriggerVoltage (	ViSession 	vi,
                                                   				ViReal64 	triggerVoltage)
{
	ViStatus	error = VI_SUCCESS;										   
	checkErr( Ivi_LockSession(vi,VI_NULL));

	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL, CHR63200A_ATTR_ADV_OCP_TRIG_VOLT,triggerVoltage),2,"triggerVoltage");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureOCPModeSpecification    
 * Purpose:  Configure current specific.  
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureOCPModeSpecification (ViSession 	vi,
                                                  			ViReal64 	highLevelCurrent, 
															ViReal64 	lowLevelCurrent)
{
	ViStatus	error = VI_SUCCESS;										   
	checkErr( Ivi_LockSession(vi,VI_NULL));

	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL, CHR63200A_ATTR_ADV_OCP_SPEC_H,highLevelCurrent),2,"High Level Current");
	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL, CHR63200A_ATTR_ADV_OCP_SPEC_L,lowLevelCurrent),3,"Low Level Current");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureOPPModeLatch    
 * Purpose:  Configure latch.  
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureOPPModeLatch (ViSession 	vi,
                                          			ViInt32 	latch)
{
	ViStatus	error = VI_SUCCESS;										   
	checkErr( Ivi_LockSession(vi,VI_NULL));

	viCheckParm(chr63200A_SetAttributeViInt32(vi,VI_NULL, CHR63200A_ATTR_ADV_OPP_LATC,latch),2,"latch");

Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureOPPModeStep    
 * Purpose:  Configure step.  
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureOPPModeStep (	ViSession 	vi,
                                         			ViInt32 	step)
{
	ViStatus	error = VI_SUCCESS;										   
	checkErr( Ivi_LockSession(vi,VI_NULL));

	viCheckParm(chr63200A_SetAttributeViInt32(vi,VI_NULL, CHR63200A_ATTR_ADV_OPP_STEP,step),2,"step");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureOPPModeLoadValue    
 * Purpose:  Configure load value  
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureOPPModeLoadValue (ViSession 	vi,
                                              			ViReal64 	startValue,
                                              			ViReal64 	endValue)
{
	ViStatus	error = VI_SUCCESS;										   
	checkErr( Ivi_LockSession(vi,VI_NULL));

	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL, CHR63200A_ATTR_ADV_OPP_STAR,startValue),2,"startValue");
	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL, CHR63200A_ATTR_ADV_OPP_END,endValue),3,"endValue");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureOPPModeDwellTime    
 * Purpose:  Configure dwell time.  
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureOPPModeDwellTime (ViSession 	vi,
                                              			ViReal64 	dwellTime)
{
	ViStatus	error = VI_SUCCESS;										   
	checkErr( Ivi_LockSession(vi,VI_NULL));

	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL, CHR63200A_ATTR_ADV_OPP_DWEL,dwellTime),2,"dwellTime");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureOPPModeTriggerVoltage    
 * Purpose:  Configure trigger voltage.  
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureOPPModeTriggerVoltage (	ViSession 	vi,
                                                   				ViReal64 	triggerVoltage)
{
	ViStatus	error = VI_SUCCESS;										   
	checkErr( Ivi_LockSession(vi,VI_NULL));

	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL, CHR63200A_ATTR_ADV_OPP_TRIG_VOLT,triggerVoltage),2,"triggerVoltage");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureOPPModeSpecification    
 * Purpose:  Configure power specific  
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureOPPModeSpecification (ViSession 	vi,
                                                  			ViReal64 	highLevelPower, 
															ViReal64	lowLevelPower)
{
	ViStatus	error = VI_SUCCESS;										   
	checkErr( Ivi_LockSession(vi,VI_NULL));

	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL, CHR63200A_ATTR_ADV_OPP_SPEC_H,highLevelPower),2,"High Level Power");
	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL, CHR63200A_ATTR_ADV_OPP_SPEC_L,lowLevelPower),3,"Low Level Power");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureSWDModeDCCurrent    
 * Purpose:  Configure DC current  
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureSWDModeDCCurrent (ViSession 	vi,
                                              			ViReal64 	DCCurrent)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_ADV_SINE_IDC,DCCurrent),2,"DCCurrent"); 			

Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureSWDModeACCurrent    
 * Purpose:  Configure AC current  
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureSWDModeACCurrent (ViSession 	vi,
                                               			ViReal64 	ACCurrent)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));

	viCheckParm(chr63200A_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_ADV_SINE_IAC,ACCurrent),2,"ACCurrent"); 
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureSWDModeFrequency    
 * Purpose:  Configure frequency  
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureSWDModeFrequency (ViSession 	vi,
                                              			ViReal64 	frequency)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));

	viCheckParm(chr63200A_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_ADV_SINE_FREQ,frequency),2,"frequency");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureSWDModeMeasureRange    
 * Purpose:  Configure measure range  
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureSWDModeMeasureRange (	ViSession 	vi,
                                                 			ViInt32 	measureRange)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViInt32(vi,VI_NULL, CHR63200A_ATTR_ADV_SWD_RNG,measureRange),2,"Range");  
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureSweepModeCurrent    
 * Purpose:  Configure current  
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureSweepModeCurrent (ViSession 	vi,
                                              			ViReal64 	maxCurrent,
                                              			ViReal64 	minCurrent)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));

	viCheckParm(chr63200A_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_CURR_SWE_IMAX,maxCurrent),2,"maxCurrent");
	viCheckParm(chr63200A_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_CURR_SWE_IMIN,minCurrent),3,"minCurrent");

Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureSweepModeFrequency    
 * Purpose:  Configure frequency  
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureSweepModeFrequency (	ViSession 	vi,
			                                                ViReal64 	startFrequency,
			                                                ViReal64 	endFrequency,
			                                                ViReal64 	stepFrequency)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));

	viCheckParm(chr63200A_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_CURR_SWE_FSTA,startFrequency),2,"startFrequency");
	viCheckParm(chr63200A_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_CURR_SWE_FEND,endFrequency),3,"endFrequency");
	viCheckParm(chr63200A_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_CURR_SWE_FSTEP,stepFrequency),4,"stepFrequency");

Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureSweepModeDwellTime    
 * Purpose:  Configure dwell time  
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureSweepModeDwellTime (	ViSession 	vi,
                                                			ViReal64 	dwellTime)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));

	viCheckParm(chr63200A_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_CURR_SWE_DWEL,dwellTime),2,"dwellTime");

Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureSweepModeDuty    
 * Purpose:  Configure duty cycle.  
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureSweepModeDuty (	ViSession 	vi,
                                           				ViReal64 	duty)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));

	viCheckParm(chr63200A_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_CURR_SWE_DUTY,duty),2,"duty");

Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureSweepModeSlewRate    
 * Purpose:  Configure slew rate  
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureSweepModeSlewRate (	ViSession 	vi,
                                               				ViReal64 	fallingSlewRate,
                                               				ViReal64 	risingSlewRate)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));

	viCheckParm(chr63200A_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_CURR_SWE_FALL,fallingSlewRate),2,"fallingSlewRate");
	viCheckParm(chr63200A_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_CURR_SWE_RISE,risingSlewRate),3,"risingSlewRate");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureSweepModeMeasureRange    
 * Purpose:  Configure measure range  
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureSweepModeMeasureRange (	ViSession 	vi,
                                                   				ViInt32 	measureRange)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViInt32(vi,VI_NULL, CHR63200A_ATTR_CURR_SWE_RNG,measureRange),2,"Range");  
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureCZMode    
 * Purpose:  Configure CZ Mode  
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureCZMode (	ViSession 	vi, 
												ViInt32 	item,
                                    			ViReal64 	itemValue)
{
	ViStatus	error = VI_SUCCESS;
	ViAttr		attrItem = -1;
	
	checkErr( Ivi_LockSession(vi,VI_NULL));

	switch(item)
	{
		
		case CHR63200A_VAL_CZ_CL:
			attrItem = CHR63200A_ATTR_IMP_STAT_CL;
			break;
		
		case CHR63200A_VAL_CZ_RL:
			attrItem = CHR63200A_ATTR_IMP_STAT_RL;
			break;
		
		case CHR63200A_VAL_CZ_LS:
			attrItem = CHR63200A_ATTR_IMP_STAT_LS;
			break;
		
		case CHR63200A_VAL_CZ_RS:
			attrItem = CHR63200A_ATTR_IMP_STAT_RS;
			break;
		
		default:
			viCheckParm(IVI_ERROR_INVALID_VALUE ,2,"Invalid Item");
			break;
	}
	
	viCheckParm(chr63200A_SetAttributeViReal64	(vi,VI_NULL, attrItem,itemValue),3,"Invalid Value");

Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureEXTWMode
 * Purpose:  Configure EXT Waveform Mode
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureEXTWMode (ViSession 	vi, 
												ViInt32 	mode,
                                      			ViInt32 	measureRange)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_ADV_EXT_WAV_MODE,mode),2,"Mode");

	viCheckParm(chr63200A_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_ADV_EXT_WAV_RNG,measureRange),3,"Measure Range");

Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_SelectUserWaveform    
 * Purpose:  Select wavefrom number  
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_SelectUserWaveform (	ViSession 	vi,
                                       				ViInt32 	number)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));

	viCheckParm(chr63200A_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_ADV_USER_WAV_NSEL,number),2,"number");

Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureUserWaveformData    
 * Purpose:  Configure waveform parameter  
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureUserWaveformData (ViSession 	vi,
                                              			ViReal64 	interval, 
														ViInt32 	repeat,
                                              			ViInt32 	chain,
                                              			ViInt32 	interpolation)
{
	ViStatus	error = VI_SUCCESS;
	
	ViChar	szUDWPara[BUFFER_SIZE]="";
	ViInt32	iNumber;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckErr(chr63200A_GetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_ADV_USER_WAV_NSEL, &iNumber));
	
	Fmt(szUDWPara,"%s<%d,%f[p8],%d,%d,%d",iNumber,interval,repeat,chain,interpolation);
	
	viCheckErr(chr63200A_SetAttributeViString	(vi,VI_NULL, CHR63200A_ATTR_ADV_USER_WAV_DATA,szUDWPara));

Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureUserWaveformPoint    
 * Purpose:  Load waveform point.  
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureUserWaveformPoint (	ViSession 	vi,
                                               				ViChar 		point[])
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	checkErr(chr63200A_SetAttributeViString	(vi,VI_NULL, CHR63200A_ATTR_ADV_USER_WAV_DATA_POIN,point));

Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_GetUserWaveformRemainPoint    
 * Purpose:  Get remain point  
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_GetUserWaveformRemainPoint (	ViSession 	vi,
                                               				ViInt32 	*remain)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_GetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_ADV_USER_WAV_REM,remain),2,"remain");

Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_GetUserWaveformStatus    
 * Purpose:  Get Status.  
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_GetUserWaveformStatus (ViSession 	vi,
                                          			ViInt32 	*dataStatus,
                                          			ViInt32 	*executeStatus)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));

	viCheckErr(chr63200A_GetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_ADV_USER_WAV_DATA_STAT,dataStatus));
	viCheckErr(chr63200A_GetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_ADV_USER_WAV_EXE_STAT,executeStatus));

Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ClearUserWaveform    
 * Purpose:  Clear waveform point.  
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ClearUserWaveform (ViSession 	vi,
                                      			ViInt32 	*clearStatus)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));

	viCheckErr(chr63200A_GetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_ADV_USER_WAV_CLE,clearStatus));

Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureVoltageSpecification    
 * Purpose:  Configure voltage specification  
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureVoltageSpecification (ViSession 	vi,
                                                  			ViReal64 	lowLevelVoltage, 
															ViReal64 	centerLevelVoltage,
                                                  			ViReal64 	highLevelVoltage)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL,	CHR63200A_ATTR_SPEC_VOLT_L,lowLevelVoltage),2,"Invalid VL");            
	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL,	CHR63200A_ATTR_SPEC_VOLT_C,centerLevelVoltage),3,"Invalid VC");      
	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL,	CHR63200A_ATTR_SPEC_VOLT_H,highLevelVoltage),4,"Invalid VH");         
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureCurrentSpecification    
 * Purpose:  Configure current specification  
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigureCurrentSpecification (ViSession 	vi,
                                                  			ViReal64 	lowLevelCurrent, 
															ViReal64 	centerLevelCurrent,
                                                  			ViReal64 	highLevelCurrent)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL,	CHR63200A_ATTR_SPEC_CURR_L,lowLevelCurrent),2,"Invalid IL");            
	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL,	CHR63200A_ATTR_SPEC_CURR_C,centerLevelCurrent),3,"Invalid IC");      
	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL,	CHR63200A_ATTR_SPEC_CURR_H,highLevelCurrent),4,"Invalid IH");         
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigurePowerSpecification    
 * Purpose:  Configure power specification  
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ConfigurePowerSpecification (	ViSession 	vi,
                                                			ViReal64 	lowLevelPower,
															ViReal64 	centerLevelPower,
                                                			ViReal64 	highLevelPower)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL,	CHR63200A_ATTR_SPEC_POW_L,lowLevelPower),2,"Invalid PL");            
	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL,	CHR63200A_ATTR_SPEC_POW_C,centerLevelPower),3,"Invalid PC");      
	viCheckParm(chr63200A_SetAttributeViReal64(vi,VI_NULL,	CHR63200A_ATTR_SPEC_POW_H,highLevelPower),4,"Invalid PH");         
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 * Function: chr63200A_ConfigureParallel    
 * Purpose:  Configure parallel and Sync.
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_ParallelAndSynchronous (	ViSession 	vi,
                                           				ViInt32 	enableParallel,
                                           				ViInt32 	parallelType,
                                           				ViInt32 	syncType)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));

	viCheckParm(chr63200A_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_CONF_PARA_MODE, parallelType),3,"ParallelType");
	viCheckParm(chr63200A_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_CONF_SYNC_MODE, syncType),4,"SyncType");
	viCheckParm(chr63200A_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_CONF_PARA_INIT, enableParallel),2,"EnableParallel");

Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}
/*****************************************************************************
 * Function: chr63200A_SystemSetup    
 * Purpose:  Configure window time  
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_SystemSetup (	ViSession 	vi, 
											ViInt32 	sound,
                                			ViInt32 	enterKey)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_CONF_SOUN,sound),2,"sound");
	viCheckParm(chr63200A_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_CONF_ENT_KEY,enterKey),3,"Enter Key");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}
/*****************************************************************************
 * Function: chr63200A_LoadSetup    
 * Purpose:  Load Setup
 *****************************************************************************/
ViStatus _VI_FUNC  chr63200A_LoadSetup (ViSession 	vi, 
										ViInt32 	latchReset,
                              			ViInt32 	latch, 
										ViReal64 	von, 
										ViReal64 	voff,
                              			ViInt32 	shortKey, 
										ViInt32 	autoOn)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));

	if(latchReset)
	{
		ViChar	szBuf[16];
		strcpy(szBuf,"CONF:VOLT:LATC:RES\n");
	 	chr63200A_WriteInstrData (vi,szBuf);

	}
	viCheckParm(chr63200A_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_CONF_VOLT_LATC,latch),3,"Latch");
	viCheckParm(chr63200A_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_CONF_VOLT_ON,von),4,"Von");
	viCheckParm(chr63200A_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_CONF_VOLT_OFF,voff),5,"Voff");
	viCheckParm(chr63200A_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_CONF_SHOR_KEY,shortKey),6,"Short Key");
	viCheckParm(chr63200A_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_CONF_AUTO_ON,autoOn),7,"Auto On");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}
/*****************************************************************************
 * Function: chr63200A_MeasurementSetting    
 * Purpose:  Measurement setting
 *****************************************************************************/

ViStatus _VI_FUNC  chr63200A_MeasurementSetting (	ViSession 	vi, 
													ViInt32 	sign,
                                       				ViReal64 	windowTime)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));
	
	viCheckParm(chr63200A_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_CONF_VOLT_SIGN,sign),2,"Sign");
	viCheckParm(chr63200A_SetAttributeViReal64	(vi,VI_NULL, CHR63200A_ATTR_CONF_WIND,windowTime),3,"Window Time");
	
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}
/*****************************************************************************   
 * Function: chr63200A_DIO                                              
 * Purpose:  Configure Digital IO Port.
 *****************************************************************************/  
ViStatus _VI_FUNC  chr63200A_DIO (	ViSession 	vi, 
									ViInt32 	DI1Type,
                        			ViInt32 	DO1Type, 
									ViBoolean 	DO1, 
									ViInt32 	DI2Type,
                        			ViInt32 	DO2Type, 
									ViBoolean 	DO2)
{
	ViStatus	error = VI_SUCCESS;
	checkErr( Ivi_LockSession(vi,VI_NULL));

	viCheckParm(chr63200A_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_CONF_DIO_IN1,DI1Type),2,"DI1Type");
	viCheckParm(chr63200A_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_CONF_DIO_IN2,DI2Type),3,"DI2Type");
	viCheckParm(chr63200A_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_CONF_DIO_OUT1,DO1Type),4,"DO1Type");
	viCheckParm(chr63200A_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_CONF_DIO_OUT2,DO2Type),5,"DO2Type");
	viCheckParm(chr63200A_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_DIO_OUT1,DO1),6,"DO1");
	viCheckParm(chr63200A_SetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_DIO_OUT2,DO2),7,"DO2");
Error:
	Ivi_UnlockSession(vi,VI_NULL);
	return error;
}

/*****************************************************************************
 *------------------------ Global Session Callbacks -------------------------*
 *****************************************************************************/

/*****************************************************************************
 * Function: chr63200A_CheckStatusCallback                                               
 * Purpose:  This function queries the instrument to determine if it has 
 *           encountered an error.  If the instrument has encountered an 
 *           error, this function returns the IVI_ERROR_INSTRUMENT_SPECIFIC 
 *           error code.  This function is called by the 
 *           chr63200A_CheckStatus utility function.  The 
 *           Ivi_SetAttribute and Ivi_GetAttribute functions invoke this 
 *           function when the optionFlags parameter includes the
 *           IVI_VAL_DIRECT_USER_CALL flag.
 *           
 *           The user can disable calls to this function by setting the 
 *           CHR63200A_QUERY_INSTRUMENT_STATUS attribute to VI_FALSE.  The driver can 
 *           disable the check status callback for a particular attribute by 
 *           setting the IVI_VAL_DONT_CHECK_STATUS flag.
 *****************************************************************************/
static ViStatus _VI_FUNC chr63200A_CheckStatusCallback (ViSession vi, ViSession io)
{
    ViStatus    error = VI_SUCCESS;
	/* Query instrument status */
	ViInt16     esr = 0; 
	ViChar		szErrmsg[256]={VI_NULL};
	
	viCheckErr( viQueryf (io, "*ESR?\n", "%hd", &esr));
	if( esr&IEEE_488_2_QUERY_ERROR_BIT) strcat(szErrmsg,"Query Error,");
	if( esr&IEEE_488_2_DEVICE_DEPENDENT_ERROR_BIT) strcat(szErrmsg,"Device Dependent Error,");
	if( esr&IEEE_488_2_EXECUTION_ERROR_BIT) strcat(szErrmsg,"Execution Error,");
	if( esr&IEEE_488_2_COMMAND_ERROR_BIT) strcat(szErrmsg,"Command Error,");
	
	if(strcmp(szErrmsg,"")!=0)
	{
		szErrmsg[strlen(szErrmsg)-1]='\0';
		Ivi_SetErrorInfo(vi, VI_TRUE, IVI_ERROR_INSTR_SPECIFIC, 0, szErrmsg);
		error = IVI_ERROR_INSTR_SPECIFIC;
	}
Error:
    return error;
}

/*****************************************************************************
 * Function: chr63200A_WaitForOPCCallback                                               
 * Purpose:  This function waits until the instrument has finished processing 
 *           all pending operations.  This function is called by the 
 *           chr63200A_WaitForOPC utility function.  The IVI engine invokes
 *           this function in the following two cases:
 *           - Before invoking the read callback for attributes for which the 
 *             IVI_VAL_WAIT_FOR_OPC_BEFORE_READS flag is set.
 *           - After invoking the write callback for attributes for which the 
 *             IVI_VAL_WAIT_FOR_OPC_AFTER_WRITES flag is set.
 *****************************************************************************/
static ViStatus _VI_FUNC chr63200A_WaitForOPCCallback (ViSession vi, ViSession io)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     opcTimeout;
    ViUInt16    statusByte;
    ViUInt16    interface;
	
	viCheckErr( viGetAttribute (io, VI_ATTR_INTF_TYPE, &interface));	
    if (interface != VI_INTF_GPIB)
    {
        ViBoolean opcDone = VI_FALSE;
        viCheckErr( viPrintf( io, "*OPC;"));
        while (!opcDone )
        {
            viCheckErr (viQueryf (io, "*OPC?", "%hd", &statusByte));
            opcDone = ( statusByte & 1);
            Delay(DELAY_TIME); 
        }
        viCheckErr( error);
    }
	else
	{
    checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, CHR63200A_ATTR_OPC_TIMEOUT, 
                                       0, &opcTimeout));

    viCheckErr( viEnableEvent (io, VI_EVENT_SERVICE_REQ, VI_QUEUE, VI_NULL));

    viCheckErr( viPrintf (io, "*OPC\n"));

        /* wait for SRQ */
    viCheckErr( viWaitOnEvent (io, VI_EVENT_SERVICE_REQ, opcTimeout, VI_NULL, VI_NULL));
    viCheckErr( viDisableEvent (io, VI_EVENT_SERVICE_REQ, VI_QUEUE));

        /* clean up after SRQ */
    viCheckErr( viBufWrite (io, (ViBuf)"*CLS", 4, VI_NULL));
    viCheckErr( viReadSTB (io, &statusByte));
	}
	
	
Error:
    viDiscardEvents (io, VI_EVENT_SERVICE_REQ, VI_QUEUE);
	
    return error;
}

/*****************************************************************************
 *----------------- Attribute Range Tables and Callbacks --------------------*
 *****************************************************************************/

/*- CHR63200A_ATTR_ID_QUERY_RESPONSE -*/

static ViStatus _VI_FUNC chr63200AAttrIdQueryResponse_ReadCallback (ViSession vi, 
                                                                   ViSession io,
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId,
                                                                   const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];
    ViUInt32    retCnt;
    
    viCheckErr( viPrintf (io, "*IDN?\n"));
    viCheckErr( viRead (io, (ViPBuf)rdBuffer, BUFFER_SIZE-1, &retCnt));
    rdBuffer[retCnt] = 0;

    checkErr( Ivi_SetValInStringCallback (vi, attributeId, rdBuffer));
    
Error:
    return error;
}
    
/*- CHR63200A_ATTR_SPECIFIC_DRIVER_REVISION -*/

static ViStatus _VI_FUNC chr63200AAttrDriverRevision_ReadCallback (ViSession vi, 
                                                                  ViSession io,
                                                                  ViConstString channelName, 
                                                                  ViAttr attributeId,
                                                                  const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      driverRevision[256];
    
    
    sprintf (driverRevision, 
             "Driver: chr63200A Series %d.%d, Compiler: %s %3.2f, "
             "Components: IVIEngine %.2f, VISA-Spec %.2f",
             CHR63200A_MAJOR_VERSION, CHR63200A_MINOR_VERSION, 
             IVI_COMPILER_NAME, IVI_COMPILER_VER_NUM, 
             IVI_ENGINE_MAJOR_VERSION + IVI_ENGINE_MINOR_VERSION/1000.0, 
             Ivi_ConvertVISAVer(VI_SPEC_VERSION));

    checkErr( Ivi_SetValInStringCallback (vi, attributeId, driverRevision));    
Error:
    return error;
}

/*- CHR63200A_ATTR_INSTRUMENT_FIRMWARE_REVISION -*/

static ViStatus _VI_FUNC chr63200AAttrFirmwareRevision_ReadCallback (ViSession vi, 
                                                                    ViSession io,
                                                                    ViConstString channelName, 
                                                                    ViAttr attributeId,
                                                                    const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      idQ[BUFFER_SIZE], rdBuffer[BUFFER_SIZE];
    
    checkErr( Ivi_GetAttributeViString (vi, "", CHR63200A_ATTR_ID_QUERY_RESPONSE,
                                        0, BUFFER_SIZE, idQ));

	Scan (idQ, "%s>%s[dxt44]%s[dxt44]%s[dxt44]%s", rdBuffer);

    checkErr( Ivi_SetValInStringCallback (vi, attributeId, rdBuffer));
    
Error:
    return error;
}

/*- CHR63200A_ATTR_INSTRUMENT_MANUFACTURER -*/

static ViStatus _VI_FUNC chr63200AAttrInstrumentManufacturer_ReadCallback (ViSession vi, 
                                                                          ViSession io,
                                                                          ViConstString channelName, 
                                                                          ViAttr attributeId,
                                                                          const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];
    ViChar      idQ[BUFFER_SIZE];
    
    checkErr( Ivi_GetAttributeViString (vi, "", CHR63200A_ATTR_ID_QUERY_RESPONSE,
                                        0, BUFFER_SIZE, idQ));
    Scan (idQ, "%s>%s[xt44]%s[d]", rdBuffer);

    checkErr( Ivi_SetValInStringCallback (vi, attributeId, rdBuffer));
    
Error:
    return error;
}

/*- CHR63200A_ATTR_INSTRUMENT_MODEL -*/

static ViStatus _VI_FUNC chr63200AAttrInstrumentModel_ReadCallback (ViSession vi, 
                                                                   ViSession io,
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId,
                                                                   const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];
    ViChar      idQ[BUFFER_SIZE];
    
    checkErr( Ivi_GetAttributeViString (vi, "", CHR63200A_ATTR_ID_QUERY_RESPONSE,
                                        0, BUFFER_SIZE, idQ));
    Scan (idQ, "%s>%s[dxt44]%s[xt44]%s[d]", rdBuffer);

    checkErr( Ivi_SetValInStringCallback (vi, attributeId, rdBuffer));
    
Error:
    return error;
}

/*- chr63200AAttrAdvAutoIset_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvAutoIset_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"AUTO:ISET?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvAutoIset_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvAutoIset_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"AUTO:ISET %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvAutoPset_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvAutoPset_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"AUTO:PSET?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvAutoPset_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvAutoPset_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"AUTO:PSET %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvAutoRset_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvAutoRset_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"AUTO:RSET?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvAutoRset_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvAutoRset_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"AUTO:RSET %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvAutoVset_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvAutoVset_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"AUTO:VSET?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvAutoVset_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvAutoVset_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"AUTO:VSET %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvBattEndv_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvBattEndv_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"BATT:ENDV?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvBattEndv_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvBattEndv_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"BATT:ENDV %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvBattFall_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvBattFall_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"BATT:FALL?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvBattFall_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvBattFall_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"BATT:FALL %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvBattMode_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvBattMode_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE];
	IviRangeTablePtr	tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr( viQueryf (io,"BATT:MODE?\n","%s",szRes));
	Scan(szRes,"%s>%s[xt40]",szRes);
	viCheckErr(Ivi_GetViInt32EntryFromString(szRes,tblPtr,value,VI_NULL,VI_NULL,VI_NULL,VI_NULL));

	Error:
	return error;
}

/*- chr63200AAttrAdvBattMode_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvBattMode_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	szValue;
	IviRangeTablePtr    tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr(Ivi_GetViInt32EntryFromValue(value,tblPtr,VI_NULL,VI_NULL,VI_NULL,VI_NULL,&szValue,VI_NULL));
	viCheckErr( viPrintf (io, "BATT:MODE %s\n",szValue));
	
Error:
	return error;
}

/*- chr63200AAttrAdvBattRise_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvBattRise_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"BATT:RISE?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvBattRise_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvBattRise_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"BATT:RISE %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvBattTout_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvBattTout_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"BATT:TOUT?\n","%d",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvBattTout_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvBattTout_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"BATT:TOUT %d\n",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvBattVal_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvBattVal_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"BATT:VAL?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvBattVal_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvBattVal_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"BATT:VAL %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvCrccIset_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvCrccIset_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"CRCC:ISET?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvCrccIset_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvCrccIset_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"CRCC:ISET %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvCrccRset_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvCrccRset_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"CRCC:RSET?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvCrccRset_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvCrccRset_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"CRCC:RSET %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvCvccIset_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvCvccIset_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"CVCC:ISET?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvCvccIset_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvCvccIset_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"CVCC:ISET %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvCvccRes_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvCvccRes_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE];
	IviRangeTablePtr	tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr( viQueryf (io,"CVCC:RES?\n","%s",szRes));
	Scan(szRes,"%s>%s[xt40]",szRes);
	viCheckErr(Ivi_GetViInt32EntryFromString(szRes,tblPtr,value,VI_NULL,VI_NULL,VI_NULL,VI_NULL));

	Error:
	return error;
}

/*- chr63200AAttrAdvCvccRes_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvCvccRes_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	szValue;
	IviRangeTablePtr    tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr(Ivi_GetViInt32EntryFromValue(value,tblPtr,VI_NULL,VI_NULL,VI_NULL,VI_NULL,&szValue,VI_NULL));
	viCheckErr( viPrintf (io, "CVCC:RES %s\n",szValue));
	
Error:
	return error;
}

/*- chr63200AAttrAdvCvccVset_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvCvccVset_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"CVCC:VSET?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvCvccVset_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvCvccVset_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"CVCC:VSET %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvCvcrRset_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvCvcrRset_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"CVCR:RSET?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvCvcrRset_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvCvcrRset_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"CVCR:RSET %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvCvcrVset_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvCvcrVset_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"CVCR:VSET?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvCvcrVset_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvCvcrVset_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"CVCR:VSET %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvExtWavMode_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvExtWavMode_ReadCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE];
	IviRangeTablePtr	tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr( viQueryf (io,"EXT:WAV:MODE?\n","%s",szRes));
	Scan(szRes,"%s>%s[xt40]",szRes);
	viCheckErr(Ivi_GetViInt32EntryFromString(szRes,tblPtr,value,VI_NULL,VI_NULL,VI_NULL,VI_NULL));

	Error:
	return error;
}

/*- chr63200AAttrAdvExtWavMode_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvExtWavMode_WriteCallback (ViSession vi,
                                                                   ViSession io,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	szValue;
	IviRangeTablePtr    tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr(Ivi_GetViInt32EntryFromValue(value,tblPtr,VI_NULL,VI_NULL,VI_NULL,VI_NULL,&szValue,VI_NULL));
	viCheckErr( viPrintf (io, "EXT:WAV:MODE %s\n",szValue));
	
Error:
	return error;
}

/*- chr63200AAttrAdvExtWavRng_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvExtWavRng_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE]="";
	ViInt32		iExtMode=-1;
	IviRangeTablePtr	tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	
	chr63200A_GetAttributeViInt32 (vi, "", CHR63200A_ATTR_ADV_EXT_WAV_MODE,&iExtMode );
	switch(iExtMode)
	{
		case CHR63200A_VAL_EXTM_CC:
			viCheckErr( viQueryf (io,"EXT:WAV:CC:VRNG?\n","%s",szRes));
			break;
		case CHR63200A_VAL_EXTM_CR:
			viCheckErr( viQueryf (io,"EXT:WAV:CR:IRNG?\n","%s",szRes));
			break;
		case CHR63200A_VAL_EXTM_CV:
			viCheckErr( viQueryf (io,"EXT:WAV:CV:IRNG?\n","%s",szRes));
			break;
		default:
			break;
	}
	
	Scan(szRes,"%s>%s[xt40]",szRes);
	viCheckErr(Ivi_GetViInt32EntryFromString(szRes,tblPtr,value,VI_NULL,VI_NULL,VI_NULL,VI_NULL));

Error:
	return error;
}

/*- chr63200AAttrAdvExtWavRng_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvExtWavRng_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	szValue;
	ViInt32		iExtMode=-1;
	IviRangeTablePtr    tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	
	viCheckErr(Ivi_GetViInt32EntryFromValue(value,tblPtr,VI_NULL,VI_NULL,VI_NULL,VI_NULL,&szValue,VI_NULL));
	
	chr63200A_GetAttributeViInt32 (vi, "", CHR63200A_ATTR_ADV_EXT_WAV_MODE,&iExtMode );
	
	switch(iExtMode)
	{
		case CHR63200A_VAL_EXTM_CC:
			viCheckErr( viPrintf (io, "EXT:WAV:CC:VRNG %s\n",szValue));
			break;
		case CHR63200A_VAL_EXTM_CR:
			viCheckErr( viPrintf (io, "EXT:WAV:CR:IRNG %s\n",szValue));
			break;
		case CHR63200A_VAL_EXTM_CV:
			viCheckErr( viPrintf (io, "EXT:WAV:CV:IRNG %s\n",szValue));
			break;
		default:
			break;
	}
	
Error:
	return error;
}

/*- chr63200AAttrAdvOcpDwel_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvOcpDwel_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"OCP:DWEL?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvOcpDwel_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvOcpDwel_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"OCP:DWEL %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvOcpEnd_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvOcpEnd_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"OCP:END?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvOcpEnd_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvOcpEnd_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"OCP:END %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvOcpLatc_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvOcpLatc_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE];
	IviRangeTablePtr	tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr( viQueryf (io,"OCP:LATC?\n","%s",szRes));
	Scan(szRes,"%s>%s[xt40]",szRes);
	viCheckErr(Ivi_GetViInt32EntryFromString(szRes,tblPtr,value,VI_NULL,VI_NULL,VI_NULL,VI_NULL));

	Error:
	return error;
}

/*- chr63200AAttrAdvOcpLatc_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvOcpLatc_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	szValue;
	IviRangeTablePtr    tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr(Ivi_GetViInt32EntryFromValue(value,tblPtr,VI_NULL,VI_NULL,VI_NULL,VI_NULL,&szValue,VI_NULL));
	viCheckErr( viPrintf (io, "OCP:LATC %s\n",szValue));
	
Error:
	return error;
}

/*- chr63200AAttrAdvOcpRes_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvOcpRes_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              const ViConstString cacheValue)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE];
	viCheckErr( viQueryf (io,"OCP:RES?\n","%s",szRes));
	checkErr(Ivi_SetValInStringCallback(vi,attributeId,szRes));
	Error:
	return error;
}

/*- chr63200AAttrAdvOcpSpecH_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvOcpSpecH_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"OCP:SPEC:H?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvOcpSpecH_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvOcpSpecH_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"OCP:SPEC:H %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvOcpSpecL_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvOcpSpecL_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"OCP:SPEC:L?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvOcpSpecL_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvOcpSpecL_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"OCP:SPEC:L %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvOcpStar_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvOcpStar_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"OCP:STAR?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvOcpStar_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvOcpStar_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"OCP:STAR %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvOcpStep_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvOcpStep_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"OCP:STEP?\n","%d",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvOcpStep_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvOcpStep_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"OCP:STEP %d\n",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvOcpTrigVolt_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvOcpTrigVolt_ReadCallback (ViSession vi,
                                                                   ViSession io,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"OCP:TRIG:VOLT?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvOcpTrigVolt_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvOcpTrigVolt_WriteCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"OCP:TRIG:VOLT %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvOppDwel_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvOppDwel_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"OPP:DWEL?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvOppDwel_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvOppDwel_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"OPP:DWEL %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvOppEnd_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvOppEnd_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"OPP:END?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvOppEnd_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvOppEnd_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"OPP:END %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvOppLatc_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvOppLatc_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE];
	IviRangeTablePtr	tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr( viQueryf (io,"OPP:LATC?\n","%s",szRes));
	Scan(szRes,"%s>%s[xt40]",szRes);
	viCheckErr(Ivi_GetViInt32EntryFromString(szRes,tblPtr,value,VI_NULL,VI_NULL,VI_NULL,VI_NULL));

	Error:
	return error;
}

/*- chr63200AAttrAdvOppLatc_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvOppLatc_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	szValue;
	IviRangeTablePtr    tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr(Ivi_GetViInt32EntryFromValue(value,tblPtr,VI_NULL,VI_NULL,VI_NULL,VI_NULL,&szValue,VI_NULL));
	viCheckErr( viPrintf (io, "OPP:LATC %s\n",szValue));
	
Error:
	return error;
}

/*- chr63200AAttrAdvOppRes_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvOppRes_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              const ViConstString cacheValue)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE];
	viCheckErr( viQueryf (io,"OPP:RES?\n","%s",szRes));
	checkErr(Ivi_SetValInStringCallback(vi,attributeId,szRes));
	Error:
	return error;
}

/*- chr63200AAttrAdvOppSpecH_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvOppSpecH_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"OPP:SPEC:H?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvOppSpecH_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvOppSpecH_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"OPP:SPEC:H %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvOppSpecL_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvOppSpecL_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"OPP:SPEC:L?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvOppSpecL_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvOppSpecL_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"OPP:SPEC:L %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvOppStar_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvOppStar_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"OPP:STAR?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvOppStar_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvOppStar_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"OPP:STAR %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvOppStep_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvOppStep_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"OPP:STEP?\n","%d",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvOppStep_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvOppStep_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"OPP:STEP %d\n",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvOppTrigVolt_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvOppTrigVolt_ReadCallback (ViSession vi,
                                                                   ViSession io,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"OPP:TRIG:VOLT?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvOppTrigVolt_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvOppTrigVolt_WriteCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"OPP:TRIG:VOLT %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvSineFreq_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvSineFreq_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"SINE:FREQ?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvSineFreq_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvSineFreq_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"SINE:FREQ %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvSineIac_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvSineIac_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"SINE:IAC?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvSineIac_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvSineIac_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"SINE:IAC %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvSineIdc_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvSineIdc_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"SINE:IDC?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvSineIdc_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvSineIdc_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"SINE:IDC %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvUserWavCle_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvUserWavCle_ReadCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViInt32	iWFNum=-1;
	
	checkErr(chr63200A_GetAttributeViInt32 (vi,"", CHR63200A_ATTR_ADV_USER_WAV_NSEL,&iWFNum));
	viCheckErr( viQueryf (io,"ADV:USER:WAV:CLE? %d\n","%d",iWFNum,value));

Error:
	return error;
}

/*- chr63200AAttrAdvUserWavDataPoin_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvUserWavDataPoin_ReadCallback (ViSession vi,
                                                                       ViSession io,
                                                                       ViConstString channelName,
                                                                       ViAttr attributeId,
                                                                       const ViConstString cacheValue)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE];

	viCheckErr( viQueryf (io,"USER:WAV:DATA:POIN?\n","%s",szRes));
	checkErr(Ivi_SetValInStringCallback(vi,attributeId,szRes));

Error:
	return error;
}

/*- chr63200AAttrAdvUserWavDataPoin_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvUserWavDataPoin_WriteCallback (ViSession vi,
                                                                        ViSession io,
                                                                        ViConstString channelName,
                                                                        ViAttr attributeId,
                                                                        ViConstString value)
{
	ViStatus	error = VI_SUCCESS;
	
	ViChar		szUWNSEL[4]= {VI_NULL};
	ViChar		szUWCmdVal[32],szTmp[BUFFER_SIZE];
	ViUInt16	*uiaUDWFD;
	ViInt16		iLen=0;
	ViInt32		iFlag,retStat;
	ViUInt8		*PointByte;
	ViUInt16	SumOfData=0, SumOfTemp=0,ChkSum[2],szEndChar[1];;
	ViUInt32	i,retCnt,iWFPoint;
	ViByte		*outputWFD;
	ViReal64	fLimit=-1;
	szEndChar[0]=0x0A;
	
	viQueryf(io,"%s\n","%s","ADV:USER:WAV:NSEL?",szUWNSEL);

	if(atoi(szUWNSEL)<10)
	{
		szUWNSEL[1]=szUWNSEL[0];
		szUWNSEL[0]='0';
	}
	
	viQueryf(io,"CURR:STAT:L1? MAX\n","%Lf",&fLimit);
	sprintf(szTmp,"%s",value);  
	
	uiaUDWFD = (ViUInt16 *)calloc(BUFFER_SIZE_Max,sizeof(ViUInt16));
	chr63200A_ConvertWFD(fLimit,szTmp,uiaUDWFD,&iWFPoint);
	PointByte = (ViUInt8 *)calloc(2*iWFPoint,sizeof(ViUInt8)); 			/*2Byte per point*/
	for (i=0; i < iWFPoint; i++)
	{
		PointByte[2*i] = uiaUDWFD[i] & 0x00FF;		/*low*/
		PointByte[2*i+1] = uiaUDWFD[i] >>8;		/*high*/
		SumOfTemp += (PointByte[2*i]+PointByte[2*i+1]);
	}
	free(uiaUDWFD); 
	
	/*2's Complement*/
	SumOfData = (~SumOfTemp)+1;
	*ChkSum = SumOfData;
	
	Fmt(szTmp,"%s<%i",(iWFPoint*2) + 2);
	iLen=(ViInt16)strlen(szUWNSEL) + (ViInt16)strlen(szTmp);		

	Fmt(szUWCmdVal,"USER:WAV:DATA:POIN #%d%s%s",iLen,szUWNSEL,szTmp);
	outputWFD = (ViByte *)calloc(BUFFER_SIZE_Max,sizeof(ViByte));
	
	ViInt32 istrLen=0;
	memcpy(outputWFD, szUWCmdVal, strlen(szUWCmdVal));
	istrLen += strlen(szUWCmdVal);
	memcpy(outputWFD + istrLen, PointByte, iWFPoint * 2);
	istrLen += iWFPoint*2;
	free(PointByte);
	memcpy(outputWFD + istrLen, ChkSum, 2);
	istrLen += 2;
	memcpy(outputWFD + istrLen, szEndChar, 1);
	istrLen += 1;

	
	viCheckErr( viWrite( io, outputWFD, istrLen, &retCnt));
	free(outputWFD);
	
	do
	{
		iFlag = 0;
		viCheckErr( viQueryf( io, "USER:WAV:DATA:STAT?\n", "%d", &retStat));
						/*Data status: 0:idle, 1:processing, 2:finish, 3~6:transfer error*/
		
		if (retStat == 1)
		{
			iFlag = 1;
			Delay(DELAY_TIME);
		}
		else if(retStat>2)
		{
			if (retStat == 3) sprintf(szTmp,"Error : 3�GData Format Error");
			else if (retStat == 4) sprintf(szTmp,"Error : 4�GData Length Error");
			else if (retStat == 5) sprintf(szTmp,"Error : 5�GOver limit of waveform data");
			else if (retStat == 6) sprintf(szTmp,"Error : 6�GChkSum Error");
		
			viCheckErrElab(CHR63200A_ERROR_WFDATA_STATUS,szTmp);
		}

	} while (iFlag == 1);

				  
Error:
	
	return error;
}

/*- chr63200AAttrAdvUserWavDataStat_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvUserWavDataStat_ReadCallback (ViSession vi,
                                                                       ViSession io,
                                                                       ViConstString channelName,
                                                                       ViAttr attributeId,
                                                                       ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"USER:WAV:DATA:STAT?\n","%d",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvUserWavData_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvUserWavData_ReadCallback (ViSession vi,
                                                                   ViSession io,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   const ViConstString cacheValue)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE];
	ViInt32	iWFNum=-1;
	
	checkErr(chr63200A_GetAttributeViInt32 (vi,"", CHR63200A_ATTR_ADV_USER_WAV_NSEL,&iWFNum));
	viCheckErr( viQueryf (io,"USER:WAV:DATA? %d\n","%s",szRes));
	checkErr(Ivi_SetValInStringCallback(vi,attributeId,szRes));
	Error:
	return error;
}

/*- chr63200AAttrAdvUserWavData_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvUserWavData_WriteCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViConstString value)
{
	ViStatus	error = VI_SUCCESS;
	viCheckErr( viPrintf (io,"USER:WAV:DATA %s\n",value));
	Error:
	return error;
}

/*- chr63200AAttrAdvUserWavExeStat_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvUserWavExeStat_ReadCallback (ViSession vi,
                                                                      ViSession io,
                                                                      ViConstString channelName,
                                                                      ViAttr attributeId,
                                                                      ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"USER:WAV:EXE:STAT?\n","%d",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvUserWavNsel_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvUserWavNsel_ReadCallback (ViSession vi,
                                                                   ViSession io,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"ADV:USER:WAV:NSEL?\n","%d",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvUserWavNsel_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvUserWavNsel_WriteCallback (ViSession vi,
                                                                    ViSession io,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"ADV:USER:WAV:NSEL %d\n",value));

	Error:
	return error;
}

/*- chr63200AAttrAdvUserWavRem_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvUserWavRem_ReadCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViInt32	iWFNum=-1;
	
	checkErr(chr63200A_GetAttributeViInt32 (vi,"", CHR63200A_ATTR_ADV_USER_WAV_NSEL,&iWFNum));
	viCheckErr( viQueryf (io,"ADV:USER:WAV:REM? %d\n","%d",iWFNum,value));

Error:
	return error;
}

/*- chr63200AAttrConfAutoOn_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrConfAutoOn_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE];
	IviRangeTablePtr	tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr( viQueryf (io,"CONF:AUTO:ON?\n","%s",szRes));
	Scan(szRes,"%s>%s[xt40]",szRes); 
	viCheckErr(Ivi_GetViInt32EntryFromString(szRes,tblPtr,value,VI_NULL,VI_NULL,VI_NULL,VI_NULL));

	Error:
	return error;
}

/*- chr63200AAttrConfAutoOn_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrConfAutoOn_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	szValue;
	IviRangeTablePtr    tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr(Ivi_GetViInt32EntryFromValue(value,tblPtr,VI_NULL,VI_NULL,VI_NULL,VI_NULL,&szValue,VI_NULL));
	viCheckErr( viPrintf (io, "CONF:AUTO:ON %s\n",szValue));
	
Error:
	return error;
}

/*- chr63200AAttrConfDioIn1_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrConfDioIn1_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"CONF:DIO:IN1?\n","%d",value));

	Error:
	return error;
}

/*- chr63200AAttrConfDioIn1_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrConfDioIn1_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"CONF:DIO:IN1 %d\n",value));

	Error:
	return error;
}

/*- chr63200AAttrConfDioIn2_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrConfDioIn2_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"CONF:DIO:IN1?\n","%d",value));

	Error:
	return error;
}

/*- chr63200AAttrConfDioIn2_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrConfDioIn2_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"CONF:DIO:IN1 %d\n",value));

	Error:
	return error;
}

/*- chr63200AAttrConfDioOut1_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrConfDioOut1_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"CONF:DIO:OUT1?\n","%d",value));

	Error:
	return error;
}

/*- chr63200AAttrConfDioOut1_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrConfDioOut1_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"CONF:DIO:OUT1 %d\n",value));

	Error:
	return error;
}

/*- chr63200AAttrConfDioOut2_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrConfDioOut2_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"CONF:DIO:OUT2?\n","%d",value));

	Error:
	return error;
}

/*- chr63200AAttrConfDioOut2_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrConfDioOut2_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"CONF:DIO:OUT2 %d\n",value));

	Error:
	return error;
}

/*- chr63200AAttrConfEntKey_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrConfEntKey_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE];
	IviRangeTablePtr	tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr( viQueryf (io,"CONF:ENT:KEY?\n","%s",szRes));
	Scan(szRes,"%s>%s[xt40]",szRes);
	viCheckErr(Ivi_GetViInt32EntryFromString(szRes,tblPtr,value,VI_NULL,VI_NULL,VI_NULL,VI_NULL));

	Error:
	return error;
}

/*- chr63200AAttrConfEntKey_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrConfEntKey_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	szValue;
	IviRangeTablePtr    tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr(Ivi_GetViInt32EntryFromValue(value,tblPtr,VI_NULL,VI_NULL,VI_NULL,VI_NULL,&szValue,VI_NULL));
	viCheckErr( viPrintf (io, "CONF:ENT:KEY %s\n",szValue));
	
Error:
	return error;
}

/*- chr63200AAttrConfParaInit_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrConfParaInit_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE];
	IviRangeTablePtr	tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr( viQueryf (io,"CONF:PARA:INIT?\n","%s",szRes));
	Scan(szRes,"%s>%s[xt40]",szRes);
	viCheckErr(Ivi_GetViInt32EntryFromString(szRes,tblPtr,value,VI_NULL,VI_NULL,VI_NULL,VI_NULL));

	Error:
	return error;
}

/*- chr63200AAttrConfParaInit_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrConfParaInit_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	szValue;
	IviRangeTablePtr	tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr(Ivi_GetViInt32EntryFromValue(value,tblPtr,VI_NULL,VI_NULL,VI_NULL,VI_NULL,&szValue,VI_NULL));
	viCheckErr( viPrintf (io, "CONF:PARA:INIT %s\n",szValue));
	
Error:
	return error;
}

/*- chr63200AAttrConfParaMode_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrConfParaMode_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE];
	IviRangeTablePtr	tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr( viQueryf (io,"CONF:PARA:MODE?\n","%s",szRes));
	Scan(szRes,"%s>%s[xt40]",szRes);
	viCheckErr(Ivi_GetViInt32EntryFromString(szRes,tblPtr,value,VI_NULL,VI_NULL,VI_NULL,VI_NULL));

	Error:
	return error;
}

/*- chr63200AAttrConfParaMode_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrConfParaMode_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	szValue;
	IviRangeTablePtr	tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr(Ivi_GetViInt32EntryFromValue(value,tblPtr,VI_NULL,VI_NULL,VI_NULL,VI_NULL,&szValue,VI_NULL));
	viCheckErr( viPrintf (io, "CONF:PARA:MODE %s\n",szValue));
	
Error:
	return error;
}

/*- chr63200AAttrConfShorKey_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrConfShorKey_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE];
	IviRangeTablePtr	tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr( viQueryf (io,"CONF:SHOR:KEY?\n","%s",szRes));
	Scan(szRes,"%s>%s[xt40]",szRes);
	viCheckErr(Ivi_GetViInt32EntryFromString(szRes,tblPtr,value,VI_NULL,VI_NULL,VI_NULL,VI_NULL));

	Error:
	return error;
}

/*- chr63200AAttrConfShorKey_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrConfShorKey_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	szValue;
	IviRangeTablePtr    tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr(Ivi_GetViInt32EntryFromValue(value,tblPtr,VI_NULL,VI_NULL,VI_NULL,VI_NULL,&szValue,VI_NULL));
	viCheckErr( viPrintf (io, "CONF:SHOR:KEY %s\n",szValue));
	
Error:
	return error;
}

/*- chr63200AAttrConfSoun_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrConfSoun_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE];
	IviRangeTablePtr	tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr( viQueryf (io,"CONF:SOUN?\n","%s",szRes));
	Scan(szRes,"%s>%s[xt40]",szRes);
	viCheckErr(Ivi_GetViInt32EntryFromString(szRes,tblPtr,value,VI_NULL,VI_NULL,VI_NULL,VI_NULL));

	Error:
	return error;
}

/*- chr63200AAttrConfSoun_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrConfSoun_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	szValue;
	IviRangeTablePtr    tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr(Ivi_GetViInt32EntryFromValue(value,tblPtr,VI_NULL,VI_NULL,VI_NULL,VI_NULL,&szValue,VI_NULL));
	viCheckErr( viPrintf (io, "CONF:SOUN %s\n",szValue));
	
Error:
	return error;
}

/*- chr63200AAttrConfSyncMode_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrConfSyncMode_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE];
	IviRangeTablePtr	tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr( viQueryf (io,"CONF:SYNC:MODE?\n","%s",szRes));
	Scan(szRes,"%s>%s[xt40]",szRes);
	viCheckErr(Ivi_GetViInt32EntryFromString(szRes,tblPtr,value,VI_NULL,VI_NULL,VI_NULL,VI_NULL));

	Error:
	return error;
}

/*- chr63200AAttrConfSyncMode_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrConfSyncMode_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	szValue;
	IviRangeTablePtr    tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr(Ivi_GetViInt32EntryFromValue(value,tblPtr,VI_NULL,VI_NULL,VI_NULL,VI_NULL,&szValue,VI_NULL));
	viCheckErr( viPrintf (io, "CONF:SYNC:MODE %s\n",szValue));
	
Error:
	return error;
}

/*- chr63200AAttrConfVoltLatc_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrConfVoltLatc_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE];
	IviRangeTablePtr    tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr( viQueryf (io,"CONF:VOLT:LATC?\n","%s",szRes));
	Scan(szRes,"%s>%s[xt40]",szRes);
	viCheckErr(Ivi_GetViInt32EntryFromString(szRes,tblPtr,value,VI_NULL,VI_NULL,VI_NULL,VI_NULL));

	Error:
	return error;
}

/*- chr63200AAttrConfVoltLatc_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrConfVoltLatc_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	szValue;
	IviRangeTablePtr	tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr(Ivi_GetViInt32EntryFromValue(value,tblPtr,VI_NULL,VI_NULL,VI_NULL,VI_NULL,&szValue,VI_NULL));
	viCheckErr( viPrintf (io, "CONF:VOLT:LATC %s\n",szValue));
	
Error:
	return error;
}

/*- chr63200AAttrConfVoltOff_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrConfVoltOff_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viQueryf (io,"CONF:VOLT:OFF?\n","%f",&value));
	
	Error:
	return error;
}

/*- chr63200AAttrConfVoltOff_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrConfVoltOff_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "CONF:VOLT:OFF %f\n",value));
	
Error:
	return error;
}

/*- chr63200AAttrConfVoltOn_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrConfVoltOn_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viQueryf (io,"CONF:VOLT:ON?\n","%f",&value));
	
	Error:
	return error;
}

/*- chr63200AAttrConfVoltOn_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrConfVoltOn_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "CONF:VOLT:ON %f\n",value));
	
Error:
	return error;
}

/*- chr63200AAttrConfVoltSign_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrConfVoltSign_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE];
	IviRangeTablePtr	tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr( viQueryf (io,"CONF:VOLT:SIGN?\n","%s",szRes));
	Scan(szRes,"%s>%s[xt40]",szRes);
	viCheckErr(Ivi_GetViInt32EntryFromString(szRes,tblPtr,value,VI_NULL,VI_NULL,VI_NULL,VI_NULL));

	Error:
	return error;
}

/*- chr63200AAttrConfVoltSign_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrConfVoltSign_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 value)									   
{
	ViStatus	error = VI_SUCCESS;
	ViString 	szValue;
	IviRangeTablePtr	tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr(Ivi_GetViInt32EntryFromValue(value,tblPtr,VI_NULL,VI_NULL,VI_NULL,VI_NULL,&szValue,VI_NULL));
	viCheckErr( viPrintf (io, "CONF:VOLT:SIGN %s\n",szValue));
	
Error:
	return error;
}

/*- chr63200AAttrConfWind_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrConfWind_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"CONF:WIND?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrConfWind_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrConfWind_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "CONF:WIND %f\n",value));
	
Error:
	return error;
}

/*- chr63200AAttrCurrDynFall_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrDynFall_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"CURR:DYN:FALL?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrCurrDynFall_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrDynFall_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"CURR:DYN:FALL %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrCurrDynL1_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrDynL1_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"CURR:DYN:L1?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrCurrDynL1_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrDynL1_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"CURR:DYN:L1 %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrCurrDynL2_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrDynL2_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"CURR:DYN:L2?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrCurrDynL2_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrDynL2_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"CURR:DYN:L2 %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrCurrDynRep_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrDynRep_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"CURR:DYN:REP?\n","%d",value));

	Error:
	return error;
}

/*- chr63200AAttrCurrDynRep_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrDynRep_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"CURR:DYN:REP %d\n",value));

	Error:
	return error;
}

/*- chr63200AAttrCurrDynRise_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrDynRise_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"CURR:DYN:RISE?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrCurrDynRise_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrDynRise_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"CURR:DYN:RISE %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrCurrDynT1_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrDynT1_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"CURR:DYN:T1?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrCurrDynT1_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrDynT1_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"CURR:DYN:T1 %fms\n",value));

	Error:
	return error;
}

/*- chr63200AAttrCurrDynT2_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrDynT2_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"CURR:DYN:T2?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrCurrDynT2_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrDynT2_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"CURR:DYN:T2 %fms\n",value));

	Error:
	return error;
}

/*- chr63200AAttrCurrDynVrng_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrDynVrng_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE];
	IviRangeTablePtr	tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr( viQueryf (io,"CURR:STAT:VRNG?\n","%s",szRes));
	Scan(szRes,"%s>%s[xt40]",szRes);
	viCheckErr(Ivi_GetViInt32EntryFromString(szRes,tblPtr,value,VI_NULL,VI_NULL,VI_NULL,VI_NULL));

	Error:
	return error;
}

/*- chr63200AAttrCurrDynVrng_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrDynVrng_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	szValue;
	IviRangeTablePtr    tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr(Ivi_GetViInt32EntryFromValue(value,tblPtr,VI_NULL,VI_NULL,VI_NULL,VI_NULL,&szValue,VI_NULL));
	viCheckErr( viPrintf (io, "CURR:STAT:VRNG %s\n",szValue));
	
Error:
	return error;
}

/*- chr63200AAttrCurrStatFall_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrStatFall_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viQueryf (io,"CURR:STAT:FALL?\n","%f",&value));
	
	Error:
	return error;
}

/*- chr63200AAttrCurrStatFall_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrStatFall_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "CURR:STAT:FALL %f\n",value));
	
Error:
	return error;
}

/*- chr63200AAttrCurrStatL1_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrStatL1_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viQueryf (io,"CURR:STAT:L1?\n","%f",&value));
	
	Error:
	return error;
}

/*- chr63200AAttrCurrStatL1_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrStatL1_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "CURR:STAT:L1 %f\n",value));
	
Error:
	return error;
}

/*- chr63200AAttrCurrStatL2_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrStatL2_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viQueryf (io,"CURR:STAT:L2?\n","%f",&value));
	
	Error:
	return error;
}

/*- chr63200AAttrCurrStatL2_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrStatL2_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "CURR:STAT:L2 %f\n",value));
	
Error:
	return error;
}

/*- chr63200AAttrCurrStatRise_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrStatRise_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viQueryf (io,"CURR:STAT:RISE?\n","%f",&value));
	
	Error:
	return error;
}

/*- chr63200AAttrCurrStatRise_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrStatRise_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viPrintf (io, "CURR:STAT:RISE %f\n",value));
	
Error:
	return error;
}

/*- chr63200AAttrCurrStatVrng_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrStatVrng_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE];
	IviRangeTablePtr	tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr( viQueryf (io,"CURR:STAT:VRNG?\n","%s",szRes));
	Scan(szRes,"%s>%s[xt40]",szRes);
	viCheckErr(Ivi_GetViInt32EntryFromString(szRes,tblPtr,value,VI_NULL,VI_NULL,VI_NULL,VI_NULL));

	Error:
	return error;
}

/*- chr63200AAttrCurrStatVrng_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrStatVrng_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	szValue;
	IviRangeTablePtr    tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr(Ivi_GetViInt32EntryFromValue(value,tblPtr,VI_NULL,VI_NULL,VI_NULL,VI_NULL,&szValue,VI_NULL));
	viCheckErr( viPrintf (io, "CURR:STAT:VRNG %s\n",szValue));
	
Error:
	return error;
}

/*- chr63200AAttrCurrSweDuty_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrSweDuty_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"CURR:SWE:DUTY?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrCurrSweDuty_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrSweDuty_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"CURR:SWE:DUTY %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrCurrSweDwel_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrSweDwel_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"CURR:SWE:DWEL?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrCurrSweDwel_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrSweDwel_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"CURR:SWE:DWEL %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrCurrSweFall_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrSweFall_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"CURR:SWE:FALL?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrCurrSweFall_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrSweFall_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"CURR:SWE:FALL %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrCurrSweFend_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrSweFend_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"CURR:SWE:FEND?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrCurrSweFend_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrSweFend_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"CURR:SWE:FEND %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrCurrSweFsta_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrSweFsta_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"CURR:SWE:FSTA?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrCurrSweFsta_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrSweFsta_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"CURR:SWE:FSTA %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrCurrSweFstep_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrSweFstep_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"CURR:SWE:FSTE?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrCurrSweFstep_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrSweFstep_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"CURR:SWE:FSTE %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrCurrSweImax_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrSweImax_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"CURR:SWE:IMAX?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrCurrSweImax_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrSweImax_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"CURR:SWE:IMAX %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrCurrSweImin_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrSweImin_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"CURR:SWE:IMIN?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrCurrSweImin_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrSweImin_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"CURR:SWE:IMIN %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrCurrSweRise_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrSweRise_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"CURR:SWE:RISE?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrCurrSweRise_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrSweRise_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"CURR:SWE:RISE %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrDigSampPoin_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrDigSampPoin_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"DIG:SAMP:POIN?\n","%f",value)); 

	Error:
	return error;
}

/*- chr63200AAttrDigSampPoin_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrDigSampPoin_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"DIG:SAMP:POIN %d\n",value));


	Error:
	return error;
}

/*- chr63200AAttrDigSampTime_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrDigSampTime_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"DIG:SAMP:TIME?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrDigSampTime_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrDigSampTime_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"DIG:SAMP:TIME %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrDigTrigPoin_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrDigTrigPoin_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"DIG:TRIG:POIN?\n","%d",value));

	Error:
	return error;
}

/*- chr63200AAttrDigTrigPoin_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrDigTrigPoin_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"DIG:TRIG:POIN %d\n",value));

	Error:
	return error;
}

/*- chr63200AAttrDigTrigSour_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrDigTrigSour_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE];
	IviRangeTablePtr	tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr( viQueryf (io,"DIG:TRIG:SOUR?\n","%s",szRes));
	Scan(szRes,"%s>%s[xt40]",szRes);
	viCheckErr(Ivi_GetViInt32EntryFromString(szRes,tblPtr,value,VI_NULL,VI_NULL,VI_NULL,VI_NULL));

	Error:
	return error;
}

/*- chr63200AAttrDigTrigSour_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrDigTrigSour_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	szValue;
	IviRangeTablePtr    tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr(Ivi_GetViInt32EntryFromValue(value,tblPtr,VI_NULL,VI_NULL,VI_NULL,VI_NULL,&szValue,VI_NULL));
	viCheckErr( viPrintf (io, "DIG:TRIG:SOUR %s\n",szValue));
	
Error:
	return error;
}

/*- chr63200AAttrDigTrig_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrDigTrig_ReadCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE];
	IviRangeTablePtr	tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr( viQueryf (io,"DIG:TRIG?\n","%s",szRes));
	viCheckErr(Ivi_GetViInt32EntryFromString(szRes,tblPtr,value,VI_NULL,VI_NULL,VI_NULL,VI_NULL));

	Error:
	return error;
}

/*- chr63200AAttrDigTrig_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrDigTrig_WriteCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	szValue;
	IviRangeTablePtr    tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	if(value < CHR63200A_VAL_DIGI_TRIG_OFF ||  value > CHR63200A_VAL_DIGI_TRIG_ON)
		checkErr(IVI_ERROR_INVALID_VALUE);
	viCheckErr(Ivi_GetViInt32EntryFromValue(value,tblPtr,VI_NULL,VI_NULL,VI_NULL,VI_NULL,&szValue,VI_NULL));
	viCheckErr( viPrintf (io, "DIG:TRIG %s\n",szValue));
	
Error:
	return error;
}

/*- chr63200AAttrDigWavCap_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrDigWavCap_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              const ViConstString cacheValue)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE];
	viCheckErr( viQueryf (io,"DIG:WAV:CAP?\n","%s",szRes));
	checkErr(Ivi_SetValInStringCallback(vi,attributeId,szRes));
	Error:
	return error;
}

/*- chr63200AAttrDioOut1_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrDioOut1_ReadCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE];
	IviRangeTablePtr	tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr( viQueryf (io,"DIO:OUT1?\n","%s",szRes));
	viCheckErr(Ivi_GetViInt32EntryFromString(szRes,tblPtr,value,VI_NULL,VI_NULL,VI_NULL,VI_NULL));

	Error:
	return error;
}

/*- chr63200AAttrDioOut1_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrDioOut1_WriteCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	szValue;
	IviRangeTablePtr    tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr(Ivi_GetViInt32EntryFromValue(value,tblPtr,VI_NULL,VI_NULL,VI_NULL,VI_NULL,&szValue,VI_NULL));
	viCheckErr( viPrintf (io, "DIO:OUT1 %s\n",szValue));
	
Error:
	return error;
}

/*- chr63200AAttrDioOut2_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrDioOut2_ReadCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE];
	IviRangeTablePtr	tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr( viQueryf (io,"DIO:OUT2?\n","%s",szRes));
	viCheckErr(Ivi_GetViInt32EntryFromString(szRes,tblPtr,value,VI_NULL,VI_NULL,VI_NULL,VI_NULL));

	Error:
	return error;
}

/*- chr63200AAttrDioOut2_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrDioOut2_WriteCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	szValue;
	IviRangeTablePtr    tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr(Ivi_GetViInt32EntryFromValue(value,tblPtr,VI_NULL,VI_NULL,VI_NULL,VI_NULL,&szValue,VI_NULL));
	viCheckErr( viPrintf (io, "DIO:OUT2 %s\n",szValue));
	
Error:
	return error;
}

/*- chr63200AAttrImpStatCl_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrImpStatCl_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"IMP:STAT:CL?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrImpStatCl_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrImpStatCl_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"IMP:STAT:CL %fuF\n",value));

	Error:
	return error;
}

/*- chr63200AAttrImpStatLs_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrImpStatLs_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"IMP:STAT:LS?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrImpStatLs_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrImpStatLs_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"IMP:STAT:LS %fuH\n",value));

	Error:
	return error;
}

/*- chr63200AAttrImpStatRl_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrImpStatRl_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"IMP:STAT:RL?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrImpStatRl_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrImpStatRl_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"IMP:STAT:RL %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrImpStatRs_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrImpStatRs_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"IMP:STAT:RS?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrImpStatRs_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrImpStatRs_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"IMP:STAT:RS %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrLoadProt_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrLoadProt_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	
	viCheckErr( viQueryf (io,"LOAD:PROT?\n","%d",value));

	Error:
	return error;
}

/*- chr63200AAttrLoadShor_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrLoadShor_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	
	ViChar		szRes[BUFFER_SIZE];
	IviRangeTablePtr    tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	
	viCheckErr( viQueryf (io,"LOAD:SHOR?\n","%s",szRes));
	viCheckErr(Ivi_GetViInt32EntryFromString(szRes,tblPtr,value,VI_NULL,VI_NULL,VI_NULL,VI_NULL));

	Error:
	return error;
}

/*- chr63200AAttrLoadShor_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrLoadShor_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	szValue;
	IviRangeTablePtr    tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	
	viCheckErr(Ivi_GetViInt32EntryFromValue(value,tblPtr,VI_NULL,VI_NULL,VI_NULL,VI_NULL,&szValue,VI_NULL));
	viCheckErr( viPrintf (io, "LOAD:SHOR %s\n",szValue));
	
Error:
	return error;
}

/*- chr63200AAttrLoad_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrLoad_ReadCallback (ViSession vi,
                                                         ViSession io,
                                                         ViConstString channelName,
                                                         ViAttr attributeId,
                                                         ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	
	ViChar		szRes[BUFFER_SIZE];
	IviRangeTablePtr    tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);	
	viCheckErr( viQueryf (io,"LOAD?\n","%s",szRes));
	viCheckErr(Ivi_GetViInt32EntryFromString(szRes,tblPtr,value,VI_NULL,VI_NULL,VI_NULL,VI_NULL));

	Error:
	return error;
}

/*- chr63200AAttrLoad_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrLoad_WriteCallback (ViSession vi,
                                                          ViSession io,
                                                          ViConstString channelName,
                                                          ViAttr attributeId,
                                                          ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	szValue;
	IviRangeTablePtr    tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);	
	viCheckErr(Ivi_GetViInt32EntryFromValue(value,tblPtr,VI_NULL,VI_NULL,VI_NULL,VI_NULL,&szValue,VI_NULL));
	viCheckErr( viPrintf (io, "LOAD %s\n",szValue));
	
Error:
	return error;
}

/*- chr63200AAttrMode_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrMode_ReadCallback (ViSession vi,
                                                         ViSession io,
                                                         ViConstString channelName,
                                                         ViAttr attributeId,
                                                         ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	
	ViChar		szRes[BUFFER_SIZE];
	IviRangeTablePtr    tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);		
	viCheckErr( viQueryf (io,"MODE?\n","%s",szRes));
	viCheckErr(Ivi_GetViInt32EntryFromString(szRes,tblPtr,value,VI_NULL,VI_NULL,VI_NULL,VI_NULL));

	Error:
	return error;
}

/*- chr63200AAttrMode_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrMode_WriteCallback (ViSession vi,
                                                          ViSession io,
                                                          ViConstString channelName,
                                                          ViAttr attributeId,
                                                          ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	szValue;
	IviRangeTablePtr    tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr(Ivi_GetViInt32EntryFromValue(value,tblPtr,VI_NULL,VI_NULL,VI_NULL,VI_NULL,&szValue,VI_NULL));
	viCheckErr( viPrintf (io, "MODE %s\n",szValue));
	
Error:
	return error;
}

/*- chr63200AAttrPowStatFall_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrPowStatFall_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"POW:STAT:FALL?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrPowStatFall_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrPowStatFall_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"POW:STAT:FALL %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrPowStatL1_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrPowStatL1_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"POW:STAT:L1?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrPowStatL1_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrPowStatL1_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"POW:STAT:L1 %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrPowStatL2_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrPowStatL2_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"POW:STAT:L2?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrPowStatL2_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrPowStatL2_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"POW:STAT:L2 %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrPowStatRise_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrPowStatRise_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"POW:STAT:RISE?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrPowStatRise_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrPowStatRise_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"POW:STAT:RISE %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrPowStatVrng_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrPowStatVrng_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE];
	IviRangeTablePtr	tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr( viQueryf (io,"POW:STAT:VRNG?\n","%s",szRes));
	Scan(szRes,"%s>%s[xt40]",szRes);  
	viCheckErr(Ivi_GetViInt32EntryFromString(szRes,tblPtr,value,VI_NULL,VI_NULL,VI_NULL,VI_NULL));

	Error:
	return error;
}

/*- chr63200AAttrPowStatVrng_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrPowStatVrng_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	szValue;
	IviRangeTablePtr    tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr(Ivi_GetViInt32EntryFromValue(value,tblPtr,VI_NULL,VI_NULL,VI_NULL,VI_NULL,&szValue,VI_NULL));
	viCheckErr( viPrintf (io, "POW:STAT:VRNG %s\n",szValue));
	
Error:
	return error;
}

/*- chr63200AAttrResDynFall_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrResDynFall_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"RES:DYN:FALL?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrResDynFall_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrResDynFall_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"RES:DYN:FALL %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrResDynIrng_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrResDynIrng_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE];
	IviRangeTablePtr	tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr( viQueryf (io,"RES:DYN:IRNG?\n","%s",szRes));
	Scan(szRes,"%s>%s[xt40]",szRes);  
	viCheckErr(Ivi_GetViInt32EntryFromString(szRes,tblPtr,value,VI_NULL,VI_NULL,VI_NULL,VI_NULL));

	Error:
	return error;
}

/*- chr63200AAttrResDynIrng_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrResDynIrng_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	szValue;
	IviRangeTablePtr    tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr(Ivi_GetViInt32EntryFromValue(value,tblPtr,VI_NULL,VI_NULL,VI_NULL,VI_NULL,&szValue,VI_NULL));
	viCheckErr( viPrintf (io, "RES:DYN:IRNG %s\n",szValue));
	
Error:
	return error;
}

/*- chr63200AAttrResDynL1_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrResDynL1_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"RES:DYN:L1?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrResDynL1_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrResDynL1_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"RES:DYN:L1 %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrResDynL2_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrResDynL2_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"RES:DYN:L2?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrResDynL2_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrResDynL2_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"RES:DYN:L2 %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrResDynRep_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrResDynRep_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"RES:DYN:REP?\n","%d",value));

	Error:
	return error;
}

/*- chr63200AAttrResDynRep_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrResDynRep_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"RES:DYN:REP %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrResDynRise_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrResDynRise_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"RES:DYN:RISE?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrResDynRise_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrResDynRise_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"RES:DYN:RISE %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrResDynT1_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrResDynT1_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"RES:DYN:T1?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrResDynT1_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrResDynT1_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"RES:DYN:T1 %fms\n",value));

	Error:
	return error;
}

/*- chr63200AAttrResDynT2_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrResDynT2_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"RES:DYN:T2?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrResDynT2_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrResDynT2_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"RES:DYN:T2 %fms\n",value));

	Error:
	return error;
}

/*- chr63200AAttrResStatFall_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrResStatFall_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"RES:STAT:FALL?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrResStatFall_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrResStatFall_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"RES:STAT:FALL %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrResStatIrng_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrResStatIrng_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE];
	IviRangeTablePtr	tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr( viQueryf (io,"RES:STAT:IRNG?\n","%s",szRes));
	Scan(szRes,"%s>%s[xt40]",szRes);  
	viCheckErr(Ivi_GetViInt32EntryFromString(szRes,tblPtr,value,VI_NULL,VI_NULL,VI_NULL,VI_NULL));

	Error:
	return error;
}

/*- chr63200AAttrResStatIrng_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrResStatIrng_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	szValue;
	IviRangeTablePtr    tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr(Ivi_GetViInt32EntryFromValue(value,tblPtr,VI_NULL,VI_NULL,VI_NULL,VI_NULL,&szValue,VI_NULL));
	viCheckErr( viPrintf (io, "RES:STAT:IRNG %s\n",szValue));
	
Error:
	return error;
}

/*- chr63200AAttrResStatL1_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrResStatL1_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"RES:STAT:L1?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrResStatL1_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrResStatL1_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"RES:STAT:L1 %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrResStatL2_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrResStatL2_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"RES:STAT:L2?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrResStatL2_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrResStatL2_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"RES:STAT:L2 %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrResStatRise_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrResStatRise_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"RES:STAT:RISE?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrResStatRise_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrResStatRise_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"RES:STAT:RISE %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrSpecCurrH_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrSpecCurrH_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"SPEC:CURR:H?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrSpecCurrH_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrSpecCurrH_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"SPEC:CURR:H %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrSpecCurrL_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrSpecCurrL_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"SPEC:CURR:L?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrSpecCurrL_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrSpecCurrL_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"SPEC:CURR:L %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrSpecCurrC_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrSpecCurrC_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"SPEC:CURR:C?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrSpecCurrC_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrSpecCurrC_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"SPEC:CURR:C %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrSpecPowC_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrSpecPowC_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"SPEC:POW:C?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrSpecPowC_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrSpecPowC_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"SPEC:POW:C %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrSpecPowH_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrSpecPowH_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"SPEC:POW:H?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrSpecPowH_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrSpecPowH_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"SPEC:POW:H %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrSpecPowL_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrSpecPowL_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"SPEC:POW:L?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrSpecPowL_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrSpecPowL_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"SPEC:POW:L %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrSpecTest_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrSpecTest_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE];
	IviRangeTablePtr	tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr( viQueryf (io,"SPEC:TEST?\n","%s",szRes));
	viCheckErr(Ivi_GetViInt32EntryFromString(szRes,tblPtr,value,VI_NULL,VI_NULL,VI_NULL,VI_NULL));

	Error:
	return error;
}

/*- chr63200AAttrSpecTest_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrSpecTest_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	szValue;
	IviRangeTablePtr    tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr(Ivi_GetViInt32EntryFromValue(value,tblPtr,VI_NULL,VI_NULL,VI_NULL,VI_NULL,&szValue,VI_NULL));
	viCheckErr( viPrintf (io, "SPEC:TEST %s\n",szValue));
	
Error:
	return error;
}

/*- chr63200AAttrSpecUnit_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrSpecUnit_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE];
	IviRangeTablePtr	tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr( viQueryf (io,"SPEC:UNIT?\n","%s",szRes));
	Scan(szRes,"%s>%s[xt40]",szRes);
	viCheckErr(Ivi_GetViInt32EntryFromString(szRes,tblPtr,value,VI_NULL,VI_NULL,VI_NULL,VI_NULL));

	Error:
	return error;
}

/*- chr63200AAttrSpecUnit_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrSpecUnit_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	szValue;
	IviRangeTablePtr    tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr(Ivi_GetViInt32EntryFromValue(value,tblPtr,VI_NULL,VI_NULL,VI_NULL,VI_NULL,&szValue,VI_NULL));
	viCheckErr( viPrintf (io, "SPEC:UNIT %s\n",szValue));
	
Error:
	return error;
}

/*- chr63200AAttrSpecVoltC_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrSpecVoltC_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"SPEC:VOLT:C?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrSpecVoltC_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrSpecVoltC_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"SPEC:VOLT:C %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrSpecVoltH_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrSpecVoltH_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"SPEC:VOLT:H?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrSpecVoltH_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrSpecVoltH_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"SPEC:VOLT:H %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrSpecVoltL_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrSpecVoltL_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"SPEC:VOLT:L?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrSpecVoltL_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrSpecVoltL_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"SPEC:VOLT:L %f\n",value));

	Error:
	return error;
}


/*- chr63200AAttrSyncRun_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrSyncRun_WriteCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	szValue;
	IviRangeTablePtr    tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr(Ivi_GetViInt32EntryFromValue(value,tblPtr,VI_NULL,VI_NULL,VI_NULL,VI_NULL,&szValue,VI_NULL));
	viCheckErr( viPrintf (io, "SYNC:RUN %s\n",szValue));
	
Error:
	return error;
}

/*- chr63200AAttrSyncType_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrSyncType_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	szValue;
	IviRangeTablePtr    tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr(Ivi_GetViInt32EntryFromValue(value,tblPtr,VI_NULL,VI_NULL,VI_NULL,VI_NULL,&szValue,VI_NULL));
	viCheckErr( viPrintf (io, "SYNC:TYPE %s\n",szValue));
	
Error:
	return error;
}

/*- chr63200AAttrVoltStatIlim_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrVoltStatIlim_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"VOLT:STAT:ILIM?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrVoltStatIlim_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrVoltStatIlim_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"VOLT:STAT:ILIM %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrVoltStatIrng_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrVoltStatIrng_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE];
	IviRangeTablePtr	tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr( viQueryf (io,"VOLT:STAT:IRNG?\n","%s",szRes));
	Scan(szRes,"%s>%s[xt40]",szRes);
	viCheckErr(Ivi_GetViInt32EntryFromString(szRes,tblPtr,value,VI_NULL,VI_NULL,VI_NULL,VI_NULL));

	Error:
	return error;
}

/*- chr63200AAttrVoltStatIrng_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrVoltStatIrng_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	szValue;
	IviRangeTablePtr    tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr(Ivi_GetViInt32EntryFromValue(value,tblPtr,VI_NULL,VI_NULL,VI_NULL,VI_NULL,&szValue,VI_NULL));
	viCheckErr( viPrintf (io, "VOLT:STAT:IRNG %s\n",szValue));
	
Error:
	return error;
}

/*- chr63200AAttrVoltStatL1_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrVoltStatL1_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"VOLT:STAT:L1?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrVoltStatL1_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrVoltStatL1_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"VOLT:STAT:L1 %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrVoltStatL2_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrVoltStatL2_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"VOLT:STAT:L2?\n","%f",value));

	Error:
	return error;
}

/*- chr63200AAttrVoltStatL2_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrVoltStatL2_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"VOLT:STAT:L2 %f\n",value));

	Error:
	return error;
}

/*- chr63200AAttrVoltStatRes_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrVoltStatRes_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE];
	IviRangeTablePtr	tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr( viQueryf (io,"VOLT:STAT:RES?\n","%s",szRes));
	Scan(szRes,"%s>%s[xt40]",szRes);
	viCheckErr(Ivi_GetViInt32EntryFromString(szRes,tblPtr,value,VI_NULL,VI_NULL,VI_NULL,VI_NULL));

	Error:
	return error;
}

/*- chr63200AAttrVoltStatRes_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrVoltStatRes_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	szValue;
	IviRangeTablePtr    tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr(Ivi_GetViInt32EntryFromValue(value,tblPtr,VI_NULL,VI_NULL,VI_NULL,VI_NULL,&szValue,VI_NULL));
	viCheckErr( viPrintf (io, "VOLT:STAT:RES %s\n",szValue));
	
Error:
	return error;
}

/*- chr63200AAttrAdvUserWavDataPoin_CheckCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvUserWavDataPoin_CheckCallback (ViSession vi,
                                                                        ViConstString channelName,
                                                                        ViAttr attributeId,
                                                                        ViConstString value)
{
	ViStatus	error = VI_SUCCESS;

	ViChar 		*uszWFD_CHK;
	ViChar 		szWFDTmp[32],szErrorMsg[128];
	ViInt32		iMode=-1;
	ViUInt16	uiWFDCount=0;
	ViReal64	fMin=0,fMax=0;
	
	uszWFD_CHK = calloc(1 + strlen(value),sizeof(ViByte));
	
	checkErr(chr63200A_GetAttributeViInt32 (vi, "", CHR63200A_ATTR_MODE,&iMode));
	
	if(iMode<CHR63200A_VAL_MOD_UDWL || iMode>CHR63200A_VAL_MOD_UDWH)
		checkErr(chr63200A_SetAttributeViInt32 (vi, "", CHR63200A_ATTR_MODE,CHR63200A_VAL_MOD_UDWL));

	
	memcpy(uszWFD_CHK,value,strlen(value));
	ViSession io = Ivi_IOSession(vi);
	
	viCheckErr( viQueryf (io,"CURR:STAT:L1? MAX\n","%Lf",&fMax));
	viCheckErr( viQueryf (io,"CURR:STAT:L1? MIN\n","%Lf",&fMin));
	
	
	while(strlen(uszWFD_CHK)>0)
	{
		uiWFDCount++;
		
		if  (strstr(uszWFD_CHK , ",") != VI_NULL)
			Scan(uszWFD_CHK,"%s>%s[xt44]%s",szWFDTmp,uszWFD_CHK);
		else	
		{
			Scan(uszWFD_CHK,"%s>%s",szWFDTmp);
			Fmt(uszWFD_CHK,"");
		}
		sprintf(szErrorMsg,"#%d : %s , Out of Range.",uiWFDCount,szWFDTmp);
		;
		viCheckErrElab(Ivi_CheckNumericRange (atof(szWFDTmp), fMin, fMax, CHR63200A_ERROR_UDW_POINT_OUTOFRANGE),szErrorMsg);
	}
	
	if(iMode<CHR63200A_VAL_MOD_UDWL || iMode>CHR63200A_VAL_MOD_UDWH)
		checkErr(chr63200A_SetAttributeViInt32 (vi, "", CHR63200A_ATTR_MODE,iMode));
																		
Error:
	free(uszWFD_CHK);
	return error;
}

/*- chr63200AAttrAdvUserWavData_CheckCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvUserWavData_CheckCallback (ViSession vi,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViConstString value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szUWD[5][BUFFER_SIZE]={{VI_NULL},{VI_NULL},{VI_NULL},{VI_NULL},{VI_NULL}};
	ViChar		szErrMsg[128];
	ViReal64	fMin,fMax;
	
	Scan(value,"%s>%s[xt44]%s[xt44]%s[xt44]%s[xt44]%s[xt44]",szUWD[0],szUWD[1],szUWD[2],szUWD[3],szUWD[4]);

	for (int i=0;i<5;i++)
	{
		checkErr(Ivi_GetViReal64EntryFromCoercedVal(i,&UDWDRangeTable,&fMin,&fMax,VI_NULL,VI_NULL,VI_NULL));
		sprintf(szErrMsg,"Value out of Range , Valid Range : %f to %f. Current Setting Parameter <Arg%d> , %s",fMin,fMax,i+1 ,szUWD[i]);
		viCheckErrElab(Ivi_CheckNumericRange (atof(szUWD[i]), fMin, fMax, IVI_ERROR_INVALID_VALUE),szErrMsg);
	}
Error:
	return error;
}

/*- chr63200AAttrLoadProtCle_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrLoadProtCle_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViBoolean value)
{
	ViStatus	error = VI_SUCCESS;
	if(value)
	{
		viCheckErr( viPrintf (io, "LOAD:PROT:CLE\n"));
	}
	
Error:
	return error;
}
/*- chr63200AAttrProgNum_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrProgNum_ReadCallback (ViSession vi,
                                                            ViSession io,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viQueryf (io,"PROG:NSEL?\n","%d",value));

	Error:
	return error;
}

/*- chr63200AAttrProgNum_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrProgNum_WriteCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;

	viCheckErr( viPrintf (io,"PROG:NSEL %d\n",value));

	Error:
	return error;
}

/*- chr63200AAttrProgSequenceNum_CheckCallback -*/

static ViStatus _VI_FUNC chr63200AAttrProgSequenceNum_CheckCallback (ViSession vi,
                                                                     ViConstString channelName,
                                                                     ViAttr attributeId,
                                                                     ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;

	ViSession io = Ivi_IOSession(vi);
	
	ViInt32		iMax = -1;

	viCheckErr( viQueryf (io,"PROG:SEQ:REM?\n","%d",&iMax));
	
	if (iMax >100)
		iMax = 100;
	
	checkErr(Ivi_CheckNumericRange (value, 1, iMax, IVI_ERROR_INVALID_VALUE));

	
Error:
	return error;
}

/*- chr63200AAttrProgNum_CheckCallback -*/

static ViStatus _VI_FUNC chr63200AAttrProgNum_CheckCallback (ViSession vi,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;

	ViSession io = Ivi_IOSession(vi);
	ViInt32	iMax = -1, iMin = -1;
	viCheckErr( viQueryf (io,"PROG:NSEL? MAX\n","%d",&iMax));
	viCheckErr( viQueryf (io,"PROG:NSEL? MIN\n","%d",&iMin));
	checkErr(Ivi_CheckNumericRange (value, iMin, iMax, IVI_ERROR_INVALID_VALUE));

	
Error:
	return error;
}

/*- chr63200AAttrProgDataValue_CheckCallback -*/

static ViStatus _VI_FUNC chr63200AAttrProgDataValue_CheckCallback (ViSession vi,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;

	ViSession io = Ivi_IOSession(vi);
	ViReal64	fMax = -1, fMin = -1;
	ViReal64	fLoadValMax = -1 ,fStartMax = -1 ,fEndMax = -1 ,fRiseMax = -1 ,fFallMax = -1 ,fDwellMax = -1; 
	ViReal64	fVHMax = -1 ,fVLMax = -1 ,fCHMax = -1 ,fCLMax = -1 ,fPHMax = -1 ,fPLMax = -1 ,fDelayMax = -1;
	ViReal64	fLoadValMin = -1 ,fStartMin = -1 ,fEndMin = -1 ,fRiseMin = -1 ,fFallMin = -1 ,fDwellMin = -1;
	ViReal64	fVHMin = -1 ,fVLMin = -1 ,fCHMin = -1 ,fCLMin = -1 ,fPHMin = -1 ,fPLMin = -1 ,fDelayMin = -1;
	
	ViChar		szReadBuffer[BUFFER_SIZE] ="";
	ViInt32		iProgramNumber	 	= -1;
	ViInt32		iProgramType 		= -1;	
	ViInt32		iProgramMode		= -1;
	ViInt32		iProgramRange		= -1;
	ViInt32		iSequence			= -1;
	
	checkErr(Ivi_GetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_PROG_NUM,			0,&iProgramNumber	));
	checkErr(Ivi_GetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_PROG_TYPE,			0,&iProgramType		));	
	checkErr(Ivi_GetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_PROG_RUN_MODE,		0,&iProgramMode		));
	checkErr(Ivi_GetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_PROG_MODE_RANGE,	0,&iProgramRange	));		
	checkErr(Ivi_GetAttributeViInt32 	(vi,VI_NULL, CHR63200A_ATTR_PROG_SEQUENCE_INDEX,0,&iSequence 		));
	
	


	if(!iProgramType)
	{
		viCheckErr( viQueryf (io,"PROG:DATA:LIST? %d,%d,%d,%d MAX\n","%s",iProgramNumber,iSequence,iProgramMode,iProgramRange,szReadBuffer));
		Scan(szReadBuffer,"%s>%s[dxt44]%s[dxt44]%s[dxt44]%s[dxt44]%s[dxt44]%f[x]%f[x]%f[x]%f[x]%f[x]%f[x]%f[x]%f[x]%f[x]%f[x]%f",
				&fLoadValMax,&fFallMax,&fRiseMax,&fDwellMax,&fVHMax,&fVLMax,&fCHMax,&fCLMax,&fPHMax,&fPLMax,&fDelayMax);

		viCheckErr( viQueryf (io,"PROG:DATA:LIST? %d,%d,%d,%d MIN\n","%s",iProgramNumber,iSequence,iProgramMode,iProgramRange,szReadBuffer));
		Scan(szReadBuffer,"%s>%s[dxt44]%s[dxt44]%s[dxt44]%s[dxt44]%s[dxt44]%f[x]%f[x]%f[x]%f[x]%f[x]%f[x]%f[x]%f[x]%f[x]%f[x]%f",
				&fLoadValMin,&fFallMin,&fRiseMin,&fDwellMin,&fVHMin,&fVLMin,&fCHMin,&fCLMin,&fPHMin,&fPLMin,&fDelayMin);		
  		
	}
	else
	{
		viCheckErr( viQueryf (io,"PROG:DATA:STEP? %d,%d,%d MAX\n","%s",iProgramNumber,iProgramMode,iProgramRange,szReadBuffer));
		Scan(szReadBuffer,"%s>%s[dxt44]%s[dxt44]%s[dxt44]%s[dxt44]%f[x]%f[x]%f[x]%f[x]%f[x]%f[x]%f[x]%f[x]%f[x]%f[x]%f[x]%f",
				&fStartMax,&fEndMax,&fFallMax,&fRiseMax,&fDwellMax,&fVHMax,&fVLMax,&fCHMax,&fCLMax,&fPHMax,&fPLMax,&fDelayMax);

		viCheckErr( viQueryf (io,"PROG:DATA:STEP? %d,%d,%d MIN\n","%s",iProgramNumber,iProgramMode,iProgramRange,szReadBuffer));
		Scan(szReadBuffer,"%s>%s[dxt44]%s[dxt44]%s[dxt44]%s[dxt44]%f[x]%f[x]%f[x]%f[x]%f[x]%f[x]%f[x]%f[x]%f[x]%f[x]%f[x]%f",
				&fStartMin,&fEndMin,&fFallMin,&fRiseMin,&fDwellMin,&fVHMin,&fVLMin,&fCHMin,&fCLMin,&fPHMin,&fPLMin,&fDelayMin);		
  		
	}	
	
	
		switch(attributeId)
		{
			case CHR63200A_ATTR_PROG_LIST_VALUE:
				fMax=fLoadValMax;
				fMin=fLoadValMin;	
			break;
			case CHR63200A_ATTR_PROG_STEP_START:
				fMax=fStartMax;
				fMin=fStartMin;
			break;
			case CHR63200A_ATTR_PROG_STEP_END:
				fMax=fEndMax;
				fMin=fEndMin;
			break;
			case CHR63200A_ATTR_PROG_RISE:
				fMax=fRiseMax;
				fMin=fRiseMin;
			break;
			case CHR63200A_ATTR_PROG_FALL:
				fMax=fFallMax;
				fMin=fFallMin;
			break;
			case CHR63200A_ATTR_PROG_DWELL:
				fMax=fDwellMax;
				fMin=fDwellMin;
			break;
			case CHR63200A_ATTR_PROG_VH:
				fMax=fVHMax;
				fMin=-1;
			break;
			case CHR63200A_ATTR_PROG_VL:
				fMax=fVLMax;
				fMin=-1;
			break;
			case CHR63200A_ATTR_PROG_IH:
				fMax=fCHMax;
				fMin=-1;
			break;
			case CHR63200A_ATTR_PROG_IL:
				fMax=fCLMax;
				fMin=-1;
			break;
			case CHR63200A_ATTR_PROG_PH:
				fMax=fPHMax;
				fMin=-1;
			break;
			case CHR63200A_ATTR_PROG_PL:
				fMax=fPLMax;
				fMin=-1;
			break;
			case CHR63200A_ATTR_PROG_DELAY:
				fMax=fDelayMax;
				fMin=fDelayMin;
			break;
			default:
				fMax=-1;
				fMin=-1;
			break;
		}
	
	
	checkErr(Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));


	
Error:
	return error;
}

/*- chr63200AAttrProgSequenceIndex_CheckCallback -*/

static ViStatus _VI_FUNC chr63200AAttrProgSequenceIndex_CheckCallback (ViSession vi,
                                                                       ViConstString channelName,
                                                                       ViAttr attributeId,
                                                                       ViInt32 value){
	ViStatus	error = VI_SUCCESS;
	ViChar		szReadBuffer[BUFFER_SIZE]="";
	ViInt32		iProgramNumber = -1;
	ViInt32		iMax = -1;
	
	ViSession io = Ivi_IOSession(vi);

	checkErr(Ivi_GetAttributeViInt32	(vi,VI_NULL, CHR63200A_ATTR_PROG_NUM,			0,&iProgramNumber	));
	viCheckErr( viQueryf (io,"PROG:DATA? %d\n","%s",iProgramNumber,szReadBuffer));
	
	Scan(szReadBuffer,"%s>%s[dxt44]%s[dxt44]%s[dxt44]%s[dxt44]%d",&iMax);

	checkErr(Ivi_CheckNumericRange (value, 1, iMax, IVI_ERROR_INVALID_VALUE));

	
Error:
	return error;
}

/*- chr63200AAttrAdvBattRng_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvBattRng_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE]="";
	ViInt32		iMode=-1;
	IviRangeTablePtr	tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	
	chr63200A_GetAttributeViInt32 (vi, "", CHR63200A_ATTR_ADV_BATT_MODE,&iMode );
	switch(iMode)
	{
		case CHR63200A_VAL_BATT_CC:
			viCheckErr( viQueryf (io,"BATT:VRNG?\n","%s",szRes));
			break;
		case CHR63200A_VAL_BATT_CR:
			viCheckErr( viQueryf (io,"BATT:IRNG?\n","%s",szRes));
			break;
		case CHR63200A_VAL_BATT_CP:
			viCheckErr( viQueryf (io,"BATT:VRNG?\n","%s",szRes));
			break;
		default:
			break;
	}
	
	Scan(szRes,"%s>%s[xt40]",szRes);
	viCheckErr(Ivi_GetViInt32EntryFromString(szRes,tblPtr,value,VI_NULL,VI_NULL,VI_NULL,VI_NULL));

Error:
	return error;
}

/*- chr63200AAttrAdvBattRng_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvBattRng_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViInt32		iMode=-1;
	IviRangeTablePtr	tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	
	chr63200A_GetAttributeViInt32 (vi, "", CHR63200A_ATTR_ADV_BATT_MODE,&iMode );
	switch(iMode)
	{
		case CHR63200A_VAL_BATT_CC:
			viCheckErr( viPrintf (io,"BATT:VRNG %d\n",value));
			break;
		case CHR63200A_VAL_BATT_CR:
			viCheckErr( viPrintf (io,"BATT:IRNG %d\n",value));
			break;
		case CHR63200A_VAL_BATT_CP:
			viCheckErr( viPrintf (io,"BATT:VRNG %d\n",value));
			break;
		default:
			break;
	}
	
Error:
	return error;
}

/*- chr63200AAttrAdvSwdRng_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvSwdRng_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE];
	IviRangeTablePtr	tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr( viQueryf (io,"SINE:VRNG?\n","%s",szRes));
	Scan(szRes,"%s>%s[xt40]",szRes);
	viCheckErr(Ivi_GetViInt32EntryFromString(szRes,tblPtr,value,VI_NULL,VI_NULL,VI_NULL,VI_NULL));

	Error:
	return error;
}

/*- chr63200AAttrAdvSwdRng_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrAdvSwdRng_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	szValue;
	IviRangeTablePtr    tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr(Ivi_GetViInt32EntryFromValue(value,tblPtr,VI_NULL,VI_NULL,VI_NULL,VI_NULL,&szValue,VI_NULL));
	viCheckErr( viPrintf (io, "SINE:VRNG %s\n",szValue));
	
Error:
	return error;
}

/*- chr63200AAttrCurrSweRng_ReadCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrSweRng_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 *value)
{
	ViStatus	error = VI_SUCCESS;
	ViChar		szRes[BUFFER_SIZE];
	IviRangeTablePtr	tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr( viQueryf (io,"CURR:SWE:VRNG?\n","%s",szRes));
	Scan(szRes,"%s>%s[xt40]",szRes);
	viCheckErr(Ivi_GetViInt32EntryFromString(szRes,tblPtr,value,VI_NULL,VI_NULL,VI_NULL,VI_NULL));

	Error:
	return error;
}

/*- chr63200AAttrCurrSweRng_WriteCallback -*/

static ViStatus _VI_FUNC chr63200AAttrCurrSweRng_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViString 	szValue;
	IviRangeTablePtr    tblPtr;
	Ivi_GetAttrRangeTable(vi,VI_NULL,attributeId,&tblPtr);
	viCheckErr(Ivi_GetViInt32EntryFromValue(value,tblPtr,VI_NULL,VI_NULL,VI_NULL,VI_NULL,&szValue,VI_NULL));
	viCheckErr( viPrintf (io, "CURR:SWE:VRNG %s\n",szValue));
	
Error:
	return error;
}





/*------------------------- RangeCheckCallback -------------------------*/

static ViStatus _VI_FUNC chr63200AAttrAdvAutoIset_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"AUTO:ISET? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"AUTO:ISET? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrAdvAutoPset_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"AUTO:PSET? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"AUTO:PSET? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrAdvAutoRset_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"AUTO:RSET? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"AUTO:RSET? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrAdvAutoVset_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"AUTO:VSET? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"AUTO:VSET? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrAdvBattEndv_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"BATT:ENDV? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"BATT:ENDV? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrAdvBattFall_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"BATT:FALL? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"BATT:FALL? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrAdvBattRise_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"BATT:RISE? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"BATT:RISE? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrAdvBattTout_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"BATT:TOUT? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"BATT:TOUT? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrAdvBattVal_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"BATT:VAL? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"BATT:VAL? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrAdvCrccIset_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"CRCC:ISET? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"CRCC:ISET? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrAdvCrccRset_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"CRCC:RSET? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"CRCC:RSET? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrAdvCvccIset_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"CVCC:ISET? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"CVCC:ISET? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrAdvCvccVset_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"CVCC:VSET? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"CVCC:VSET? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrAdvCvcrRset_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"CVCR:RSET? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"CVCR:RSET? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrAdvCvcrVset_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"CVCR:VSET? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"CVCR:VSET? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrAdvOcpDwel_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"OCP:DWEL? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"OCP:DWEL? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrAdvOcpEnd_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"OCP:END? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"OCP:END? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrAdvOcpSpecH_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"OCP:SPEC:H? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"OCP:SPEC:H? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrAdvOcpSpecL_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"OCP:SPEC:L? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"OCP:SPEC:L? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrAdvOcpStar_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"OCP:STAR? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"OCP:STAR? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrAdvOcpStep_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"OCP:STEP? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"OCP:STEP? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrAdvOcpTrigVolt_CheckCallback (ViSession vi,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"OCP:TRIG:VOLT? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"OCP:TRIG:VOLT? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrAdvOppDwel_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"OPP:DWEL? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"OPP:DWEL? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrAdvOppEnd_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"OPP:END? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"OPP:END? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrAdvOppSpecH_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"OPP:SPEC:H? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"OPP:SPEC:H? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrAdvOppSpecL_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"OPP:SPEC:L? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"OPP:SPEC:L? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrAdvOppStar_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"OPP:STAR? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"OPP:STAR? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrAdvOppStep_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"OPP:STEP? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"OPP:STEP? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrAdvOppTrigVolt_CheckCallback (ViSession vi,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"OPP:TRIG:VOLT? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"OPP:TRIG:VOLT? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrAdvSineFreq_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"SINE:FREQ? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"SINE:FREQ? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrAdvSineIac_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"SINE:IAC? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"SINE:IAC? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrAdvSineIdc_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"SINE:IDC? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"SINE:IDC? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrAdvUserWavNsel_CheckCallback (ViSession vi,
                                                                    ViConstString channelName,
                                                                    ViAttr attributeId,
                                                                    ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"ADV:USER:WAV:NSEL? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"ADV:USER:WAV:NSEL? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrConfVoltOff_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"CONF:VOLT:OFF? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"CONF:VOLT:OFF? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrConfVoltOn_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"CONF:VOLT:ON? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"CONF:VOLT:ON? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrConfWind_CheckCallback (ViSession vi,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"CONF:WIND? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"CONF:WIND? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrCurrDynFall_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"CURR:DYN:FALL? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"CURR:DYN:FALL? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrCurrDynL1_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"CURR:DYN:L1? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"CURR:DYN:L1? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrCurrDynL2_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"CURR:DYN:L2? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"CURR:DYN:L2? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrCurrDynRep_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"CURR:DYN:REP? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"CURR:DYN:REP? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrCurrDynRise_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"CURR:DYN:RISE? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"CURR:DYN:RISE? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrCurrDynT1_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"CURR:DYN:T1? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"CURR:DYN:T1? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrCurrDynT2_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"CURR:DYN:T2? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"CURR:DYN:T2? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrCurrStatFall_CheckCallback (ViSession vi,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"CURR:STAT:FALL? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"CURR:STAT:FALL? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrCurrStatL1_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"CURR:STAT:L1? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"CURR:STAT:L1? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrCurrStatL2_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"CURR:STAT:L2? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"CURR:STAT:L2? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrCurrStatRise_CheckCallback (ViSession vi,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"CURR:STAT:RISE? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"CURR:STAT:RISE? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrCurrSweDuty_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"CURR:SWE:DUTY? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"CURR:SWE:DUTY? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrCurrSweDwel_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"CURR:SWE:DWEL? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"CURR:SWE:DWEL? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrCurrSweFall_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"CURR:SWE:FALL? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"CURR:SWE:FALL? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrCurrSweFend_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"CURR:SWE:FEND? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"CURR:SWE:FEND? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrCurrSweFsta_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"CURR:SWE:FSTA? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"CURR:SWE:FSTA? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrCurrSweFstep_CheckCallback (ViSession vi,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"CURR:SWE:FSTE? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"CURR:SWE:FSTE? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrCurrSweImax_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"CURR:SWE:IMAX? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"CURR:SWE:IMAX? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrCurrSweImin_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"CURR:SWE:IMIN? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"CURR:SWE:IMIN? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrCurrSweRise_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"CURR:SWE:RISE? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"CURR:SWE:RISE? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrDigSampPoin_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"DIG:SAMP:POIN? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"DIG:SAMP:POIN? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrDigSampTime_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"DIG:SAMP:TIME? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"DIG:SAMP:TIME? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrDigTrigPoin_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"DIG:TRIG:POIN? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"DIG:TRIG:POIN? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrImpStatCl_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"IMP:STAT:CL? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"IMP:STAT:CL? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrImpStatLs_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"IMP:STAT:LS? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"IMP:STAT:LS? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrImpStatRl_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"IMP:STAT:RL? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"IMP:STAT:RL? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrImpStatRs_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"IMP:STAT:RS? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"IMP:STAT:RS? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrPowStatFall_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"POW:STAT:FALL? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"POW:STAT:FALL? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrPowStatL1_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"POW:STAT:L1? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"POW:STAT:L1? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrPowStatL2_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"POW:STAT:L2? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"POW:STAT:L2? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrPowStatRise_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"POW:STAT:RISE? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"POW:STAT:RISE? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrResDynFall_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"RES:DYN:FALL? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"RES:DYN:FALL? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrResDynL1_CheckCallback (ViSession vi,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"RES:DYN:L1? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"RES:DYN:L1? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrResDynL2_CheckCallback (ViSession vi,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"RES:DYN:L2? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"RES:DYN:L2? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrResDynRep_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"RES:DYN:REP? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"RES:DYN:REP? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrResDynRise_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"RES:DYN:RISE? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"RES:DYN:RISE? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrResDynT1_CheckCallback (ViSession vi,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"RES:DYN:T1? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"RES:DYN:T1? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrResDynT2_CheckCallback (ViSession vi,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"RES:DYN:T2? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"RES:DYN:T2? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrResStatFall_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"RES:STAT:FALL? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"RES:STAT:FALL? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrResStatL1_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"RES:STAT:L1? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"RES:STAT:L1? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrResStatL2_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"RES:STAT:L2? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"RES:STAT:L2? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrResStatRise_CheckCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"RES:STAT:RISE? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"RES:STAT:RISE? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrSpecCurrC_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"SPEC:CURR:C? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"SPEC:CURR:C? MIN\n","%Lf",&fMin));
	if(Ivi_CheckNumericRange (value, -1, -1, IVI_ERROR_INVALID_VALUE)!=0)
		viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrSpecCurrH_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"SPEC:CURR:H? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"SPEC:CURR:H? MIN\n","%Lf",&fMin));
	if(Ivi_CheckNumericRange (value, -1, -1, IVI_ERROR_INVALID_VALUE)!=0)
		viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrSpecCurrL_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"SPEC:CURR:L? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"SPEC:CURR:L? MIN\n","%Lf",&fMin));
	if(Ivi_CheckNumericRange (value, -1, -1, IVI_ERROR_INVALID_VALUE)!=0)
		viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrSpecPowC_CheckCallback (ViSession vi,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"SPEC:POW:C? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"SPEC:POW:C? MIN\n","%Lf",&fMin));
	if(Ivi_CheckNumericRange (value, -1, -1, IVI_ERROR_INVALID_VALUE)!=0)
		viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrSpecPowH_CheckCallback (ViSession vi,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"SPEC:POW:H? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"SPEC:POW:H? MIN\n","%Lf",&fMin));
	if(Ivi_CheckNumericRange (value, -1, -1, IVI_ERROR_INVALID_VALUE)!=0)
		viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrSpecPowL_CheckCallback (ViSession vi,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"SPEC:POW:L? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"SPEC:POW:L? MIN\n","%Lf",&fMin));
	if(Ivi_CheckNumericRange (value, -1, -1, IVI_ERROR_INVALID_VALUE)!=0)
		viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrSpecVoltC_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"SPEC:VOLT:C? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"SPEC:VOLT:C? MIN\n","%Lf",&fMin));
	if(Ivi_CheckNumericRange (value, -1, -1, IVI_ERROR_INVALID_VALUE)!=0)
		viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrSpecVoltH_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"SPEC:VOLT:H? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"SPEC:VOLT:H? MIN\n","%Lf",&fMin));
	if(Ivi_CheckNumericRange (value, -1, -1, IVI_ERROR_INVALID_VALUE)!=0)
		viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrSpecVoltL_CheckCallback (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"SPEC:VOLT:L? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"SPEC:VOLT:L? MIN\n","%Lf",&fMin));
	if(Ivi_CheckNumericRange (value, -1, -1, IVI_ERROR_INVALID_VALUE)!=0)
		viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrVoltStatIlim_CheckCallback (ViSession vi,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"VOLT:STAT:ILIM? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"VOLT:STAT:ILIM? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrVoltStatL1_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"VOLT:STAT:L1? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"VOLT:STAT:L1? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

static ViStatus _VI_FUNC chr63200AAttrVoltStatL2_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
	ViStatus	error = VI_SUCCESS;
	ViReal64	fMin=0,fMax=0;
	ViSession	io = Ivi_IOSession(vi);
	
	viCheckErr(viQueryf(io,"VOLT:STAT:L2? MAX\n","%Lf",&fMax));
	viCheckErr(viQueryf(io,"VOLT:STAT:L2? MIN\n","%Lf",&fMin));
	viCheckErr( Ivi_CheckNumericRange (value, fMin, fMax, IVI_ERROR_INVALID_VALUE));
	
Error:
	return error;
}

/*****************************************************************************
 * Function: chr63200A_InitAttributes                                                  
 * Purpose:  This function adds attributes to the IVI session, initializes   
 *           instrument attributes, and sets attribute invalidation          
 *           dependencies.                                                   
 *****************************************************************************/
static ViStatus chr63200A_InitAttributes (ViSession vi)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     flags = 0;
    
        /*- Initialize instrument attributes --------------------------------*/            

    checkErr( Ivi_SetAttributeViInt32 (vi, "", CHR63200A_ATTR_SPECIFIC_DRIVER_MAJOR_VERSION,
                                       0, CHR63200A_MAJOR_VERSION));
    checkErr( Ivi_SetAttributeViInt32 (vi, "", CHR63200A_ATTR_SPECIFIC_DRIVER_MINOR_VERSION,
                                       0, CHR63200A_MINOR_VERSION));

    checkErr( Ivi_SetAttrReadCallbackViString (vi, CHR63200A_ATTR_SPECIFIC_DRIVER_REVISION,
                                               chr63200AAttrDriverRevision_ReadCallback));
    checkErr( Ivi_SetAttributeViInt32 (vi, "", CHR63200A_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION,
                                       0, CHR63200A_CLASS_SPEC_MAJOR_VERSION));
    checkErr( Ivi_SetAttributeViInt32 (vi, "", CHR63200A_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION,
                                       0, CHR63200A_CLASS_SPEC_MINOR_VERSION));
    checkErr( Ivi_SetAttributeViString (vi, "", CHR63200A_ATTR_SUPPORTED_INSTRUMENT_MODELS,
                                        0, CHR63200A_SUPPORTED_INSTRUMENT_MODELS));


    checkErr( Ivi_GetAttributeFlags (vi, CHR63200A_ATTR_INSTRUMENT_FIRMWARE_REVISION, &flags));
    checkErr( Ivi_SetAttributeFlags (vi, CHR63200A_ATTR_INSTRUMENT_FIRMWARE_REVISION, 
                                     flags | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION));
    checkErr( Ivi_SetAttrReadCallbackViString (vi, CHR63200A_ATTR_INSTRUMENT_FIRMWARE_REVISION,
                                               chr63200AAttrFirmwareRevision_ReadCallback));
	checkErr( Ivi_GetAttributeFlags (vi, CHR63200A_ATTR_INSTRUMENT_MANUFACTURER, &flags));
	checkErr( Ivi_SetAttributeFlags (vi, CHR63200A_ATTR_INSTRUMENT_MANUFACTURER, 
	                                 flags | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION));
	checkErr( Ivi_SetAttrReadCallbackViString (vi, CHR63200A_ATTR_INSTRUMENT_MANUFACTURER,
	                                           chr63200AAttrInstrumentManufacturer_ReadCallback));

	checkErr( Ivi_GetAttributeFlags (vi, CHR63200A_ATTR_INSTRUMENT_MODEL, &flags));
	checkErr( Ivi_SetAttributeFlags (vi, CHR63200A_ATTR_INSTRUMENT_MODEL, 
	                                 flags | IVI_VAL_USE_CALLBACKS_FOR_SIMULATION));
	checkErr( Ivi_SetAttrReadCallbackViString (vi, CHR63200A_ATTR_INSTRUMENT_MODEL,
	                                           chr63200AAttrInstrumentModel_ReadCallback));

    checkErr( Ivi_SetAttributeViString (vi, "", CHR63200A_ATTR_SPECIFIC_DRIVER_VENDOR,
                                        0, CHR63200A_DRIVER_VENDOR));
    checkErr( Ivi_SetAttributeViString (vi, "", CHR63200A_ATTR_SPECIFIC_DRIVER_DESCRIPTION,
                                        0, CHR63200A_DRIVER_DESCRIPTION));
    checkErr( Ivi_SetAttributeViAddr (vi, VI_NULL,IVI_ATTR_OPC_CALLBACK, 0,
                                      (ViAddr)chr63200A_WaitForOPCCallback));
    checkErr( Ivi_SetAttributeViAddr (vi, VI_NULL, IVI_ATTR_CHECK_STATUS_CALLBACK, 0,
                                      (ViAddr)chr63200A_CheckStatusCallback));

        
	checkErr( Ivi_SetAttributeViString (vi, "", CHR63200A_ATTR_GROUP_CAPABILITIES, 0,
	                                    "None"));
    
        /*- Add instrument-specific attributes ------------------------------*/            
	
	checkErr (Ivi_AddAttributeViString (vi, CHR63200A_ATTR_ID_QUERY_RESPONSE,
	                                    "CHR63200A_ATTR_ID_QUERY_RESPONSE",
	                                    "Chroma 632", IVI_VAL_NOT_USER_WRITABLE,
	                                    chr63200AAttrIdQueryResponse_ReadCallback,
	                                    VI_NULL));
	                                           
    checkErr( Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_OPC_TIMEOUT,
                                       "CHR63200A_ATTR_OPC_TIMEOUT",
                                       OPC_TIME, IVI_VAL_HIDDEN | IVI_VAL_DONT_CHECK_STATUS,
                                       VI_NULL, VI_NULL, VI_NULL)); 
    checkErr( Ivi_AddAttributeViAddr (vi, CHR63200A_ATTR_OPC_CALLBACK,
                                       "CHR63200A_ATTR_OPC_CALLBACK",
                                       VI_NULL,
                                       IVI_VAL_HIDDEN | IVI_VAL_DONT_CHECK_STATUS,
                                       VI_NULL, VI_NULL));
    checkErr( Ivi_AddAttributeViAddr (vi,
                                       CHR63200A_ATTR_CHECK_STATUS_CALLBACK,
                                       "CHR63200A_ATTR_CHECK_STATUS_CALLBACK",
                                       VI_NULL,
                                       IVI_VAL_HIDDEN | IVI_VAL_DONT_CHECK_STATUS,
                                       VI_NULL, VI_NULL));
    checkErr( Ivi_AddAttributeViAddr (vi,
                                       CHR63200A_ATTR_USER_INTERCHANGE_CHECK_CALLBACK,
                                       "CHR63200A_ATTR_USER_INTERCHANGE_CHECK_CALLBACK",
                                       VI_NULL,
                                       IVI_VAL_HIDDEN | IVI_VAL_DONT_CHECK_STATUS,
                                       VI_NULL, VI_NULL));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_ADV_AUTO_ISET,
	                                    "CHR63200A_ATTR_ADV_AUTO_ISET", 20, 0,
	                                    chr63200AAttrAdvAutoIset_ReadCallback,
	                                    chr63200AAttrAdvAutoIset_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_ADV_AUTO_ISET,
	                                            chr63200AAttrAdvAutoIset_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_ADV_AUTO_PSET,
	                                    "CHR63200A_ATTR_ADV_AUTO_PSET", 20, 0,
	                                    chr63200AAttrAdvAutoPset_ReadCallback,
	                                    chr63200AAttrAdvAutoPset_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_ADV_AUTO_PSET,
	                                            chr63200AAttrAdvAutoPset_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_ADV_AUTO_RSET,
	                                    "CHR63200A_ATTR_ADV_AUTO_RSET", 20, 0,
	                                    chr63200AAttrAdvAutoRset_ReadCallback,
	                                    chr63200AAttrAdvAutoRset_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_ADV_AUTO_RSET,
	                                            chr63200AAttrAdvAutoRset_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_ADV_AUTO_VSET,
	                                    "CHR63200A_ATTR_ADV_AUTO_VSET", 20, 0,
	                                    chr63200AAttrAdvAutoVset_ReadCallback,
	                                    chr63200AAttrAdvAutoVset_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_ADV_AUTO_VSET,
	                                            chr63200AAttrAdvAutoVset_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_ADV_BATT_ENDV,
	                                    "CHR63200A_ATTR_ADV_BATT_ENDV", 0.5, 0,
	                                    chr63200AAttrAdvBattEndv_ReadCallback,
	                                    chr63200AAttrAdvBattEndv_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_ADV_BATT_ENDV,
	                                            chr63200AAttrAdvBattEndv_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_ADV_BATT_FALL,
	                                    "CHR63200A_ATTR_ADV_BATT_FALL", 1.0, 0,
	                                    chr63200AAttrAdvBattFall_ReadCallback,
	                                    chr63200AAttrAdvBattFall_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_ADV_BATT_FALL,
	                                            chr63200AAttrAdvBattFall_CheckCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_ADV_BATT_MODE,
	                                   "CHR63200A_ATTR_ADV_BATT_MODE", 0, 0,
	                                   chr63200AAttrAdvBattMode_ReadCallback,
	                                   chr63200AAttrAdvBattMode_WriteCallback,
	                                   &attrAdvBattModeRangeTable));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_ADV_BATT_RISE,
	                                    "CHR63200A_ATTR_ADV_BATT_RISE", 1.0, 0,
	                                    chr63200AAttrAdvBattRise_ReadCallback,
	                                    chr63200AAttrAdvBattRise_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_ADV_BATT_RISE,
	                                            chr63200AAttrAdvBattRise_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_ADV_BATT_TOUT,
	                                    "CHR63200A_ATTR_ADV_BATT_TOUT", 0, 0,
	                                    chr63200AAttrAdvBattTout_ReadCallback,
	                                    chr63200AAttrAdvBattTout_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_ADV_BATT_TOUT,
	                                            chr63200AAttrAdvBattTout_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_ADV_BATT_VAL,
	                                    "CHR63200A_ATTR_ADV_BATT_VAL", 0.5, 0,
	                                    chr63200AAttrAdvBattVal_ReadCallback,
	                                    chr63200AAttrAdvBattVal_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_ADV_BATT_VAL,
	                                            chr63200AAttrAdvBattVal_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_ADV_CRCC_ISET,
	                                    "CHR63200A_ATTR_ADV_CRCC_ISET", 10, 0,
	                                    chr63200AAttrAdvCrccIset_ReadCallback,
	                                    chr63200AAttrAdvCrccIset_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_ADV_CRCC_ISET,
	                                            chr63200AAttrAdvCrccIset_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_ADV_CRCC_RSET,
	                                    "CHR63200A_ATTR_ADV_CRCC_RSET", 20, 0,
	                                    chr63200AAttrAdvCrccRset_ReadCallback,
	                                    chr63200AAttrAdvCrccRset_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_ADV_CRCC_RSET,
	                                            chr63200AAttrAdvCrccRset_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_ADV_CVCC_ISET,
	                                    "CHR63200A_ATTR_ADV_CVCC_ISET", 20, 0,
	                                    chr63200AAttrAdvCvccIset_ReadCallback,
	                                    chr63200AAttrAdvCvccIset_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_ADV_CVCC_ISET,
	                                            chr63200AAttrAdvCvccIset_CheckCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_ADV_CVCC_RES,
	                                   "CHR63200A_ATTR_ADV_CVCC_RES", 0, 0,
	                                   chr63200AAttrAdvCvccRes_ReadCallback,
	                                   chr63200AAttrAdvCvccRes_WriteCallback,
	                                   &attrVoltStatResRangeTable));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_ADV_CVCC_VSET,
	                                    "CHR63200A_ATTR_ADV_CVCC_VSET", 8, 0,
	                                    chr63200AAttrAdvCvccVset_ReadCallback,
	                                    chr63200AAttrAdvCvccVset_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_ADV_CVCC_VSET,
	                                            chr63200AAttrAdvCvccVset_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_ADV_CVCR_RSET,
	                                    "CHR63200A_ATTR_ADV_CVCR_RSET", 20, 0,
	                                    chr63200AAttrAdvCvcrRset_ReadCallback,
	                                    chr63200AAttrAdvCvcrRset_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_ADV_CVCR_RSET,
	                                            chr63200AAttrAdvCvcrRset_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_ADV_CVCR_VSET,
	                                    "CHR63200A_ATTR_ADV_CVCR_VSET", 10, 0,
	                                    chr63200AAttrAdvCvcrVset_ReadCallback,
	                                    chr63200AAttrAdvCvcrVset_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_ADV_CVCR_VSET,
	                                            chr63200AAttrAdvCvcrVset_CheckCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_ADV_EXT_WAV_MODE,
	                                   "CHR63200A_ATTR_ADV_EXT_WAV_MODE", 0, 0,
	                                   chr63200AAttrAdvExtWavMode_ReadCallback,
	                                   chr63200AAttrAdvExtWavMode_WriteCallback,
	                                   &attrAdvExtWavModeRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_ADV_EXT_WAV_RNG,
	                                   "CHR63200A_ATTR_ADV_EXT_WAV_RNG", 0, 0,
	                                   chr63200AAttrAdvExtWavRng_ReadCallback,
	                                   chr63200AAttrAdvExtWavRng_WriteCallback,
	                                   &attrCurrStatVrngRangeTable));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_ADV_OCP_DWEL,
	                                    "CHR63200A_ATTR_ADV_OCP_DWEL", 0.5, 0,
	                                    chr63200AAttrAdvOcpDwel_ReadCallback,
	                                    chr63200AAttrAdvOcpDwel_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_ADV_OCP_DWEL,
	                                            chr63200AAttrAdvOcpDwel_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_ADV_OCP_END,
	                                    "CHR63200A_ATTR_ADV_OCP_END", 0.5, 0,
	                                    chr63200AAttrAdvOcpEnd_ReadCallback,
	                                    chr63200AAttrAdvOcpEnd_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_ADV_OCP_END,
	                                            chr63200AAttrAdvOcpEnd_CheckCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_ADV_OCP_LATC,
	                                   "CHR63200A_ATTR_ADV_OCP_LATC", 0, 0,
	                                   chr63200AAttrAdvOcpLatc_ReadCallback,
	                                   chr63200AAttrAdvOcpLatc_WriteCallback,
	                                   &attrConfAutoOnRangeTable));
	checkErr (Ivi_AddAttributeViString (vi, CHR63200A_ATTR_ADV_OCP_RES,
	                                    "CHR63200A_ATTR_ADV_OCP_RES", "",
	                                    IVI_VAL_NOT_USER_WRITABLE,
	                                    chr63200AAttrAdvOcpRes_ReadCallback, VI_NULL));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_ADV_OCP_SPEC_H,
	                                    "CHR63200A_ATTR_ADV_OCP_SPEC_H", 0.5, 0,
	                                    chr63200AAttrAdvOcpSpecH_ReadCallback,
	                                    chr63200AAttrAdvOcpSpecH_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_ADV_OCP_SPEC_H,
	                                            chr63200AAttrAdvOcpSpecH_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_ADV_OCP_SPEC_L,
	                                    "CHR63200A_ATTR_ADV_OCP_SPEC_L", 0.5, 0,
	                                    chr63200AAttrAdvOcpSpecL_ReadCallback,
	                                    chr63200AAttrAdvOcpSpecL_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_ADV_OCP_SPEC_L,
	                                            chr63200AAttrAdvOcpSpecL_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_ADV_OCP_STAR,
	                                    "CHR63200A_ATTR_ADV_OCP_STAR", 0.5, 0,
	                                    chr63200AAttrAdvOcpStar_ReadCallback,
	                                    chr63200AAttrAdvOcpStar_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_ADV_OCP_STAR,
	                                            chr63200AAttrAdvOcpStar_CheckCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_ADV_OCP_STEP,
	                                   "CHR63200A_ATTR_ADV_OCP_STEP", 1, 0,
	                                   chr63200AAttrAdvOcpStep_ReadCallback,
	                                   chr63200AAttrAdvOcpStep_WriteCallback,
	                                   VI_NULL));
	checkErr (Ivi_SetAttrCheckCallbackViInt32 (vi, CHR63200A_ATTR_ADV_OCP_STEP,
	                                           chr63200AAttrAdvOcpStep_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_ADV_OCP_TRIG_VOLT,
	                                    "CHR63200A_ATTR_ADV_OCP_TRIG_VOLT", 0.5, 0,
	                                    chr63200AAttrAdvOcpTrigVolt_ReadCallback,
	                                    chr63200AAttrAdvOcpTrigVolt_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_ADV_OCP_TRIG_VOLT,
	                                            chr63200AAttrAdvOcpTrigVolt_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_ADV_OPP_DWEL,
	                                    "CHR63200A_ATTR_ADV_OPP_DWEL", 0.5, 0,
	                                    chr63200AAttrAdvOppDwel_ReadCallback,
	                                    chr63200AAttrAdvOppDwel_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_ADV_OPP_DWEL,
	                                            chr63200AAttrAdvOppDwel_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_ADV_OPP_END,
	                                    "CHR63200A_ATTR_ADV_OPP_END", 100, 0,
	                                    chr63200AAttrAdvOppEnd_ReadCallback,
	                                    chr63200AAttrAdvOppEnd_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_ADV_OPP_END,
	                                            chr63200AAttrAdvOppEnd_CheckCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_ADV_OPP_LATC,
	                                   "CHR63200A_ATTR_ADV_OPP_LATC", 0, 0,
	                                   chr63200AAttrAdvOppLatc_ReadCallback,
	                                   chr63200AAttrAdvOppLatc_WriteCallback,
	                                   &attrConfAutoOnRangeTable));
	checkErr (Ivi_AddAttributeViString (vi, CHR63200A_ATTR_ADV_OPP_RES,
	                                    "CHR63200A_ATTR_ADV_OPP_RES", "",
	                                    IVI_VAL_NOT_USER_WRITABLE,
	                                    chr63200AAttrAdvOppRes_ReadCallback, VI_NULL));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_ADV_OPP_SPEC_H,
	                                    "CHR63200A_ATTR_ADV_OPP_SPEC_H", 0.5, 0,
	                                    chr63200AAttrAdvOppSpecH_ReadCallback,
	                                    chr63200AAttrAdvOppSpecH_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_ADV_OPP_SPEC_H,
	                                            chr63200AAttrAdvOppSpecH_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_ADV_OPP_SPEC_L,
	                                    "CHR63200A_ATTR_ADV_OPP_SPEC_L", 0.5, 0,
	                                    chr63200AAttrAdvOppSpecL_ReadCallback,
	                                    chr63200AAttrAdvOppSpecL_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_ADV_OPP_SPEC_L,
	                                            chr63200AAttrAdvOppSpecL_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_ADV_OPP_STAR,
	                                    "CHR63200A_ATTR_ADV_OPP_STAR", 100, 0,
	                                    chr63200AAttrAdvOppStar_ReadCallback,
	                                    chr63200AAttrAdvOppStar_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_ADV_OPP_STAR,
	                                            chr63200AAttrAdvOppStar_CheckCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_ADV_OPP_STEP,
	                                   "CHR63200A_ATTR_ADV_OPP_STEP", 1, 0,
	                                   chr63200AAttrAdvOppStep_ReadCallback,
	                                   chr63200AAttrAdvOppStep_WriteCallback,
	                                   VI_NULL));
	checkErr (Ivi_SetAttrCheckCallbackViInt32 (vi, CHR63200A_ATTR_ADV_OPP_STEP,
	                                           chr63200AAttrAdvOppStep_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_ADV_OPP_TRIG_VOLT,
	                                    "CHR63200A_ATTR_ADV_OPP_TRIG_VOLT", 0.5, 0,
	                                    chr63200AAttrAdvOppTrigVolt_ReadCallback,
	                                    chr63200AAttrAdvOppTrigVolt_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_ADV_OPP_TRIG_VOLT,
	                                            chr63200AAttrAdvOppTrigVolt_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_ADV_SINE_FREQ,
	                                    "CHR63200A_ATTR_ADV_SINE_FREQ", 1.0, 0,
	                                    chr63200AAttrAdvSineFreq_ReadCallback,
	                                    chr63200AAttrAdvSineFreq_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_ADV_SINE_FREQ,
	                                            chr63200AAttrAdvSineFreq_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_ADV_SINE_IAC,
	                                    "CHR63200A_ATTR_ADV_SINE_IAC", 0.5, 0,
	                                    chr63200AAttrAdvSineIac_ReadCallback,
	                                    chr63200AAttrAdvSineIac_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_ADV_SINE_IAC,
	                                            chr63200AAttrAdvSineIac_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_ADV_SINE_IDC,
	                                    "CHR63200A_ATTR_ADV_SINE_IDC", 0.5, 0,
	                                    chr63200AAttrAdvSineIdc_ReadCallback,
	                                    chr63200AAttrAdvSineIdc_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_ADV_SINE_IDC,
	                                            chr63200AAttrAdvSineIdc_CheckCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_ADV_USER_WAV_CLE,
	                                   "CHR63200A_ATTR_ADV_USER_WAV_CLE", 0,
	                                   IVI_VAL_NEVER_CACHE | IVI_VAL_DONT_CHECK_STATUS,
	                                   chr63200AAttrAdvUserWavCle_ReadCallback,
	                                   VI_NULL, &attrAdvUserWavCleRangeTable));
	checkErr (Ivi_AddAttributeViString (vi, CHR63200A_ATTR_ADV_USER_WAV_DATA,
	                                    "CHR63200A_ATTR_ADV_USER_WAV_DATA",
	                                    "1,0.001,1,0,YES", IVI_VAL_NEVER_CACHE,
	                                    chr63200AAttrAdvUserWavData_ReadCallback,
	                                    chr63200AAttrAdvUserWavData_WriteCallback));
	checkErr (Ivi_SetAttrCheckCallbackViString (vi, CHR63200A_ATTR_ADV_USER_WAV_DATA,
	                                            chr63200AAttrAdvUserWavData_CheckCallback));
	checkErr (Ivi_AddAttributeViString (vi, CHR63200A_ATTR_ADV_USER_WAV_DATA_POIN,
	                                    "CHR63200A_ATTR_ADV_USER_WAV_DATA_POIN", "",
	                                    IVI_VAL_NEVER_CACHE,
	                                    chr63200AAttrAdvUserWavDataPoin_ReadCallback,
	                                    chr63200AAttrAdvUserWavDataPoin_WriteCallback));
	checkErr (Ivi_SetAttrCheckCallbackViString (vi,
	                                            CHR63200A_ATTR_ADV_USER_WAV_DATA_POIN,
	                                            chr63200AAttrAdvUserWavDataPoin_CheckCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_ADV_USER_WAV_DATA_STAT,
	                                   "CHR63200A_ATTR_ADV_USER_WAV_DATA_STAT", 0,
	                                   IVI_VAL_NEVER_CACHE,
	                                   chr63200AAttrAdvUserWavDataStat_ReadCallback,
	                                   VI_NULL, &attrAdvUserWavDataStatRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_ADV_USER_WAV_EXE_STAT,
	                                   "CHR63200A_ATTR_ADV_USER_WAV_EXE_STAT", 0,
	                                   IVI_VAL_NEVER_CACHE,
	                                   chr63200AAttrAdvUserWavExeStat_ReadCallback,
	                                   VI_NULL, &attrAdvUserWavExeStatRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_ADV_USER_WAV_NSEL,
	                                   "CHR63200A_ATTR_ADV_USER_WAV_NSEL", 1, 0,
	                                   chr63200AAttrAdvUserWavNsel_ReadCallback,
	                                   chr63200AAttrAdvUserWavNsel_WriteCallback,
	                                   VI_NULL));
	checkErr (Ivi_SetAttrCheckCallbackViInt32 (vi, CHR63200A_ATTR_ADV_USER_WAV_NSEL,
	                                           chr63200AAttrAdvUserWavNsel_CheckCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_ADV_USER_WAV_REM,
	                                   "CHR63200A_ATTR_ADV_USER_WAV_REM", 1,
	                                   IVI_VAL_NOT_USER_WRITABLE | IVI_VAL_NEVER_CACHE,
	                                   chr63200AAttrAdvUserWavRem_ReadCallback,
	                                   VI_NULL, VI_NULL));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_CONF_AUTO_ON,
	                                   "CHR63200A_ATTR_CONF_AUTO_ON", 0, 0,
	                                   chr63200AAttrConfAutoOn_ReadCallback,
	                                   chr63200AAttrConfAutoOn_WriteCallback,
	                                   &attrConfAutoOnRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_CONF_DIO_IN1,
	                                   "CHR63200A_ATTR_CONF_DIO_IN1", 0, 0,
	                                   chr63200AAttrConfDioIn1_ReadCallback,
	                                   chr63200AAttrConfDioIn1_WriteCallback,
	                                   &attrConfDioIn1RangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_CONF_DIO_IN2,
	                                   "CHR63200A_ATTR_CONF_DIO_IN2", 0, 0,
	                                   chr63200AAttrConfDioIn2_ReadCallback,
	                                   chr63200AAttrConfDioIn2_WriteCallback,
	                                   &attrConfDioIn1RangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_CONF_DIO_OUT1,
	                                   "CHR63200A_ATTR_CONF_DIO_OUT1", 0, 0,
	                                   chr63200AAttrConfDioOut1_ReadCallback,
	                                   chr63200AAttrConfDioOut1_WriteCallback,
	                                   &attrConfDioOut1RangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_CONF_DIO_OUT2,
	                                   "CHR63200A_ATTR_CONF_DIO_OUT2", 0, 0,
	                                   chr63200AAttrConfDioOut2_ReadCallback,
	                                   chr63200AAttrConfDioOut2_WriteCallback,
	                                   &attrConfDioOut1RangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_CONF_ENT_KEY,
	                                   "CHR63200A_ATTR_CONF_ENT_KEY", 0, 0,
	                                   chr63200AAttrConfEntKey_ReadCallback,
	                                   chr63200AAttrConfEntKey_WriteCallback,
	                                   &attrConfEntKeyRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_CONF_PARA_INIT,
	                                   "CHR63200A_ATTR_CONF_PARA_INIT", 0, 0,
	                                   chr63200AAttrConfParaInit_ReadCallback,
	                                   chr63200AAttrConfParaInit_WriteCallback,
	                                   &attrConfParaInitRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_CONF_PARA_MODE,
	                                   "CHR63200A_ATTR_CONF_PARA_MODE", 0, 0,
	                                   chr63200AAttrConfParaMode_ReadCallback,
	                                   chr63200AAttrConfParaMode_WriteCallback,
	                                   &attrConfParaModeRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_CONF_SHOR_KEY,
	                                   "CHR63200A_ATTR_CONF_SHOR_KEY", 0, 0,
	                                   chr63200AAttrConfShorKey_ReadCallback,
	                                   chr63200AAttrConfShorKey_WriteCallback,
	                                   &attrConfShorKeyRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_CONF_SOUN,
	                                   "CHR63200A_ATTR_CONF_SOUN", 0, 0,
	                                   chr63200AAttrConfSoun_ReadCallback,
	                                   chr63200AAttrConfSoun_WriteCallback,
	                                   &attrConfSounRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_CONF_SYNC_MODE,
	                                   "CHR63200A_ATTR_CONF_SYNC_MODE", 0, 0,
	                                   chr63200AAttrConfSyncMode_ReadCallback,
	                                   chr63200AAttrConfSyncMode_WriteCallback,
	                                   &attrConfSyncModeRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_CONF_VOLT_LATC,
	                                   "CHR63200A_ATTR_CONF_VOLT_LATC", 0, 0,
	                                   chr63200AAttrConfVoltLatc_ReadCallback,
	                                   chr63200AAttrConfVoltLatc_WriteCallback,
	                                   &attrConfVoltLatcRangeTable));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_CONF_VOLT_OFF,
	                                    "CHR63200A_ATTR_CONF_VOLT_OFF", 0.5, 0,
	                                    chr63200AAttrConfVoltOff_ReadCallback,
	                                    chr63200AAttrConfVoltOff_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_CONF_VOLT_OFF,
	                                            chr63200AAttrConfVoltOff_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_CONF_VOLT_ON,
	                                    "CHR63200A_ATTR_CONF_VOLT_ON", 0.5, 0,
	                                    chr63200AAttrConfVoltOn_ReadCallback,
	                                    chr63200AAttrConfVoltOn_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_CONF_VOLT_ON,
	                                            chr63200AAttrConfVoltOn_CheckCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_CONF_VOLT_SIGN,
	                                   "CHR63200A_ATTR_CONF_VOLT_SIGN", 0, 0,
	                                   chr63200AAttrConfVoltSign_ReadCallback,
	                                   chr63200AAttrConfVoltSign_WriteCallback,
	                                   &attrConfVoltSignRangeTable));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_CONF_WIND,
	                                    "CHR63200A_ATTR_CONF_WIND", 0.5, 0,
	                                    chr63200AAttrConfWind_ReadCallback,
	                                    chr63200AAttrConfWind_WriteCallback, VI_NULL,
	                                    0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_CONF_WIND,
	                                            chr63200AAttrConfWind_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_CURR_DYN_FALL,
	                                    "CHR63200A_ATTR_CURR_DYN_FALL", 1.0, 0,
	                                    chr63200AAttrCurrDynFall_ReadCallback,
	                                    chr63200AAttrCurrDynFall_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_CURR_DYN_FALL,
	                                            chr63200AAttrCurrDynFall_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_CURR_DYN_L1,
	                                    "CHR63200A_ATTR_CURR_DYN_L1", 10, 0,
	                                    chr63200AAttrCurrDynL1_ReadCallback,
	                                    chr63200AAttrCurrDynL1_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_CURR_DYN_L1,
	                                            chr63200AAttrCurrDynL1_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_CURR_DYN_L2,
	                                    "CHR63200A_ATTR_CURR_DYN_L2", 10, 0,
	                                    chr63200AAttrCurrDynL2_ReadCallback,
	                                    chr63200AAttrCurrDynL2_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_CURR_DYN_L2,
	                                            chr63200AAttrCurrDynL2_CheckCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_CURR_DYN_REP,
	                                   "CHR63200A_ATTR_CURR_DYN_REP", 0, 0,
	                                   chr63200AAttrCurrDynRep_ReadCallback,
	                                   chr63200AAttrCurrDynRep_WriteCallback,
	                                   VI_NULL));
	checkErr (Ivi_SetAttrCheckCallbackViInt32 (vi, CHR63200A_ATTR_CURR_DYN_REP,
	                                           chr63200AAttrCurrDynRep_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_CURR_DYN_RISE,
	                                    "CHR63200A_ATTR_CURR_DYN_RISE", 1.0, 0,
	                                    chr63200AAttrCurrDynRise_ReadCallback,
	                                    chr63200AAttrCurrDynRise_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_CURR_DYN_RISE,
	                                            chr63200AAttrCurrDynRise_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_CURR_DYN_T1,
	                                    "CHR63200A_ATTR_CURR_DYN_T1", 0.1, 0,
	                                    chr63200AAttrCurrDynT1_ReadCallback,
	                                    chr63200AAttrCurrDynT1_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_CURR_DYN_T1,
	                                            chr63200AAttrCurrDynT1_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_CURR_DYN_T2,
	                                    "CHR63200A_ATTR_CURR_DYN_T2", 0.1, 0,
	                                    chr63200AAttrCurrDynT2_ReadCallback,
	                                    chr63200AAttrCurrDynT2_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_CURR_DYN_T2,
	                                            chr63200AAttrCurrDynT2_CheckCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_CURR_DYN_VRNG,
	                                   "CHR63200A_ATTR_CURR_DYN_VRNG", 0, 0,
	                                   chr63200AAttrCurrDynVrng_ReadCallback,
	                                   chr63200AAttrCurrDynVrng_WriteCallback,
	                                   &attrCurrDynVrngRangeTable));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_CURR_STAT_FALL,
	                                    "CHR63200A_ATTR_CURR_STAT_FALL", 1, 0,
	                                    chr63200AAttrCurrStatFall_ReadCallback,
	                                    chr63200AAttrCurrStatFall_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_CURR_STAT_FALL,
	                                            chr63200AAttrCurrStatFall_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_CURR_STAT_L1,
	                                    "CHR63200A_ATTR_CURR_STAT_L1", 10, 0,
	                                    chr63200AAttrCurrStatL1_ReadCallback,
	                                    chr63200AAttrCurrStatL1_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_CURR_STAT_L1,
	                                            chr63200AAttrCurrStatL1_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_CURR_STAT_L2,
	                                    "CHR63200A_ATTR_CURR_STAT_L2", 10, 0,
	                                    chr63200AAttrCurrStatL2_ReadCallback,
	                                    chr63200AAttrCurrStatL2_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_CURR_STAT_L2,
	                                            chr63200AAttrCurrStatL2_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_CURR_STAT_RISE,
	                                    "CHR63200A_ATTR_CURR_STAT_RISE", 1, 0,
	                                    chr63200AAttrCurrStatRise_ReadCallback,
	                                    chr63200AAttrCurrStatRise_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_CURR_STAT_RISE,
	                                            chr63200AAttrCurrStatRise_CheckCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_CURR_STAT_VRNG,
	                                   "CHR63200A_ATTR_CURR_STAT_VRNG", 0, 0,
	                                   chr63200AAttrCurrStatVrng_ReadCallback,
	                                   chr63200AAttrCurrStatVrng_WriteCallback,
	                                   &attrCurrStatVrngRangeTable));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_CURR_SWE_DUTY,
	                                    "CHR63200A_ATTR_CURR_SWE_DUTY", 1, 0,
	                                    chr63200AAttrCurrSweDuty_ReadCallback,
	                                    chr63200AAttrCurrSweDuty_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_CURR_SWE_DUTY,
	                                            chr63200AAttrCurrSweDuty_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_CURR_SWE_DWEL,
	                                    "CHR63200A_ATTR_CURR_SWE_DWEL", 1.0, 0,
	                                    chr63200AAttrCurrSweDwel_ReadCallback,
	                                    chr63200AAttrCurrSweDwel_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_CURR_SWE_DWEL,
	                                            chr63200AAttrCurrSweDwel_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_CURR_SWE_FALL,
	                                    "CHR63200A_ATTR_CURR_SWE_FALL", 2.5, 0,
	                                    chr63200AAttrCurrSweFall_ReadCallback,
	                                    chr63200AAttrCurrSweFall_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_CURR_SWE_FALL,
	                                            chr63200AAttrCurrSweFall_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_CURR_SWE_FEND,
	                                    "CHR63200A_ATTR_CURR_SWE_FEND", 0.01, 0,
	                                    chr63200AAttrCurrSweFend_ReadCallback,
	                                    chr63200AAttrCurrSweFend_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_CURR_SWE_FEND,
	                                            chr63200AAttrCurrSweFend_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_CURR_SWE_FSTA,
	                                    "CHR63200A_ATTR_CURR_SWE_FSTA", 0.01, 0,
	                                    chr63200AAttrCurrSweFsta_ReadCallback,
	                                    chr63200AAttrCurrSweFsta_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_CURR_SWE_FSTA,
	                                            chr63200AAttrCurrSweFsta_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_CURR_SWE_FSTEP,
	                                    "CHR63200A_ATTR_CURR_SWE_FSTEP", 0.01, 0,
	                                    chr63200AAttrCurrSweFstep_ReadCallback,
	                                    chr63200AAttrCurrSweFstep_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_CURR_SWE_FSTEP,
	                                            chr63200AAttrCurrSweFstep_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_CURR_SWE_IMAX,
	                                    "CHR63200A_ATTR_CURR_SWE_IMAX", 20, 0,
	                                    chr63200AAttrCurrSweImax_ReadCallback,
	                                    chr63200AAttrCurrSweImax_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_CURR_SWE_IMAX,
	                                            chr63200AAttrCurrSweImax_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_CURR_SWE_IMIN,
	                                    "CHR63200A_ATTR_CURR_SWE_IMIN", 0, 0,
	                                    chr63200AAttrCurrSweImin_ReadCallback,
	                                    chr63200AAttrCurrSweImin_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_CURR_SWE_IMIN,
	                                            chr63200AAttrCurrSweImin_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_CURR_SWE_RISE,
	                                    "CHR63200A_ATTR_CURR_SWE_RISE", 2.5, 0,
	                                    chr63200AAttrCurrSweRise_ReadCallback,
	                                    chr63200AAttrCurrSweRise_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_CURR_SWE_RISE,
	                                            chr63200AAttrCurrSweRise_CheckCallback));

	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_DIG_SAMP_POIN,
	                                   "CHR63200A_ATTR_DIG_SAMP_POIN", 1, 0,
	                                   chr63200AAttrDigSampPoin_ReadCallback,
	                                   chr63200AAttrDigSampPoin_WriteCallback,
	                                   VI_NULL));
	checkErr (Ivi_SetAttrCheckCallbackViInt32 (vi, CHR63200A_ATTR_DIG_SAMP_POIN,
	                                           chr63200AAttrDigSampPoin_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_DIG_SAMP_TIME,
	                                    "CHR63200A_ATTR_DIG_SAMP_TIME", 0.02, 0,
	                                    chr63200AAttrDigSampTime_ReadCallback,
	                                    chr63200AAttrDigSampTime_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_DIG_SAMP_TIME,
	                                            chr63200AAttrDigSampTime_CheckCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_DIG_TRIG,
	                                   "CHR63200A_ATTR_DIG_TRIG", 0,
	                                   IVI_VAL_NEVER_CACHE,
	                                   chr63200AAttrDigTrig_ReadCallback,
	                                   chr63200AAttrDigTrig_WriteCallback,
	                                   &attrDigTrigRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_DIG_TRIG_POIN,
	                                   "CHR63200A_ATTR_DIG_TRIG_POIN", 1, 0,
	                                   chr63200AAttrDigTrigPoin_ReadCallback,
	                                   chr63200AAttrDigTrigPoin_WriteCallback,
	                                   VI_NULL));
	checkErr (Ivi_SetAttrCheckCallbackViInt32 (vi, CHR63200A_ATTR_DIG_TRIG_POIN,
	                                           chr63200AAttrDigTrigPoin_CheckCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_DIG_TRIG_SOUR,
	                                   "CHR63200A_ATTR_DIG_TRIG_SOUR", 0, 0,
	                                   chr63200AAttrDigTrigSour_ReadCallback,
	                                   chr63200AAttrDigTrigSour_WriteCallback,
	                                   &attrDigTrigSourRangeTable));
	checkErr (Ivi_AddAttributeViString (vi, CHR63200A_ATTR_DIG_WAV_CAP,
	                                    "CHR63200A_ATTR_DIG_WAV_CAP", "WAIT",
	                                    IVI_VAL_NOT_USER_WRITABLE | IVI_VAL_NEVER_CACHE,
	                                    chr63200AAttrDigWavCap_ReadCallback, VI_NULL));

	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_DIO_OUT1,
	                                   "CHR63200A_ATTR_DIO_OUT1", 0, 0,
	                                   chr63200AAttrDioOut1_ReadCallback,
	                                   chr63200AAttrDioOut1_WriteCallback,
	                                   &attrDioOut1RangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_DIO_OUT2,
	                                   "CHR63200A_ATTR_DIO_OUT2", 0, 0,
	                                   chr63200AAttrDioOut2_ReadCallback,
	                                   chr63200AAttrDioOut2_WriteCallback,
	                                   &attrDioOut1RangeTable));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_IMP_STAT_CL,
	                                    "CHR63200A_ATTR_IMP_STAT_CL", 0.02, 0,
	                                    chr63200AAttrImpStatCl_ReadCallback,
	                                    chr63200AAttrImpStatCl_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_IMP_STAT_CL,
	                                            chr63200AAttrImpStatCl_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_IMP_STAT_LS,
	                                    "CHR63200A_ATTR_IMP_STAT_LS", 0.00002, 0,
	                                    chr63200AAttrImpStatLs_ReadCallback,
	                                    chr63200AAttrImpStatLs_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_IMP_STAT_LS,
	                                            chr63200AAttrImpStatLs_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_IMP_STAT_RL,
	                                    "CHR63200A_ATTR_IMP_STAT_RL", 20.0, 0,
	                                    chr63200AAttrImpStatRl_ReadCallback,
	                                    chr63200AAttrImpStatRl_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_IMP_STAT_RL,
	                                            chr63200AAttrImpStatRl_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_IMP_STAT_RS,
	                                    "CHR63200A_ATTR_IMP_STAT_RS", 20, 0,
	                                    chr63200AAttrImpStatRs_ReadCallback,
	                                    chr63200AAttrImpStatRs_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_IMP_STAT_RS,
	                                            chr63200AAttrImpStatRs_CheckCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_LOAD,
	                                   "CHR63200A_ATTR_LOAD", 0, 0,
	                                   chr63200AAttrLoad_ReadCallback,
	                                   chr63200AAttrLoad_WriteCallback,
	                                   &attrLoadRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_LOAD_PROT,
	                                   "CHR63200A_ATTR_LOAD_PROT", 0, 0,
	                                   chr63200AAttrLoadProt_ReadCallback, VI_NULL,
	                                   &attrLoadProtRangeTable));
	checkErr (Ivi_AddAttributeViBoolean (vi, CHR63200A_ATTR_LOAD_PROT_CLE,
	                                     "CHR63200A_ATTR_LOAD_PROT_CLE", 0,
	                                     IVI_VAL_DONT_CHECK_STATUS, VI_NULL,
	                                     chr63200AAttrLoadProtCle_WriteCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_LOAD_SHOR,
	                                   "CHR63200A_ATTR_LOAD_SHOR", 0, 0,
	                                   chr63200AAttrLoadShor_ReadCallback,
	                                   chr63200AAttrLoadShor_WriteCallback,
	                                   &attrLoadShorRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_MODE,
	                                   "CHR63200A_ATTR_MODE", 1, IVI_VAL_NEVER_CACHE,
	                                   chr63200AAttrMode_ReadCallback,
	                                   chr63200AAttrMode_WriteCallback,
	                                   &OperationalModeRangeTable));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_POW_STAT_FALL,
	                                    "CHR63200A_ATTR_POW_STAT_FALL", 1.0, 0,
	                                    chr63200AAttrPowStatFall_ReadCallback,
	                                    chr63200AAttrPowStatFall_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_POW_STAT_FALL,
	                                            chr63200AAttrPowStatFall_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_POW_STAT_L1,
	                                    "CHR63200A_ATTR_POW_STAT_L1", 10, 0,
	                                    chr63200AAttrPowStatL1_ReadCallback,
	                                    chr63200AAttrPowStatL1_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_POW_STAT_L1,
	                                            chr63200AAttrPowStatL1_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_POW_STAT_L2,
	                                    "CHR63200A_ATTR_POW_STAT_L2", 10, 0,
	                                    chr63200AAttrPowStatL2_ReadCallback,
	                                    chr63200AAttrPowStatL2_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_POW_STAT_L2,
	                                            chr63200AAttrPowStatL2_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_POW_STAT_RISE,
	                                    "CHR63200A_ATTR_POW_STAT_RISE", 1.0, 0,
	                                    chr63200AAttrPowStatRise_ReadCallback,
	                                    chr63200AAttrPowStatRise_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_POW_STAT_RISE,
	                                            chr63200AAttrPowStatRise_CheckCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_POW_STAT_VRNG,
	                                   "CHR63200A_ATTR_POW_STAT_VRNG", 0, 0,
	                                   chr63200AAttrPowStatVrng_ReadCallback,
	                                   chr63200AAttrPowStatVrng_WriteCallback,
	                                   &attrConfVoltRangRangeTable));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_RES_DYN_FALL,
	                                    "CHR63200A_ATTR_RES_DYN_FALL", 1.0, 0,
	                                    chr63200AAttrResDynFall_ReadCallback,
	                                    chr63200AAttrResDynFall_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_RES_DYN_FALL,
	                                            chr63200AAttrResDynFall_CheckCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_RES_DYN_IRNG,
	                                   "CHR63200A_ATTR_RES_DYN_IRNG", 0, 0,
	                                   chr63200AAttrResDynIrng_ReadCallback,
	                                   chr63200AAttrResDynIrng_WriteCallback,
	                                   &attrConfVoltRangRangeTable));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_RES_DYN_L1,
	                                    "CHR63200A_ATTR_RES_DYN_L1", 10, 0,
	                                    chr63200AAttrResDynL1_ReadCallback,
	                                    chr63200AAttrResDynL1_WriteCallback, VI_NULL,
	                                    0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_RES_DYN_L1,
	                                            chr63200AAttrResDynL1_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_RES_DYN_L2,
	                                    "CHR63200A_ATTR_RES_DYN_L2", 10, 0,
	                                    chr63200AAttrResDynL2_ReadCallback,
	                                    chr63200AAttrResDynL2_WriteCallback, VI_NULL,
	                                    0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_RES_DYN_L2,
	                                            chr63200AAttrResDynL2_CheckCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_RES_DYN_REP,
	                                   "CHR63200A_ATTR_RES_DYN_REP", 0, 0,
	                                   chr63200AAttrResDynRep_ReadCallback,
	                                   chr63200AAttrResDynRep_WriteCallback, VI_NULL));
	checkErr (Ivi_SetAttrCheckCallbackViInt32 (vi, CHR63200A_ATTR_RES_DYN_REP,
	                                           chr63200AAttrResDynRep_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_RES_DYN_RISE,
	                                    "CHR63200A_ATTR_RES_DYN_RISE", 1.0, 0,
	                                    chr63200AAttrResDynRise_ReadCallback,
	                                    chr63200AAttrResDynRise_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_RES_DYN_RISE,
	                                            chr63200AAttrResDynRise_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_RES_DYN_T1,
	                                    "CHR63200A_ATTR_RES_DYN_T1", 0.1, 0,
	                                    chr63200AAttrResDynT1_ReadCallback,
	                                    chr63200AAttrResDynT1_WriteCallback, VI_NULL,
	                                    0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_RES_DYN_T1,
	                                            chr63200AAttrResDynT1_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_RES_DYN_T2,
	                                    "CHR63200A_ATTR_RES_DYN_T2", 0.1, 0,
	                                    chr63200AAttrResDynT2_ReadCallback,
	                                    chr63200AAttrResDynT2_WriteCallback, VI_NULL,
	                                    0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_RES_DYN_T2,
	                                            chr63200AAttrResDynT2_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_RES_STAT_FALL,
	                                    "CHR63200A_ATTR_RES_STAT_FALL", 1.0, 0,
	                                    chr63200AAttrResStatFall_ReadCallback,
	                                    chr63200AAttrResStatFall_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_RES_STAT_FALL,
	                                            chr63200AAttrResStatFall_CheckCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_RES_STAT_IRNG,
	                                   "CHR63200A_ATTR_RES_STAT_IRNG", 0, 0,
	                                   chr63200AAttrResStatIrng_ReadCallback,
	                                   chr63200AAttrResStatIrng_WriteCallback,
	                                   &attrConfVoltRangRangeTable));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_RES_STAT_L1,
	                                    "CHR63200A_ATTR_RES_STAT_L1", 10, 0,
	                                    chr63200AAttrResStatL1_ReadCallback,
	                                    chr63200AAttrResStatL1_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_RES_STAT_L1,
	                                            chr63200AAttrResStatL1_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_RES_STAT_L2,
	                                    "CHR63200A_ATTR_RES_STAT_L2", 10, 0,
	                                    chr63200AAttrResStatL2_ReadCallback,
	                                    chr63200AAttrResStatL2_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_RES_STAT_L2,
	                                            chr63200AAttrResStatL2_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_RES_STAT_RISE,
	                                    "CHR63200A_ATTR_RES_STAT_RISE", 1.0, 0,
	                                    chr63200AAttrResStatRise_ReadCallback,
	                                    chr63200AAttrResStatRise_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_RES_STAT_RISE,
	                                            chr63200AAttrResStatRise_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_SPEC_CURR_C,
	                                    "CHR63200A_ATTR_SPEC_CURR_C", -1, 0,
	                                    chr63200AAttrSpecCurrC_ReadCallback,
	                                    chr63200AAttrSpecCurrC_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_SPEC_CURR_C,
	                                            chr63200AAttrSpecCurrC_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_SPEC_CURR_H,
	                                    "CHR63200A_ATTR_SPEC_CURR_H", -1, 0,
	                                    chr63200AAttrSpecCurrH_ReadCallback,
	                                    chr63200AAttrSpecCurrH_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_SPEC_CURR_H,
	                                            chr63200AAttrSpecCurrH_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_SPEC_CURR_L,
	                                    "CHR63200A_ATTR_SPEC_CURR_L", -1, 0,
	                                    chr63200AAttrSpecCurrL_ReadCallback,
	                                    chr63200AAttrSpecCurrL_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_SPEC_CURR_L,
	                                            chr63200AAttrSpecCurrL_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_SPEC_POW_C,
	                                    "CHR63200A_ATTR_SPEC_POW_C", -1, 0,
	                                    chr63200AAttrSpecPowC_ReadCallback,
	                                    chr63200AAttrSpecPowC_WriteCallback, VI_NULL,
	                                    0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_SPEC_POW_C,
	                                            chr63200AAttrSpecPowC_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_SPEC_POW_H,
	                                    "CHR63200A_ATTR_SPEC_POW_H", -1, 0,
	                                    chr63200AAttrSpecPowH_ReadCallback,
	                                    chr63200AAttrSpecPowH_WriteCallback, VI_NULL,
	                                    0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_SPEC_POW_H,
	                                            chr63200AAttrSpecPowH_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_SPEC_POW_L,
	                                    "CHR63200A_ATTR_SPEC_POW_L", -1, 0,
	                                    chr63200AAttrSpecPowL_ReadCallback,
	                                    chr63200AAttrSpecPowL_WriteCallback, VI_NULL,
	                                    0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_SPEC_POW_L,
	                                            chr63200AAttrSpecPowL_CheckCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_SPEC_TEST,
	                                   "CHR63200A_ATTR_SPEC_TEST", 0, 0,
	                                   chr63200AAttrSpecTest_ReadCallback,
	                                   chr63200AAttrSpecTest_WriteCallback,
	                                   &attrConfAutoOnRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_SPEC_UNIT,
	                                   "CHR63200A_ATTR_SPEC_UNIT", 0, 0,
	                                   chr63200AAttrSpecUnit_ReadCallback,
	                                   chr63200AAttrSpecUnit_WriteCallback,
	                                   &attrSpecUnitRangeTable));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_SPEC_VOLT_C,
	                                    "CHR63200A_ATTR_SPEC_VOLT_C", -1, 0,
	                                    chr63200AAttrSpecVoltC_ReadCallback,
	                                    chr63200AAttrSpecVoltC_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_SPEC_VOLT_C,
	                                            chr63200AAttrSpecVoltC_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_SPEC_VOLT_H,
	                                    "CHR63200A_ATTR_SPEC_VOLT_H", -1, 0,
	                                    chr63200AAttrSpecVoltH_ReadCallback,
	                                    chr63200AAttrSpecVoltH_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_SPEC_VOLT_H,
	                                            chr63200AAttrSpecVoltH_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_SPEC_VOLT_L,
	                                    "CHR63200A_ATTR_SPEC_VOLT_L", -1, 0,
	                                    chr63200AAttrSpecVoltL_ReadCallback,
	                                    chr63200AAttrSpecVoltL_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_SPEC_VOLT_L,
	                                            chr63200AAttrSpecVoltL_CheckCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_SYNC_RUN,
	                                   "CHR63200A_ATTR_SYNC_RUN", 0, 0, VI_NULL,
	                                   chr63200AAttrSyncRun_WriteCallback,
	                                   &attrConfAutoOnRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_SYNC_TYPE,
	                                   "CHR63200A_ATTR_SYNC_TYPE", 0, 0, VI_NULL,
	                                   chr63200AAttrSyncType_WriteCallback,
	                                   &attrConfSyncModeRangeTable));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_VOLT_STAT_ILIM,
	                                    "CHR63200A_ATTR_VOLT_STAT_ILIM", 0, 0,
	                                    chr63200AAttrVoltStatIlim_ReadCallback,
	                                    chr63200AAttrVoltStatIlim_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_VOLT_STAT_ILIM,
	                                            chr63200AAttrVoltStatIlim_CheckCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_VOLT_STAT_IRNG,
	                                   "CHR63200A_ATTR_VOLT_STAT_IRNG", 0, 0,
	                                   chr63200AAttrVoltStatIrng_ReadCallback,
	                                   chr63200AAttrVoltStatIrng_WriteCallback,
	                                   &attrConfVoltRangRangeTable));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_VOLT_STAT_L1,
	                                    "CHR63200A_ATTR_VOLT_STAT_L1", 5, 0,
	                                    chr63200AAttrVoltStatL1_ReadCallback,
	                                    chr63200AAttrVoltStatL1_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_VOLT_STAT_L1,
	                                            chr63200AAttrVoltStatL1_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_VOLT_STAT_L2,
	                                    "CHR63200A_ATTR_VOLT_STAT_L2", 5, 0,
	                                    chr63200AAttrVoltStatL2_ReadCallback,
	                                    chr63200AAttrVoltStatL2_WriteCallback,
	                                    VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_VOLT_STAT_L2,
	                                            chr63200AAttrVoltStatL2_CheckCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_VOLT_STAT_RES,
	                                   "CHR63200A_ATTR_VOLT_STAT_RES", 0, 0,
	                                   chr63200AAttrVoltStatRes_ReadCallback,
	                                   chr63200AAttrVoltStatRes_WriteCallback,
	                                   &attrVoltStatResRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_PROG_NUM,
	                                   "CHR63200A_ATTR_PROG_NUM", 1, 0,
	                                   chr63200AAttrProgNum_ReadCallback,
	                                   chr63200AAttrProgNum_WriteCallback, VI_NULL));
	checkErr (Ivi_SetAttrCheckCallbackViInt32 (vi, CHR63200A_ATTR_PROG_NUM,
	                                           chr63200AAttrProgNum_CheckCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_PROG_TYPE,
	                                   "CHR63200A_ATTR_PROG_TYPE", 0,
	                                   IVI_VAL_ALWAYS_CACHE | IVI_VAL_HIDDEN,
	                                   VI_NULL, VI_NULL, &attrProgTypeRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_PROG_CHAIN,
	                                   "CHR63200A_ATTR_PROG_CHAIN", 0,
	                                   IVI_VAL_ALWAYS_CACHE | IVI_VAL_HIDDEN,
	                                   VI_NULL, VI_NULL, &attrProgChainRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_PROG_REPEAT,
	                                   "CHR63200A_ATTR_PROG_REPEAT", 0,
	                                   IVI_VAL_ALWAYS_CACHE | IVI_VAL_HIDDEN,
	                                   VI_NULL, VI_NULL, &attrProgRepeatRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_PROG_SEQUENCE_NUM,
	                                   "CHR63200A_ATTR_PROG_SEQUENCE_NUM", 1,
	                                   IVI_VAL_ALWAYS_CACHE | IVI_VAL_HIDDEN,
	                                   VI_NULL, VI_NULL,
	                                   &attrProgSequenceNumRangeTable));
	checkErr (Ivi_SetAttrCheckCallbackViInt32 (vi, CHR63200A_ATTR_PROG_SEQUENCE_NUM,
	                                           chr63200AAttrProgSequenceNum_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_PROG_LIST_VALUE,
	                                    "CHR63200A_ATTR_PROG_LIST_VALUE", 0,
	                                    IVI_VAL_ALWAYS_CACHE | IVI_VAL_HIDDEN,
	                                    VI_NULL, VI_NULL, VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_PROG_LIST_VALUE,
	                                            chr63200AAttrProgDataValue_CheckCallback));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_PROG_SEQUENCE_INDEX,
	                                   "CHR63200A_ATTR_PROG_SEQUENCE_INDEX", 1,
	                                   IVI_VAL_ALWAYS_CACHE | IVI_VAL_HIDDEN,
	                                   VI_NULL, VI_NULL, VI_NULL));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_PROG_STEP_START,
	                                    "CHR63200A_ATTR_PROG_STEP_START", 0,
	                                    IVI_VAL_ALWAYS_CACHE | IVI_VAL_HIDDEN,
	                                    VI_NULL, VI_NULL, VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_PROG_STEP_START,
	                                            chr63200AAttrProgDataValue_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_PROG_STEP_END,
	                                    "CHR63200A_ATTR_PROG_STEP_END", 0,
	                                    IVI_VAL_ALWAYS_CACHE | IVI_VAL_HIDDEN,
	                                    VI_NULL, VI_NULL, VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_PROG_STEP_END,
	                                            chr63200AAttrProgDataValue_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_PROG_RISE,
	                                    "CHR63200A_ATTR_PROG_RISE", 0.5,
	                                    IVI_VAL_ALWAYS_CACHE | IVI_VAL_HIDDEN,
	                                    VI_NULL, VI_NULL, VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_PROG_RISE,
	                                            chr63200AAttrProgDataValue_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_PROG_FALL,
	                                    "CHR63200A_ATTR_PROG_FALL", 0.5,
	                                    IVI_VAL_ALWAYS_CACHE | IVI_VAL_HIDDEN,
	                                    VI_NULL, VI_NULL, VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_PROG_FALL,
	                                            chr63200AAttrProgDataValue_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_PROG_DWELL,
	                                    "CHR63200A_ATTR_PROG_DWELL", 0.1,
	                                    IVI_VAL_ALWAYS_CACHE | IVI_VAL_HIDDEN,
	                                    VI_NULL, VI_NULL, VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_PROG_DWELL,
	                                            chr63200AAttrProgDataValue_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_PROG_VH,
	                                    "CHR63200A_ATTR_PROG_VH", -1,
	                                    IVI_VAL_ALWAYS_CACHE | IVI_VAL_HIDDEN,
	                                    VI_NULL, VI_NULL, VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_PROG_VH,
	                                            chr63200AAttrProgDataValue_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_PROG_VL,
	                                    "CHR63200A_ATTR_PROG_VL", -1,
	                                    IVI_VAL_ALWAYS_CACHE | IVI_VAL_HIDDEN,
	                                    VI_NULL, VI_NULL, VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_PROG_VL,
	                                            chr63200AAttrProgDataValue_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_PROG_IH,
	                                    "CHR63200A_ATTR_PROG_IH", -1,
	                                    IVI_VAL_ALWAYS_CACHE | IVI_VAL_HIDDEN,
	                                    VI_NULL, VI_NULL, VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_PROG_IH,
	                                            chr63200AAttrProgDataValue_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_PROG_IL,
	                                    "CHR63200A_ATTR_PROG_IL", -1,
	                                    IVI_VAL_ALWAYS_CACHE | IVI_VAL_HIDDEN,
	                                    VI_NULL, VI_NULL, VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_PROG_IL,
	                                            chr63200AAttrProgDataValue_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_PROG_PH,
	                                    "CHR63200A_ATTR_PROG_PH", -1,
	                                    IVI_VAL_ALWAYS_CACHE | IVI_VAL_HIDDEN,
	                                    VI_NULL, VI_NULL, VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_PROG_PH,
	                                            chr63200AAttrProgDataValue_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_PROG_PL,
	                                    "CHR63200A_ATTR_PROG_PL", -1,
	                                    IVI_VAL_ALWAYS_CACHE | IVI_VAL_HIDDEN,
	                                    VI_NULL, VI_NULL, VI_NULL, 0));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_PROG_PL,
	                                            chr63200AAttrProgDataValue_CheckCallback));
	checkErr (Ivi_AddAttributeViReal64 (vi, CHR63200A_ATTR_PROG_DELAY,
	                                    "CHR63200A_ATTR_PROG_DELAY", 0,
	                                    IVI_VAL_ALWAYS_CACHE | IVI_VAL_HIDDEN,
	                                    VI_NULL, VI_NULL, VI_NULL, 0));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_PROG_TRIG_MODE,
	                                   "CHR63200A_ATTR_PROG_TRIG_MODE", 0,
	                                   IVI_VAL_ALWAYS_CACHE | IVI_VAL_HIDDEN,
	                                   VI_NULL, VI_NULL, &attrProgTrigModeRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_PROG_RUN_MODE,
	                                   "CHR63200A_ATTR_PROG_RUN_MODE", 0,
	                                   IVI_VAL_ALWAYS_CACHE | IVI_VAL_HIDDEN,
	                                   VI_NULL, VI_NULL, &attrProgRunModeRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_PROG_MODE_RANGE,
	                                   "CHR63200A_ATTR_PROG_MODE_RANGE", 0,
	                                   IVI_VAL_ALWAYS_CACHE | IVI_VAL_HIDDEN,
	                                   VI_NULL, VI_NULL,
	                                   &attrProgModeRangeRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_ADV_BATT_RNG,
	                                   "CHR63200A_ATTR_ADV_BATT_RNG", 0, 0,
	                                   chr63200AAttrAdvBattRng_ReadCallback,
	                                   chr63200AAttrAdvBattRng_WriteCallback,
	                                   &attrConfVoltRangRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_CURR_SWE_RNG,
	                                   "CHR63200A_ATTR_CURR_SWE_RNG", 0, 0,
	                                   chr63200AAttrCurrSweRng_ReadCallback,
	                                   chr63200AAttrCurrSweRng_WriteCallback,
	                                   &attrConfVoltRangRangeTable));
	checkErr (Ivi_AddAttributeViInt32 (vi, CHR63200A_ATTR_ADV_SWD_RNG,
	                                   "CHR63200A_ATTR_ADV_SWD_RNG", 0, 0,
	                                   chr63200AAttrAdvSwdRng_ReadCallback,
	                                   chr63200AAttrAdvSwdRng_WriteCallback,
	                                   &attrConfVoltRangRangeTable));
	checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, CHR63200A_ATTR_PROG_DELAY,
	                                            chr63200AAttrProgDataValue_CheckCallback));
	checkErr (Ivi_SetAttrCheckCallbackViInt32 (vi,
	                                           CHR63200A_ATTR_PROG_SEQUENCE_INDEX,
	                                           chr63200AAttrProgSequenceIndex_CheckCallback));
                                       
        /*- Setup attribute invalidations -----------------------------------*/            


	checkErr (Ivi_AddAttributeInvalidation (vi, CHR63200A_ATTR_ADV_BATT_VAL,
	                                        CHR63200A_ATTR_ADV_BATT_MODE, VI_TRUE));
	checkErr (Ivi_AddAttributeInvalidation (vi, CHR63200A_ATTR_ADV_EXT_WAV_RNG,
	                                        CHR63200A_ATTR_ADV_EXT_WAV_MODE, VI_TRUE));
Error:
    return error;
}

/*****************************************************************************
 *------------------- End Instrument Driver Source Code ---------------------*
 *****************************************************************************/
