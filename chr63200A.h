/****************************************************************************
 *                       Chroma 63200A                           
 *                                                                          
 * Title:    chr63200A.h                                        
 * Purpose:  Chroma 63200A                                       
 *           instrument driver declarations.                                
 *                                                                          
 ****************************************************************************/

#ifndef __CHR63200A_HEADER
#define __CHR63200A_HEADER

#include <ivi.h>

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

/****************************************************************************
 *----------------- Instrument Driver Revision Information -----------------*
 ****************************************************************************/
#define CHR63200A_MAJOR_VERSION                1     /* Instrument driver major version   */
#define CHR63200A_MINOR_VERSION                0     /* Instrument driver minor version   */
                                                                
#define CHR63200A_CLASS_SPEC_MAJOR_VERSION     1     /* Class specification major version */
#define CHR63200A_CLASS_SPEC_MINOR_VERSION     0     /* Class specification minor version */


#define CHR63200A_SUPPORTED_INSTRUMENT_MODELS  "63205A-150-300"
#define CHR63200A_DRIVER_VENDOR                "Chroma"
#ifdef	_IVI_mswin64_
#define CHR63200A_DRIVER_DESCRIPTION           "Electronic Load"
#else                    
#define CHR63200A_DRIVER_DESCRIPTION           "Electronic Load"
#endif

/**************************************************************************** 
 *------------------------------ Useful Macros -----------------------------* 
 ****************************************************************************/


/**************************************************************************** 
 *---------------------------- Attribute Defines ---------------------------* 
 ****************************************************************************/

    /*- IVI Inherent Instrument Attributes ---------------------------------*/    

        /* User Options */
#define CHR63200A_ATTR_RANGE_CHECK                   IVI_ATTR_RANGE_CHECK                    /* ViBoolean */
#define CHR63200A_ATTR_QUERY_INSTRUMENT_STATUS       IVI_ATTR_QUERY_INSTRUMENT_STATUS        /* ViBoolean */
#define CHR63200A_ATTR_CACHE                         IVI_ATTR_CACHE                          /* ViBoolean */
#define CHR63200A_ATTR_SIMULATE                      IVI_ATTR_SIMULATE                       /* ViBoolean */
#define CHR63200A_ATTR_RECORD_COERCIONS              IVI_ATTR_RECORD_COERCIONS               /* ViBoolean */
#define CHR63200A_ATTR_INTERCHANGE_CHECK             IVI_ATTR_INTERCHANGE_CHECK              /* ViBoolean */
        
        /* Driver Information  */
#define CHR63200A_ATTR_SPECIFIC_DRIVER_PREFIX        IVI_ATTR_SPECIFIC_DRIVER_PREFIX         /* ViString, read-only  */
#define CHR63200A_ATTR_SUPPORTED_INSTRUMENT_MODELS   IVI_ATTR_SUPPORTED_INSTRUMENT_MODELS    /* ViString, read-only  */
#define CHR63200A_ATTR_GROUP_CAPABILITIES            IVI_ATTR_GROUP_CAPABILITIES             /* ViString, read-only  */
#define CHR63200A_ATTR_INSTRUMENT_MANUFACTURER       IVI_ATTR_INSTRUMENT_MANUFACTURER        /* ViString, read-only  */
#define CHR63200A_ATTR_INSTRUMENT_MODEL              IVI_ATTR_INSTRUMENT_MODEL               /* ViString, read-only  */
#define CHR63200A_ATTR_INSTRUMENT_FIRMWARE_REVISION  IVI_ATTR_INSTRUMENT_FIRMWARE_REVISION              /* ViString, read-only  */
#define CHR63200A_ATTR_SPECIFIC_DRIVER_REVISION      IVI_ATTR_SPECIFIC_DRIVER_REVISION       /* ViString, read-only  */
#define CHR63200A_ATTR_SPECIFIC_DRIVER_VENDOR        IVI_ATTR_SPECIFIC_DRIVER_VENDOR         /* ViString, read-only  */
#define CHR63200A_ATTR_SPECIFIC_DRIVER_DESCRIPTION   IVI_ATTR_SPECIFIC_DRIVER_DESCRIPTION    /* ViString, read-only  */
#define CHR63200A_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION IVI_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION /* ViInt32, read-only */
#define CHR63200A_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION IVI_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION /* ViInt32, read-only */

        /* Advanced Session Information */
#define CHR63200A_ATTR_LOGICAL_NAME                  IVI_ATTR_LOGICAL_NAME                   	/* ViString, read-only  */
#define CHR63200A_ATTR_IO_RESOURCE_DESCRIPTOR        IVI_ATTR_IO_RESOURCE_DESCRIPTOR         	/* ViString, read-only  */
#define CHR63200A_ATTR_DRIVER_SETUP                  IVI_ATTR_DRIVER_SETUP                   	/* ViString, read-only  */        
    /*- Instrument-Specific Attributes -------------------------------------*/

#define CHR63200A_ATTR_ID_QUERY_RESPONSE      		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 1L)     	/* ViString, read-only */
#define CHR63200A_ATTR_ADV_AUTO_ISET        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 2L)	 	/* ViReal64 */
#define CHR63200A_ATTR_ADV_AUTO_PSET        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 3L)	 	/* ViReal64 */ 
#define CHR63200A_ATTR_ADV_AUTO_RSET        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 4L)	 	/* ViReal64 */ 
#define CHR63200A_ATTR_ADV_AUTO_VSET        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 6L)	 	/* ViReal64 */

#define CHR63200A_ATTR_ADV_BATT_ENDV        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 7L)	 	/* ViReal64 */
#define CHR63200A_ATTR_ADV_BATT_FALL        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 8L)		/* ViReal64 */
#define CHR63200A_ATTR_ADV_BATT_MODE        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 9L)		/* ViInt32 */
#define CHR63200A_ATTR_ADV_BATT_RISE        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 10L)		/* ViReal64 */
#define CHR63200A_ATTR_ADV_BATT_TOUT        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 12L)		/* ViReal64 */
#define CHR63200A_ATTR_ADV_BATT_VAL         		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 13L)		/* ViReal64 */

#define CHR63200A_ATTR_ADV_CRCC_ISET        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 14L)		/* ViReal64 */
#define CHR63200A_ATTR_ADV_CRCC_RSET        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 15L)		/* ViReal64 */
#define CHR63200A_ATTR_ADV_CVCC_ISET        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 17L)		/* ViReal64 */
#define CHR63200A_ATTR_ADV_CVCC_RES         		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 18L)		/* ViInt32 */
#define CHR63200A_ATTR_ADV_CVCC_VSET        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 20L)		/* ViReal64 */
#define CHR63200A_ATTR_ADV_CVCR_RSET        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 22L)		/* ViReal64 */
#define CHR63200A_ATTR_ADV_CVCR_VSET        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 24L)		/* ViReal64 */

#define CHR63200A_ATTR_ADV_EXT_WAV_MODE     		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 25L)		/* ViInt32 */
#define CHR63200A_ATTR_ADV_EXT_WAV_RNG      		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 26L)		/* ViInt32 */

#define CHR63200A_ATTR_ADV_OCP_DWEL         		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 27L)		/* ViReal64 */
#define CHR63200A_ATTR_ADV_OCP_END          		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 28L)		/* ViReal64 */
#define CHR63200A_ATTR_ADV_OCP_LATC         		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 29L)		/* ViInt32 */
#define CHR63200A_ATTR_ADV_OCP_RES          		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 30L)		/* ViString, read-only */
#define CHR63200A_ATTR_ADV_OCP_SPEC_H       		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 31L)		/* ViReal64 */
#define CHR63200A_ATTR_ADV_OCP_SPEC_L       		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 32L)		/* ViReal64 */
#define CHR63200A_ATTR_ADV_OCP_STAR         		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 33L)		/* ViReal64 */
#define CHR63200A_ATTR_ADV_OCP_STEP         		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 34L)		/* ViInt32 */
#define CHR63200A_ATTR_ADV_OCP_TRIG_VOLT    		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 35L)		/* ViReal64 */

#define CHR63200A_ATTR_ADV_OPP_DWEL         		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 36L)		/* ViReal64 */
#define CHR63200A_ATTR_ADV_OPP_END          		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 37L)		/* ViReal64 */
#define CHR63200A_ATTR_ADV_OPP_LATC         		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 38L)		/* ViInt32 */
#define CHR63200A_ATTR_ADV_OPP_RES          		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 39L)		/* ViString, read-only */
#define CHR63200A_ATTR_ADV_OPP_SPEC_H       		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 40L)		/* ViReal64 */
#define CHR63200A_ATTR_ADV_OPP_SPEC_L       		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 41L)		/* ViReal64 */
#define CHR63200A_ATTR_ADV_OPP_STAR         		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 42L)		/* ViReal64 */
#define CHR63200A_ATTR_ADV_OPP_STEP         		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 43L)		/* ViInt32 */
#define CHR63200A_ATTR_ADV_OPP_TRIG_VOLT    		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 44L)		/* ViReal64 */

#define CHR63200A_ATTR_ADV_SINE_FREQ        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 45L)		/* ViReal64 */
#define CHR63200A_ATTR_ADV_SINE_IAC         		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 46L)		/* ViReal64 */
#define CHR63200A_ATTR_ADV_SINE_IDC         		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 47L)		/* ViReal64 */

#define CHR63200A_ATTR_ADV_USER_WAV_CLE     		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 57L)		/* ViInt32 */
#define CHR63200A_ATTR_ADV_USER_WAV_DATA    		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 58L)		/* ViString */
#define CHR63200A_ATTR_ADV_USER_WAV_DATA_POIN	 	(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 59L)		/* ViString */
#define CHR63200A_ATTR_ADV_USER_WAV_DATA_STAT	 	(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 60L)		/* ViInt32 */
#define CHR63200A_ATTR_ADV_USER_WAV_EXE_STAT 		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 61L)		/* ViInt32 */
#define CHR63200A_ATTR_ADV_USER_WAV_NSEL    		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 62L)		/* ViInt32 */
#define CHR63200A_ATTR_ADV_USER_WAV_REM     		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 63L)		/* ViInt32 */

#define CHR63200A_ATTR_CONF_AUTO_ON         		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 64L)		/* ViInt32 */
#define CHR63200A_ATTR_CONF_DIO_IN1         		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 65L)		/* ViInt32 */
#define CHR63200A_ATTR_CONF_DIO_IN2         		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 66L)		/* ViInt32 */
#define CHR63200A_ATTR_CONF_DIO_OUT1        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 67L)		/* ViInt32 */
#define CHR63200A_ATTR_CONF_DIO_OUT2        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 68L)		/* ViInt32 */
#define CHR63200A_ATTR_CONF_ENT_KEY         		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 69L)		/* ViInt32 */
#define CHR63200A_ATTR_CONF_PARA_INIT       		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 70L)		/* ViInt32 */
#define CHR63200A_ATTR_CONF_PARA_MODE       		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 71L)		/* ViInt32 */
#define CHR63200A_ATTR_CONF_SHOR_KEY        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 72L)		/* ViInt32 */
#define CHR63200A_ATTR_CONF_SOUN            		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 73L)		/* ViInt32 */
#define CHR63200A_ATTR_CONF_SYNC_MODE       		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 74L)		/* ViInt32 */
#define CHR63200A_ATTR_CONF_VOLT_LATC       		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 75L)		/* ViInt32 */
#define CHR63200A_ATTR_CONF_VOLT_OFF        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 77L)		/* ViReal64 */
#define CHR63200A_ATTR_CONF_VOLT_ON         		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 78L)		/* ViReal64 */
#define CHR63200A_ATTR_CONF_VOLT_SIGN       		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 80L)		/* ViInt32 */
#define CHR63200A_ATTR_CONF_WIND            		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 81L)		/* ViReal64 */

#define CHR63200A_ATTR_CURR_DYN_FALL        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 82L)		/* ViReal64 */
#define CHR63200A_ATTR_CURR_DYN_L1          		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 83L)		/* ViReal64 */
#define CHR63200A_ATTR_CURR_DYN_L2          		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 84L)		/* ViReal64 */
#define CHR63200A_ATTR_CURR_DYN_REP         		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 85L)		/* ViInt32 */
#define CHR63200A_ATTR_CURR_DYN_RISE        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 86L)		/* ViReal64 */
#define CHR63200A_ATTR_CURR_DYN_T1          		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 87L)		/* ViReal64 */
#define CHR63200A_ATTR_CURR_DYN_T2          		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 88L)		/* ViReal64 */
#define CHR63200A_ATTR_CURR_DYN_VRNG        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 89L)		/* ViInt32 */

#define CHR63200A_ATTR_CURR_STAT_FALL       		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 90L)		/* ViReal64 */
#define CHR63200A_ATTR_CURR_STAT_L1         		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 91L)		/* ViReal64 */
#define CHR63200A_ATTR_CURR_STAT_L2         		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 92L)		/* ViReal64 */
#define CHR63200A_ATTR_CURR_STAT_RISE       		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 93L)		/* ViReal64 */
#define CHR63200A_ATTR_CURR_STAT_VRNG       		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 94L)		/* ViInt32 */

#define CHR63200A_ATTR_CURR_SWE_DUTY        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 95L)		/* ViReal64 */
#define CHR63200A_ATTR_CURR_SWE_DWEL        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 96L)		/* ViReal64 */
#define CHR63200A_ATTR_CURR_SWE_FALL        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 97L)		/* ViReal64 */
#define CHR63200A_ATTR_CURR_SWE_FEND        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 98L)		/* ViReal64 */
#define CHR63200A_ATTR_CURR_SWE_FSTA        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 99L)		/* ViReal64 */
#define CHR63200A_ATTR_CURR_SWE_FSTEP       		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 100L)		/* ViReal64 */
#define CHR63200A_ATTR_CURR_SWE_IMAX        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 101L)		/* ViReal64 */
#define CHR63200A_ATTR_CURR_SWE_IMIN        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 102L)		/* ViReal64 */
#define CHR63200A_ATTR_CURR_SWE_RISE        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 103L)		/* ViReal64 */

#define CHR63200A_ATTR_DIG_SAMP_POIN        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 106L)		/* ViInt32 */
#define CHR63200A_ATTR_DIG_SAMP_TIME        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 107L)		/* ViReal64 */
#define CHR63200A_ATTR_DIG_TRIG             		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 108L)		/* ViInt32 */
#define CHR63200A_ATTR_DIG_TRIG_POIN        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 109L)		/* ViInt32 */
#define CHR63200A_ATTR_DIG_TRIG_SOUR        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 110L)		/* ViInt32 */
#define CHR63200A_ATTR_DIG_WAV_CAP          		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 111L)		/* ViString, read-only */

#define CHR63200A_ATTR_DIO_OUT1             		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 113L)		/* ViInt32 */
#define CHR63200A_ATTR_DIO_OUT2             		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 114L)		/* ViInt32 */

#define CHR63200A_ATTR_IMP_STAT_CL          		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 132L)		/* ViReal64 */
#define CHR63200A_ATTR_IMP_STAT_LS          		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 133L)		/* ViReal64 */
#define CHR63200A_ATTR_IMP_STAT_RL          		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 134L)		/* ViReal64 */
#define CHR63200A_ATTR_IMP_STAT_RS          		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 135L)		/* ViReal64 */

#define CHR63200A_ATTR_LOAD                 		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 136L)		/* ViInt32 */
#define CHR63200A_ATTR_LOAD_PROT            		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 137L)		/* ViInt32 */
#define CHR63200A_ATTR_LOAD_PROT_CLE        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 138L)		/* ViInt32 */
#define CHR63200A_ATTR_LOAD_SHOR            		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 139L)		/* ViInt32 */

#define CHR63200A_ATTR_MODE                 		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 145L)		/* ViInt32 */

#define CHR63200A_ATTR_POW_STAT_FALL        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 146L)		/* ViReal64 */
#define CHR63200A_ATTR_POW_STAT_L1          		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 147L)		/* ViReal64 */
#define CHR63200A_ATTR_POW_STAT_L2          		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 148L)		/* ViReal64 */
#define CHR63200A_ATTR_POW_STAT_RISE        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 149L)		/* ViReal64 */
#define CHR63200A_ATTR_POW_STAT_VRNG        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 150L)		/* ViInt32 */

#define CHR63200A_ATTR_RES_DYN_FALL         		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 161L)		/* ViReal64 */
#define CHR63200A_ATTR_RES_DYN_IRNG         		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 162L)		/* ViInt32 */
#define CHR63200A_ATTR_RES_DYN_L1           		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 163L)		/* ViReal64 */
#define CHR63200A_ATTR_RES_DYN_L2           		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 164L)		/* ViReal64 */
#define CHR63200A_ATTR_RES_DYN_REP          		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 165L)		/* ViInt32 */
#define CHR63200A_ATTR_RES_DYN_RISE         		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 166L)		/* ViReal64 */
#define CHR63200A_ATTR_RES_DYN_T1           		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 167L)		/* ViReal64 */
#define CHR63200A_ATTR_RES_DYN_T2           		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 168L)		/* ViReal64 */

#define CHR63200A_ATTR_RES_STAT_FALL        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 169L)		/* ViReal64 */
#define CHR63200A_ATTR_RES_STAT_IRNG        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 170L)		/* ViInt32 */
#define CHR63200A_ATTR_RES_STAT_L1          		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 171L)		/* ViReal64 */
#define CHR63200A_ATTR_RES_STAT_L2          		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 172L)		/* ViReal64 */
#define CHR63200A_ATTR_RES_STAT_RISE        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 173L)		/* ViReal64 */

#define CHR63200A_ATTR_SPEC_CURR_C          		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 178L)		/* ViReal64 */
#define CHR63200A_ATTR_SPEC_CURR_H          		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 179L)		/* ViReal64 */
#define CHR63200A_ATTR_SPEC_CURR_L          		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 180L)		/* ViReal64 */
#define CHR63200A_ATTR_SPEC_POW_C           		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 181L)		/* ViReal64 */
#define CHR63200A_ATTR_SPEC_POW_H           		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 182L)		/* ViReal64 */
#define CHR63200A_ATTR_SPEC_POW_L           		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 183L)		/* ViReal64 */
#define CHR63200A_ATTR_SPEC_TEST            		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 184L)		/* ViReal64 */
#define CHR63200A_ATTR_SPEC_VOLT_C          		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 186L)		/* ViReal64 */
#define CHR63200A_ATTR_SPEC_VOLT_H          		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 187L)		/* ViReal64 */
#define CHR63200A_ATTR_SPEC_VOLT_L          		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 188L)		/* ViReal64 */


#define CHR63200A_ATTR_SYNC_RUN             		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 202L)		/* ViInt32 */
#define CHR63200A_ATTR_SYNC_TYPE            		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 203L)		/* ViInt32 */

#define CHR63200A_ATTR_VOLT_STAT_ILIM       		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 206L)		/* ViReal64 */
#define CHR63200A_ATTR_VOLT_STAT_IRNG       		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 207L)		/* ViInt32 */
#define CHR63200A_ATTR_VOLT_STAT_L1         		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 208L)		/* ViReal64 */
#define CHR63200A_ATTR_VOLT_STAT_L2         		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 209L)		/* ViReal64 */
#define CHR63200A_ATTR_VOLT_STAT_RES        		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 210L)		/* ViInt32 */

#define CHR63200A_ATTR_SPEC_UNIT            		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 211L)		/* ViInt32 */

#define CHR63200A_ATTR_PROG_NUM             		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 212L)		/* ViInt32 */

#define CHR63200A_ATTR_ADV_BATT_RNG         		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 213L)		/* ViInt32 */
#define CHR63200A_ATTR_CURR_SWE_RNG         		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 214L)		/* ViInt32 */
#define CHR63200A_ATTR_ADV_SWD_RNG          		(IVI_SPECIFIC_PUBLIC_ATTR_BASE + 215L)		/* ViInt32 */


    /*- Obsolete Inherent Instrument Attributes ----------------------------*/

        /* Driver Information */
#define CHR63200A_ATTR_SPECIFIC_DRIVER_MAJOR_VERSION IVI_ATTR_SPECIFIC_DRIVER_MAJOR_VERSION  /* ViInt32,  read-only  */
#define CHR63200A_ATTR_SPECIFIC_DRIVER_MINOR_VERSION IVI_ATTR_SPECIFIC_DRIVER_MINOR_VERSION  /* ViInt32,  read-only  */

        /* Error Info */
#define CHR63200A_ATTR_PRIMARY_ERROR                 IVI_ATTR_PRIMARY_ERROR                  /* ViInt32 */
#define CHR63200A_ATTR_SECONDARY_ERROR               IVI_ATTR_SECONDARY_ERROR                /* ViInt32 */
#define CHR63200A_ATTR_ERROR_ELABORATION             IVI_ATTR_ERROR_ELABORATION              /* ViString  */

     /* Obsolete Attribute Identifiers for Renamed Attributes */
#define CHR63200A_ATTR_NUM_CHANNELS             IVI_ATTR_CHANNEL_COUNT
#define CHR63200A_ATTR_QUERY_INSTR_STATUS       CHR63200A_ATTR_QUERY_INSTRUMENT_STATUS            
#define CHR63200A_ATTR_RESOURCE_DESCRIPTOR      CHR63200A_ATTR_IO_RESOURCE_DESCRIPTOR


/**************************************************************************** 
 *------------------------ Attribute Value Defines -------------------------* 
 ****************************************************************************/

        /* Instrument specific attribute value definitions */

#define CHR63200A_VAL_MOD_CCL 										1
#define CHR63200A_VAL_MOD_CCM 										2
#define CHR63200A_VAL_MOD_CCH 										3
#define CHR63200A_VAL_MOD_CRL 										4
#define CHR63200A_VAL_MOD_CRM 										5
#define CHR63200A_VAL_MOD_CRH 										6
#define CHR63200A_VAL_MOD_CVL 										7
#define CHR63200A_VAL_MOD_CVM 										8
#define CHR63200A_VAL_MOD_CVH 										9
#define CHR63200A_VAL_MOD_CPL 										10
#define CHR63200A_VAL_MOD_CPM 										11
#define CHR63200A_VAL_MOD_CPH 										12
#define CHR63200A_VAL_MOD_CCDL 										13
#define CHR63200A_VAL_MOD_CCDM 										14
#define CHR63200A_VAL_MOD_CCDH 										15
#define CHR63200A_VAL_MOD_CRDL 										16
#define CHR63200A_VAL_MOD_CRDM 										17
#define CHR63200A_VAL_MOD_CRDH 										18
#define CHR63200A_VAL_MOD_BATL 										19
#define CHR63200A_VAL_MOD_BATM 										20
#define CHR63200A_VAL_MOD_BATH 										21
#define CHR63200A_VAL_MOD_SWDL 										22
#define CHR63200A_VAL_MOD_SWDM 										23
#define CHR63200A_VAL_MOD_SWDH 										24
#define CHR63200A_VAL_MOD_OCPL 										25
#define CHR63200A_VAL_MOD_OCPM 										26
#define CHR63200A_VAL_MOD_OCPH 										27
#define CHR63200A_VAL_MOD_CCSL 										28
#define CHR63200A_VAL_MOD_CCSM 										29
#define CHR63200A_VAL_MOD_CCSH 										30
#define CHR63200A_VAL_MOD_CZL 										31
#define CHR63200A_VAL_MOD_CZM 										32
#define CHR63200A_VAL_MOD_CZH 										33
#define CHR63200A_VAL_MOD_CVCC 										34
#define CHR63200A_VAL_MOD_CRCC 										35
#define CHR63200A_VAL_MOD_CVCR 										36
#define CHR63200A_VAL_MOD_AUTO 										37
#define CHR63200A_VAL_MOD_PROG 										38
#define CHR63200A_VAL_MOD_MPPTL 									39
#define CHR63200A_VAL_MOD_MPPTM 									40
#define CHR63200A_VAL_MOD_MPPTH 									41
#define CHR63200A_VAL_MOD_UDWL 										42
#define CHR63200A_VAL_MOD_UDWM 										43
#define CHR63200A_VAL_MOD_UDWH 										44
#define CHR63200A_VAL_MOD_EXTL 										45
#define CHR63200A_VAL_MOD_EXTM 										46
#define CHR63200A_VAL_MOD_EXTH 										47

#define CHR63200A_VAL_OFF                                           0
#define CHR63200A_VAL_ON                                            1

#define CHR63200A_VAL_HOLD                                          0
#define CHR63200A_VAL_TOGGLE                                        1
#define CHR63200A_VAL_DISABLE                                       2

#define CHR63200A_VAL_LOW                                           0
#define CHR63200A_VAL_MIDDLE                                        1
#define CHR63200A_VAL_HIGH                                          2

#define CHR63200A_VAL_PLUS                                          0
#define CHR63200A_VAL_MINUS                                         1

#define CHR63200A_VAL_NONE                                          0
#define CHR63200A_VAL_MASTER                                        1
#define CHR63200A_VAL_SLAVE                                         2

#define CHR63200A_VAL_NEXT                                          0
#define CHR63200A_VAL_FIXED                                         1

#define CHR63200A_VAL_EXT                                           1
#define CHR63200A_VAL_REM                                           2

#define CHR63200A_VAL_SLOW                                          0
#define CHR63200A_VAL_NORMAL                                        1
#define CHR63200A_VAL_FAST                                          2

#define CHR63200A_VAL_CZ_CL                                       	0
#define CHR63200A_VAL_CZ_RL                                       	1
#define CHR63200A_VAL_CZ_LS                                       	2	
#define CHR63200A_VAL_CZ_RS                                       	3	
	
#define CHR63200A_VAL_EXTM_CC                                       0
#define CHR63200A_VAL_EXTM_CR                                       1
#define CHR63200A_VAL_EXTM_CV                                       2

#define CHR63200A_VAL_DIGI_TRIG_OFF                                 0
#define CHR63200A_VAL_DIGI_TRIG_ON                                  1
#define CHR63200A_VAL_DIGI_TRIG_IDLE                                101
#define CHR63200A_VAL_DIGI_TRIG_PRE_TRIG                            102
#define CHR63200A_VAL_DIGI_TRIG_WAIT_TRIG                           103
#define CHR63200A_VAL_DIGI_TRIG_POST_TRIG                           104

#define CHR63200A_VAL_DIGI_TRIG_SOUR_LOADON                         0
#define CHR63200A_VAL_DIGI_TRIG_SOUR_LOADOFF                        1
#define CHR63200A_VAL_DIGI_TRIG_SOUR_TTL                            2
#define CHR63200A_VAL_DIGI_TRIG_SOUR_BUS                            3
#define CHR63200A_VAL_DIGI_TRIG_SOUR_MANUAL                         4

#define CHR63200A_VAL_SPEC_PERCENT                                  0
#define CHR63200A_VAL_SPEC_VAL                                      1

#define CHR63200A_VAL_ADV_PROG_TYPE_STEP                            1
#define CHR63200A_VAL_ADV_PROG_TYPE_LIST                            0

#define CHR63200A_VAL_ADV_PROG_SKIP                                 0
#define CHR63200A_VAL_ADV_PROG_AUTO                                 1
#define CHR63200A_VAL_ADV_PROG_MANUAL                               2
#define CHR63200A_VAL_ADV_PROG_EXTERNAL                             3

#define CHR63200A_VAL_ADV_PROG_CC                                   0
#define CHR63200A_VAL_ADV_PROG_CR                                   1
#define CHR63200A_VAL_ADV_PROG_CV                                   2
#define CHR63200A_VAL_ADV_PROG_CP                                   3

#define CHR63200A_VAL_UDWD_WAVEFORM                                 0
#define CHR63200A_VAL_UDWD_INTERVAL                                 1.000000
#define CHR63200A_VAL_UDWD_REPEAT                                   2.000000
#define CHR63200A_VAL_UDWD_CHAIN                                    3.000000
#define CHR63200A_VAL_UDWD_INTERPOLATION                            4.000000

#define CHR63200A_VAL_UDW_STAT_IDLE                                 0
#define CHR63200A_VAL_UDW_STAT_WAIT                                 1
#define CHR63200A_VAL_UDW_STAT_FIN                                  2
#define CHR63200A_VAL_UDW_STAT_FORMAT                               3
#define CHR63200A_VAL_UDW_STAT_LEN                                  4
#define CHR63200A_VAL_UDW_STAT_LIM                                  5
#define CHR63200A_VAL_UDW_STAT_CHKSUM                               6

#define CHR63200A_VAL_UDW_EXE_IDLE                                  0
#define CHR63200A_VAL_UDW_EXE_RUN                                   1
#define CHR63200A_VAL_UDW_EXE_FIN                                   2
#define CHR63200A_VAL_UDW_EXE_STOP                                  3

#define CHR63200A_VAL_BATT_CC                                       0
#define CHR63200A_VAL_BATT_CR                                       1
#define CHR63200A_VAL_BATT_CP                                       2

#define CHR63200A_VAL_WAV_CLE_OK                                    0
#define CHR63200A_VAL_WAV_CLE_ERR                                   1

#define CHR63200A_VAL_DIGI_TRIG_FINISH_TRIG                         105


	
/**************************************************************************** 
 *---------------- Instrument Driver Function Declarations -----------------* 
 ****************************************************************************/

    /*- Init and Close Functions -------------------------------------------*/
ViStatus _VI_FUNC  chr63200A_init (ViRsrc resourceName, ViBoolean IDQuery,ViBoolean resetDevice, ViSession *vi);
ViStatus _VI_FUNC  chr63200A_InitWithOptions (ViRsrc resourceName, ViBoolean IDQuery,ViBoolean resetDevice, ViString optionString, ViSession *newVi);
ViStatus _VI_FUNC  chr63200A_close (ViSession vi);   

    /*- Coercion Info Functions --------------------------------------------*/
ViStatus _VI_FUNC  chr63200A_GetNextCoercionRecord (ViSession vi,ViInt32 bufferSize,ViChar record[]);
    
    /*- Locking Functions --------------------------------------------------*/
ViStatus _VI_FUNC  chr63200A_LockSession (ViSession vi, ViBoolean *callerHasLock);   
ViStatus _VI_FUNC  chr63200A_UnlockSession (ViSession vi, ViBoolean *callerHasLock);


    /*- Error Functions ----------------------------------------------------*/
ViStatus _VI_FUNC  chr63200A_error_query (ViSession vi, ViInt32 *errorCode,ViChar errorMessage[]);

ViStatus _VI_FUNC  chr63200A_GetError (ViSession vi, ViStatus *code, ViInt32 bufferSize, ViChar description[]);
ViStatus _VI_FUNC  chr63200A_ClearError (ViSession vi);

ViStatus _VI_FUNC  chr63200A_error_message (ViSession vi, ViStatus errorCode,ViChar errorMessage[256]);
    
    /*- Obsolete Error Handling Functions ----------------------------*/
ViStatus _VI_FUNC  chr63200A_GetErrorInfo (ViSession vi, ViStatus *primaryError, ViStatus *secondaryError, ViChar errorElaboration[256]);
ViStatus _VI_FUNC  chr63200A_ClearErrorInfo (ViSession vi);

    /*- Interchangeability Checking Functions ------------------------------*/
ViStatus _VI_FUNC chr63200A_GetNextInterchangeWarning (ViSession vi, ViInt32 bufferSize, ViChar warnString[]);
ViStatus _VI_FUNC chr63200A_ResetInterchangeCheck (ViSession vi);
ViStatus _VI_FUNC chr63200A_ClearInterchangeWarnings (ViSession vi);

    /*- Utility Functions --------------------------------------------------*/
ViStatus _VI_FUNC chr63200A_InvalidateAllAttributes (ViSession vi);
ViStatus _VI_FUNC  chr63200A_reset (ViSession vi);
ViStatus _VI_FUNC  chr63200A_ResetWithDefaults (ViSession vi);
ViStatus _VI_FUNC  chr63200A_self_test (ViSession vi, ViInt16 *selfTestResult,ViChar selfTestMessage[]);
ViStatus _VI_FUNC  chr63200A_revision_query (ViSession vi, ViChar instrumentDriverRevision[],ViChar firmwareRevision[]);
ViStatus _VI_FUNC  chr63200A_Disable (ViSession vi);
ViStatus _VI_FUNC  chr63200A_WriteInstrData (ViSession vi, ViConstString writeBuffer); 
ViStatus _VI_FUNC  chr63200A_ReadInstrData  (ViSession vi, ViInt32 numBytes, ViChar rdBuf[], ViInt32 *bytesRead);

    /*- Set, Get, and Check Attribute Functions ----------------------------*/
ViStatus _VI_FUNC  chr63200A_GetAttributeViInt32 (ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 *value);
ViStatus _VI_FUNC  chr63200A_GetAttributeViReal64 (ViSession vi, ViConstString channelName, ViAttr attribute, ViReal64 *value);
ViStatus _VI_FUNC  chr63200A_GetAttributeViString (ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 bufSize, ViChar value[]); 
ViStatus _VI_FUNC  chr63200A_GetAttributeViSession (ViSession vi, ViConstString channelName, ViAttr attribute, ViSession *value);
ViStatus _VI_FUNC  chr63200A_GetAttributeViBoolean (ViSession vi, ViConstString channelName, ViAttr attribute, ViBoolean *value);

ViStatus _VI_FUNC  chr63200A_SetAttributeViInt32 (ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 value);
ViStatus _VI_FUNC  chr63200A_SetAttributeViReal64 (ViSession vi, ViConstString channelName, ViAttr attribute, ViReal64 value);
ViStatus _VI_FUNC  chr63200A_SetAttributeViString (ViSession vi, ViConstString channelName, ViAttr attribute, ViConstString value); 
ViStatus _VI_FUNC  chr63200A_SetAttributeViSession (ViSession vi, ViConstString channelName, ViAttr attribute, ViSession value);
ViStatus _VI_FUNC  chr63200A_SetAttributeViBoolean (ViSession vi, ViConstString channelName, ViAttr attribute, ViBoolean value);

ViStatus _VI_FUNC  chr63200A_CheckAttributeViInt32 (ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 value);
ViStatus _VI_FUNC  chr63200A_CheckAttributeViReal64 (ViSession vi, ViConstString channelName, ViAttr attribute, ViReal64 value);
ViStatus _VI_FUNC  chr63200A_CheckAttributeViString (ViSession vi, ViConstString channelName, ViAttr attribute, ViConstString value); 
ViStatus _VI_FUNC  chr63200A_CheckAttributeViSession (ViSession vi, ViConstString channelName, ViAttr attribute, ViSession value);
ViStatus _VI_FUNC  chr63200A_CheckAttributeViBoolean (ViSession vi, ViConstString channelName, ViAttr attribute, ViBoolean value);


ViStatus _VI_FUNC  chr63200A_Abort (ViSession vi);
ViStatus _VI_FUNC  chr63200A_Fetch (ViSession vi, ViInt32 item,ViReal64 *result);
ViStatus _VI_FUNC  chr63200A_ElectronicLoadState (ViSession vi,ViBoolean trigger, ViBoolean shortState,ViBoolean loadStatus[], ViBoolean clear);
ViStatus _VI_FUNC  chr63200A_SynchronousRun (ViSession vi, ViBoolean run);
ViStatus _VI_FUNC  chr63200A_Measure (ViSession vi, ViInt32 inputPort,ViInt32 item, ViReal64 *response);
ViStatus _VI_FUNC  chr63200A_ProtectionTestResult (ViSession vi,ViInt32 *testResult, ViReal64 *testValue,ViReal64 *maximumPower,ViInt32 item);
ViStatus _VI_FUNC  chr63200A_OperatingMode (ViSession vi, ViInt32 mode, ViInt32 level);
ViStatus _VI_FUNC  chr63200A_SpecificationJudgment (ViSession vi,ViInt32 item, ViChar testResult[]);

ViStatus _VI_FUNC  chr63200A_DigitizingInitiate (ViSession vi);
ViStatus _VI_FUNC  chr63200A_DigitizingAbort (ViSession vi);	 
ViStatus _VI_FUNC  chr63200A_DigitizingSamplingTime (ViSession vi, ViReal64 time); 
ViStatus _VI_FUNC  chr63200A_DigitizingSamplingPoint (ViSession vi,  ViInt32 point);  
ViStatus _VI_FUNC  chr63200A_DigitizingTrigger (ViSession vi, ViInt32 state);
ViStatus _VI_FUNC  chr63200A_DigitizingTriggerPoint (ViSession vi, ViInt32 point);
ViStatus _VI_FUNC  chr63200A_DigitizingTriggerSource (ViSession vi, ViInt32 source);
ViStatus _VI_FUNC  chr63200A_DigitizingTriggerStatus (ViSession vi, ViInt32 *status);
ViStatus _VI_FUNC  chr63200A_DigitizingWaveform (ViSession vi,ViReal64 dataResponse[],ViInt32 returnItem);

ViStatus _VI_FUNC  chr63200A_ConfigureCCModeMeasureRange (ViSession vi,ViInt32 measureRange);
ViStatus _VI_FUNC  chr63200A_ConfigureCCModeLoadValue (ViSession vi,ViReal64 loadValue);
ViStatus _VI_FUNC  chr63200A_ConfigureCCModeSlewRate (ViSession vi,ViReal64 fallingSlewRate,ViReal64 risingSlewRate);

ViStatus _VI_FUNC  chr63200A_ConfigureCRModeMeasureRange (ViSession vi,ViInt32 measureRange);
ViStatus _VI_FUNC  chr63200A_ConfigureCRModeLoadValue (ViSession vi,ViReal64 loadValue);
ViStatus _VI_FUNC  chr63200A_ConfigureCRModeSlewRate (ViSession vi,ViReal64 fallingSlewRate,ViReal64 risingSlewRate);

ViStatus _VI_FUNC  chr63200A_ConfigureCVModeMeasureRange (ViSession vi,ViInt32 measureRange);
ViStatus _VI_FUNC  chr63200A_ConfigureCVModeLoadValue (ViSession vi,ViReal64 loadValue);
ViStatus _VI_FUNC  chr63200A_ConfigureCVModeCurrentLimit (ViSession vi,ViReal64 currentLimit);
ViStatus _VI_FUNC  chr63200A_ConfigureCVModeResponseSpeed (ViSession vi,ViInt32 responseSpeed);

ViStatus _VI_FUNC  chr63200A_ConfigureCPModeMeasureRange (ViSession vi,ViInt32 measureRange);
ViStatus _VI_FUNC  chr63200A_ConfigureCPModeLoadValue (ViSession vi,ViReal64 loadValue);
ViStatus _VI_FUNC  chr63200A_ConfigureCPModeSlewRate (ViSession vi,ViReal64 fallingSlewRate,ViReal64 risingSlewRate);
							  
ViStatus _VI_FUNC  chr63200A_ConfigureCCDModeMeasureRange (ViSession vi,ViInt32 measureRange);
ViStatus _VI_FUNC  chr63200A_ConfigureCCDModeRepeat (ViSession vi,ViInt32 repeat);
ViStatus _VI_FUNC  chr63200A_ConfigureCCDModeSlewRate (ViSession vi,ViReal64 fallingSlewRate,ViReal64 risingSlewRate);
ViStatus _VI_FUNC  chr63200A_ConfigureCCDModeLoadValue (ViSession vi,ViReal64 l1, ViReal64 l2);
ViStatus _VI_FUNC  chr63200A_ConfigureCCDModeTime (ViSession vi, ViReal64 t1,ViReal64 t2);

ViStatus _VI_FUNC  chr63200A_ConfigureCRDModeMeasureRange (ViSession vi,ViInt32 measureRange);
ViStatus _VI_FUNC  chr63200A_ConfigureCRDModeRepeat (ViSession vi,ViInt32 repeat);
ViStatus _VI_FUNC  chr63200A_ConfigureCRDModeSlewRate (ViSession vi,ViReal64 fallingSlewRate,ViReal64 risingSlewRate);
ViStatus _VI_FUNC  chr63200A_ConfigureCRDModeLoadValue (ViSession vi,ViReal64 l1, ViReal64 l2);
ViStatus _VI_FUNC  chr63200A_ConfigureCRDModeTime (ViSession vi, ViReal64 t1,ViReal64 t2);

ViStatus _VI_FUNC  chr63200A_ConfigureAutoModeVoltage (ViSession vi,ViReal64 voltage);
ViStatus _VI_FUNC  chr63200A_ConfigureAutoModeResistance (ViSession vi,ViReal64 resistance);
ViStatus _VI_FUNC  chr63200A_ConfigureAutoModeCurrent (ViSession vi,ViReal64 current);
ViStatus _VI_FUNC  chr63200A_ConfigureAutoModePower (ViSession vi,ViReal64 power);

ViStatus _VI_FUNC  chr63200A_ConfigureBatteryModeRunMode (ViSession vi,ViInt32 mode);
ViStatus _VI_FUNC  chr63200A_ConfigureBatteryModeLoadValue (ViSession vi,ViReal64 loadValue);
ViStatus _VI_FUNC  chr63200A_ConfigureBatteryModeSlewRate (ViSession vi,ViReal64 fallingSlewRate,ViReal64 risingSlewRate);
ViStatus _VI_FUNC  chr63200A_ConfigureBatteryModeEndVoltage (ViSession vi,ViReal64 endVoltage);
ViStatus _VI_FUNC  chr63200A_ConfigureBatteryModeTimeout (ViSession vi,ViReal64 timeout);
ViStatus _VI_FUNC  chr63200A_ConfigureBatteryModeMeasureRange (ViSession vi,ViInt32 measureRange);

ViStatus _VI_FUNC  chr63200A_SelectProgram (ViSession vi,ViInt32 programNumber);
ViStatus _VI_FUNC  chr63200A_ConfigureProgramType (ViSession vi,ViInt32 type, ViInt32 chain,ViInt32 repeat, ViInt32 sequence);
ViStatus _VI_FUNC  chr63200A_SelectSequence (ViSession vi,ViInt32 sequenceIndex);
ViStatus _VI_FUNC  chr63200A_ConfigureProgramTriggerMode (ViSession vi,ViInt32 triggerMode);
ViStatus _VI_FUNC  chr63200A_ConfigureProgramRunMode (ViSession vi,ViInt32 runMode, ViInt32 modeRange);
ViStatus _VI_FUNC  chr63200A_ConfigureProgramListValue (ViSession vi,ViReal64 loadValue);
ViStatus _VI_FUNC  chr63200A_ConfigureProgramStepValue (ViSession vi,ViReal64 startValue,ViReal64 endValue);
ViStatus _VI_FUNC  chr63200A_ConfigureProgramSlewRate (ViSession vi,ViReal64 risingSlewRate,ViReal64 fallingSlewRate);
ViStatus _VI_FUNC  chr63200A_ConfigureProgramDwellTime (ViSession vi,ViReal64 dwellTime);
ViStatus _VI_FUNC  chr63200A_ConfigureProgramSpecification (ViSession vi,ViReal64 VH, ViReal64 VL,ViReal64 IH, ViReal64 IL,ViReal64 PH, ViReal64 PL);
ViStatus _VI_FUNC  chr63200A_ConfigureProgramDelayTime (ViSession vi,ViReal64 delayTime);
ViStatus _VI_FUNC  chr63200A_SaveProgram (ViSession vi);
ViStatus _VI_FUNC  chr63200A_GetProgramStates (ViSession vi,ViInt32 *programNumber,ViInt32 *sequnceNumber, ViInt32 *loadMode,ViInt32 *executionState);
ViStatus _VI_FUNC  chr63200A_ClearSequence (ViSession vi);
ViStatus _VI_FUNC  chr63200A_FailOfSequence (ViSession vi, ViChar result[]);
ViStatus _VI_FUNC  chr63200A_RemainSequence (ViSession vi, ViInt32 *result);
ViStatus _VI_FUNC  chr63200A_SpecificationOfSequence (ViSession vi,ViReal64 result[], ViInt32 parameter);
ViStatus _VI_FUNC  chr63200A_StartSpecificationTest (ViSession vi,ViBoolean enable, ViInt32 unit);

ViStatus _VI_FUNC  chr63200A_ConfigureCVCCModeResponse (ViSession vi,ViInt32 response);
ViStatus _VI_FUNC  chr63200A_ConfigureCVCCModeVoltage (ViSession vi,ViReal64 voltage);
ViStatus _VI_FUNC  chr63200A_ConfigureCVCCModeCurrent (ViSession vi,ViReal64 current);
ViStatus _VI_FUNC  chr63200A_ConfigureCRCCModeResistance (ViSession vi,ViReal64 resistance);
ViStatus _VI_FUNC  chr63200A_ConfigureCRCCModeCurrent (ViSession vi,ViReal64 current);
ViStatus _VI_FUNC  chr63200A_ConfigureCVCRModeResistance (ViSession vi,ViReal64 resistance);
ViStatus _VI_FUNC  chr63200A_ConfigureCVCRModeVoltage (ViSession vi,ViReal64 voltage);

ViStatus _VI_FUNC  chr63200A_ConfigureOCPModeLatch (ViSession vi,ViInt32 latch);
ViStatus _VI_FUNC  chr63200A_ConfigureOCPModeStep (ViSession vi,ViInt32 step);
ViStatus _VI_FUNC  chr63200A_ConfigureOCPModeLoadValue (ViSession vi,ViReal64 startValue,ViReal64 endValue);
ViStatus _VI_FUNC  chr63200A_ConfigureOCPModeDwellTime (ViSession vi,ViReal64 dwellTime);
ViStatus _VI_FUNC  chr63200A_ConfigureOCPModeTriggerVoltage (ViSession vi,ViReal64 triggerVoltage);
ViStatus _VI_FUNC  chr63200A_ConfigureOCPModeSpecification (ViSession vi,ViReal64 highLevelCurrent, ViReal64 lowLevelCurrent);

ViStatus _VI_FUNC  chr63200A_ConfigureOPPModeLatch (ViSession vi,ViInt32 latch);
ViStatus _VI_FUNC  chr63200A_ConfigureOPPModeStep (ViSession vi,ViInt32 step);
ViStatus _VI_FUNC  chr63200A_ConfigureOPPModeLoadValue (ViSession vi,ViReal64 startValue,ViReal64 endValue);
ViStatus _VI_FUNC  chr63200A_ConfigureOPPModeDwellTime (ViSession vi,ViReal64 dwellTime);
ViStatus _VI_FUNC  chr63200A_ConfigureOPPModeTriggerVoltage (ViSession vi,ViReal64 triggerVoltage);
ViStatus _VI_FUNC  chr63200A_ConfigureOPPModeSpecification (ViSession vi,ViReal64 highLevelPower, ViReal64 lowLevelPower); 

ViStatus _VI_FUNC  chr63200A_ConfigureSWDModeDCCurrent (ViSession vi,ViReal64 DCCurrent);
ViStatus _VI_FUNC  chr63200A_ConfigureSWDModeACCurrent (ViSession vi,ViReal64 ACCurrent);
ViStatus _VI_FUNC  chr63200A_ConfigureSWDModeFrequency (ViSession vi,ViReal64 frequency);
ViStatus _VI_FUNC  chr63200A_ConfigureSWDModeMeasureRange (ViSession vi,ViInt32 measureRange);

ViStatus _VI_FUNC  chr63200A_ConfigureSweepModeCurrent (ViSession vi,ViReal64 maxCurrent,ViReal64 minCurrent);
ViStatus _VI_FUNC  chr63200A_ConfigureSweepModeFrequency (ViSession vi,ViReal64 startFrequency,ViReal64 endFrequency,ViReal64 stepFrequency);
ViStatus _VI_FUNC  chr63200A_ConfigureSweepModeDwellTime (ViSession vi,ViReal64 dwellTime);
ViStatus _VI_FUNC  chr63200A_ConfigureSweepModeDuty (ViSession vi,ViReal64 duty);
ViStatus _VI_FUNC  chr63200A_ConfigureSweepModeSlewRate (ViSession vi,ViReal64 fallingSlewRate,ViReal64 risingSlewRate);
ViStatus _VI_FUNC  chr63200A_ConfigureSweepModeMeasureRange (ViSession vi,ViInt32 measureRange);

ViStatus _VI_FUNC  chr63200A_ConfigureCZMode (ViSession instrumentHandle, ViInt32 item,ViReal64 itemValue);

ViStatus _VI_FUNC  chr63200A_ConfigureEXTWMode (ViSession instrumentHandle, ViInt32 mode,ViInt32 measureRange);

ViStatus _VI_FUNC  chr63200A_SelectUserWaveform (ViSession vi,ViInt32 number);
ViStatus _VI_FUNC  chr63200A_ConfigureUserWaveformData (ViSession vi,ViReal64 interval, ViInt32 repeat,ViInt32 chain,ViInt32 interpolation);
ViStatus _VI_FUNC  chr63200A_ConfigureUserWaveformPoint (ViSession vi,ViChar point[]);
ViStatus _VI_FUNC  chr63200A_GetUserWaveformRemainPoint (ViSession vi,ViInt32 *remain);
ViStatus _VI_FUNC  chr63200A_GetUserWaveformStatus (ViSession vi,ViInt32 *dataStatus,ViInt32 *executeStatus);
ViStatus _VI_FUNC  chr63200A_ClearUserWaveform (ViSession vi,ViInt32 *clearStatus);

ViStatus _VI_FUNC  chr63200A_ConfigureVoltageSpecification (ViSession vi,ViReal64 lowLevelVoltage, ViReal64 centerLevelVoltage,ViReal64 highLevelVoltage);
ViStatus _VI_FUNC  chr63200A_ConfigureCurrentSpecification (ViSession vi,ViReal64 lowLevelCurrent, ViReal64 centerLevelCurrent,ViReal64 highLevelCurrent);
ViStatus _VI_FUNC  chr63200A_ConfigurePowerSpecification (ViSession vi,ViReal64 lowLevelPower, ViReal64 centerLevelPower,ViReal64 highLevelPower);

ViStatus _VI_FUNC  chr63200A_ParallelAndSynchronous (ViSession vi,ViInt32 enableParallel,ViInt32 parallelType,ViInt32 syncType);
ViStatus _VI_FUNC  chr63200A_SystemSetup (ViSession vi, ViInt32 sound,ViInt32 enterKey);
ViStatus _VI_FUNC  chr63200A_LoadSetup (ViSession vi, ViInt32 latchReset,ViInt32 latch, ViReal64 von, ViReal64 voff,ViInt32 shortKey, ViInt32 autoOn);
ViStatus _VI_FUNC  chr63200A_MeasurementSetting (ViSession vi, ViInt32 sign,ViReal64 windowTime);
ViStatus _VI_FUNC  chr63200A_DIO (ViSession vi, ViInt32 DI1Type,ViInt32 DO1Type, ViBoolean DO1, ViInt32 DI2Type,ViInt32 DO2Type, ViBoolean DO2);

/****************************************************************************
 *------------------------ Error And Completion Codes ----------------------*
 ****************************************************************************/
#define CHR63200A_ERROR_MODE  								(IVI_SPECIFIC_ERROR_BASE + 1)
#define CHR63200A_ERROR_COMMAND_TYPE						(IVI_SPECIFIC_ERROR_BASE + 2)
#define	CHR63200A_ERROR_QUERY_PARAMETER						(IVI_SPECIFIC_ERROR_BASE + 3)

#define	CHR63200A_ERROR_PROGRAM_RUN_TYPE						(IVI_SPECIFIC_ERROR_BASE + 100)
#define CHR63200A_ERROR_PROGRAM_LIST_NUMBER  					(IVI_SPECIFIC_ERROR_BASE + 101)
#define CHR63200A_ERROR_PROGRAM_LIST_SEQUENCE   				(IVI_SPECIFIC_ERROR_BASE + 102)
#define CHR63200A_ERROR_PROGRAM_LIST_TRIGGER_MODE   			(IVI_SPECIFIC_ERROR_BASE + 103)
#define CHR63200A_ERROR_PROGRAM_LIST_RUN_MODE   				(IVI_SPECIFIC_ERROR_BASE + 104)
#define CHR63200A_ERROR_PROGRAM_LIST_MODE_RANGE   				(IVI_SPECIFIC_ERROR_BASE + 105)
#define CHR63200A_ERROR_PROGRAM_LIST_LOAD_VALUE   				(IVI_SPECIFIC_ERROR_BASE + 106)
#define CHR63200A_ERROR_PROGRAM_LIST_FALLING		   			(IVI_SPECIFIC_ERROR_BASE + 107)
#define CHR63200A_ERROR_PROGRAM_LIST_RISING		   				(IVI_SPECIFIC_ERROR_BASE + 108)
#define CHR63200A_ERROR_PROGRAM_LIST_DWELL		   				(IVI_SPECIFIC_ERROR_BASE + 109)
#define CHR63200A_ERROR_PROGRAM_LIST_VOLTAGE_HIGH   			(IVI_SPECIFIC_ERROR_BASE + 110)
#define CHR63200A_ERROR_PROGRAM_LIST_VOLTAGE_LOW	   			(IVI_SPECIFIC_ERROR_BASE + 111)
#define CHR63200A_ERROR_PROGRAM_LIST_CURRENT_HIGH   			(IVI_SPECIFIC_ERROR_BASE + 112)
#define CHR63200A_ERROR_PROGRAM_LIST_CURRENT_LOW	   			(IVI_SPECIFIC_ERROR_BASE + 113)
#define CHR63200A_ERROR_PROGRAM_LIST_POWER_HIGH					(IVI_SPECIFIC_ERROR_BASE + 114)
#define CHR63200A_ERROR_PROGRAM_LIST_POWER_LOW	   				(IVI_SPECIFIC_ERROR_BASE + 115)
#define CHR63200A_ERROR_PROGRAM_LIST_DELAY		   				(IVI_SPECIFIC_ERROR_BASE + 116)

#define CHR63200A_ERROR_PROGRAM_STEP_NUMBER  					(IVI_SPECIFIC_ERROR_BASE + 151)
#define CHR63200A_ERROR_PROGRAM_STEP_TRIGGER_MODE   			(IVI_SPECIFIC_ERROR_BASE + 152)
#define CHR63200A_ERROR_PROGRAM_STEP_RUN_MODE  		 			(IVI_SPECIFIC_ERROR_BASE + 153)
#define CHR63200A_ERROR_PROGRAM_STEP_MODE_RANGE   				(IVI_SPECIFIC_ERROR_BASE + 154)
#define CHR63200A_ERROR_PROGRAM_STEP_START_VALUE   				(IVI_SPECIFIC_ERROR_BASE + 155)
#define CHR63200A_ERROR_PROGRAM_STEP_END_VALUE   				(IVI_SPECIFIC_ERROR_BASE + 156)
#define CHR63200A_ERROR_PROGRAM_STEP_FALLING		   			(IVI_SPECIFIC_ERROR_BASE + 157)
#define CHR63200A_ERROR_PROGRAM_STEP_RISING		   				(IVI_SPECIFIC_ERROR_BASE + 158)
#define CHR63200A_ERROR_PROGRAM_STEP_DWELL		   				(IVI_SPECIFIC_ERROR_BASE + 159)
#define CHR63200A_ERROR_PROGRAM_STEP_VOLTAGE_HIGH   			(IVI_SPECIFIC_ERROR_BASE + 160)
#define CHR63200A_ERROR_PROGRAM_STEP_VOLTAGE_LOW	   			(IVI_SPECIFIC_ERROR_BASE + 161)
#define CHR63200A_ERROR_PROGRAM_STEP_CURRENT_HIGH   			(IVI_SPECIFIC_ERROR_BASE + 162)
#define CHR63200A_ERROR_PROGRAM_STEP_CURRENT_LOW	   			(IVI_SPECIFIC_ERROR_BASE + 163)
#define CHR63200A_ERROR_PROGRAM_STEP_POWER_HIGH					(IVI_SPECIFIC_ERROR_BASE + 164)
#define CHR63200A_ERROR_PROGRAM_STEP_POWER_LOW	   				(IVI_SPECIFIC_ERROR_BASE + 165)
#define CHR63200A_ERROR_PROGRAM_STEP_DELAY		   				(IVI_SPECIFIC_ERROR_BASE + 166)

#define CHR63200A_ERROR_WFDATA_STATUS							(IVI_SPECIFIC_ERROR_BASE + 170)
#define CHR63200A_ERROR_UDW_POINT_OUTOFRANGE					(IVI_SPECIFIC_ERROR_BASE + 171)


#define CHR63200A_ERROR_DIG_CAP_WF_FAIL							(IVI_SPECIFIC_ERROR_BASE + 190)


/**************************************************************************** 
 *---------------------------- End Include File ----------------------------* 
 ****************************************************************************/



#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif
#endif /* __CHR63200A_HEADER */
