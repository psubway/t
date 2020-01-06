/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FILE:    cd_dev.c                                                         */
/*                                                                           */
/* PURPOSE: To handle     Charger and Discharger                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Include files                                                             */
/*---------------------------------------------------------------------------*/

#include <ansi_c.h>
#include <userint.h>
#include <utility.h>
#include <cvirte.h>  
#include <stdio.h>   
#include "BTBController.h"
#include "define.h"

#include "agn8700.h"
#include "chr63200A.h"

#define STR_LEN		256

static ViSession	agn8700;
static ViSession	chr63200;
static ViStatus		error = VI_SUCCESS; 

static ViReal64		fResult=-1;
static ViBoolean	blnLoadStatus[16];

static ViBoolean	overVoltage = VI_FALSE;
static ViBoolean	overCurrent = VI_FALSE;
static ViBoolean	constantVoltage = VI_TRUE;
static ViBoolean	constantCurrent = VI_FALSE;

static bool 		charger_rising_delay = true;
static bool 		discharger_rising_delay = true;



void BuildErrorString (ViSession agn8700, ViStatus error, ViString errStr , int deviceNum);

float ch_device_init()
{
	float ret;
	ret = 0;
	error = VI_SUCCESS;

#ifdef AGN8700
	if (agn8700)
        agn8700_close (agn8700);
	
	ret = agn8700_InitWithOptions(CHARGER_NAME,	            /* resourceName */
										VI_TRUE,			/* IdQuery */
										VI_TRUE,			/* resetDevice */
										"Simulate=1,RangeCheck=0,QueryInstrStatus=0,Cache=0,DriverSetup=Model:N8731A",
										&agn8700);

	if(ret<0)
	{
		event_send(EventTabNum, TABPANEL_TEXTBOX, sError, ret, MSG_ERROR5);
		SetCtrlVal(panelErrMessage,PNL_ERRMSG_TEXTMSG,MSG_ERROR5);
		DisplayPanel(panelErrMessage);
		if(agn8700)
			agn8700_close(agn8700);
		return ret;
	}
	else
	{
		event_send(EventTabNum, TABPANEL_TEXTBOX, EventMessage, NULL, MSG_CHARGER_ON);
		charger_live = ON;
	}
#endif	//#ifdef AGN8700

#ifdef CHR63200
	if(chr63200)
		chr63200A_close(chr63200);
	
	ret = chr63200A_InitWithOptions(DISCHARGER_NAME,	
										VI_TRUE,
										VI_TRUE,
										"Simulate=1,RangeCheck=0,QueryInstrStatus=0,Cache=0",&chr63200);

	if(ret<0)
	{
		event_send(EventTabNum, TABPANEL_TEXTBOX, sError, ret, MSG_ERROR6);
		SetCtrlVal(panelErrMessage,PNL_ERRMSG_TEXTMSG,MSG_ERROR6);
		DisplayPanel(panelErrMessage);
		if(chr63200)
			chr63200A_close(chr63200);
		return ret;
	}
	else
	{
		event_send(EventTabNum, TABPANEL_TEXTBOX, EventMessage, NULL, MSG_DISCHARGER_ON);
		discharger_live = ON;
	}
#endif	//#ifdef CHR63200
	return ret;
}

#ifdef AGN8700||CHR63200
void BuildErrorString (ViSession device, ViStatus error, ViString errStr, int deviceNum)
{
    ViChar   tempStr[STR_LEN];
    ViChar   *p = errStr;
    ViStatus primaryErr;
                                
    /* Get description of the returned status code */
	if(deviceNum == AGN8700)
	{
		p += sprintf (p, "Error: Charger(Hex 0x%x)", error);
   		agn8700_error_message(device, error, tempStr);
	}
	else if(deviceNum == CHR63200)
	{
		p += sprintf (p, "Error: Discharger(Hex 0x%x)", error);
		chr63200A_error_message(device, error, tempStr);
	}
	
	sprintf (p, " %s", tempStr);
}
#endif	//#ifdef AGN8700||CHR63200

bool charger_on()
{
	static unsigned waiting_cnt;
	
	waiting_cnt = 0;
	while(!charger_relay)
	{
		if(waiting_cnt++>20)
		{
			waiting_cnt = 0;
			goto Error;
			break;
		}
		Delay(0.5);
	};
	
#ifndef AGN8700
	return true;

Error:
	return false;
#else
	// To-do apply ethernet Control
	ViReal64 tc = 0, tv = 0;
	
	tc = (ViReal64)TargetCurrentValue;
	tv = (ViReal64)TargetVoltageValue;
	
	checkErr( agn8700_ConfigureOutputEnabled (agn8700, "", VI_FALSE));
	checkErr( agn8700_ConfigureCurrentLimit (agn8700, "", AGN8700_VAL_CURRENT_REGULATE, tc));  // max 25A
	checkErr( agn8700_ConfigureOVP (agn8700, "", VI_TRUE, 55.0));		// max 50V, the voltage cannot be programmed higher than about 5% below the OVP setting.
	checkErr( agn8700_ConfigureVoltageLevel (agn8700, "", tv));
	checkErr( agn8700_ConfigureOutputEnabled (agn8700, "", VI_TRUE));
	
	Delay(2.0);/* wait for some time until the voltage has been adjusted to the setting value. */

    /* checking device status and mode  */
	checkErr( agn8700_QueryOutputState (agn8700, "", AGN8700_VAL_OUTPUT_OVER_VOLTAGE, &overVoltage));
	checkErr( agn8700_QueryOutputState (agn8700, "", AGN8700_VAL_OUTPUT_OVER_CURRENT, &overCurrent));
	checkErr( agn8700_QueryOutputState (agn8700, "", AGN8700_VAL_OUTPUT_CONSTANT_CURRENT, &constantCurrent));
	checkErr( agn8700_QueryOutputState (agn8700, "", AGN8700_VAL_OUTPUT_CONSTANT_VOLTAGE, &constantVoltage));
	
	checkErr( Ivi_SetAttributeViBoolean (agn8700, "", IVIDCPWR_ATTR_OUTPUT_ENABLED, 0, VI_TRUE));

	return true;
Error:
	if(error != VI_SUCCESS)
	{
		ViChar   errStr[2048];
		
		BuildErrorString(agn8700, error, errStr, AGN8700);

		if(BTBWorkingStatus < STS_ERROR)
		{
			SetErrorMessage(errStr,error,AutoRun);
			btb_status_change(STS_ERROR);
		}
		agn8700_ConfigureOutputEnabled (agn8700, "", VI_FALSE);
		
		if(overVoltage || overCurrent)
			agn8700_ResetOutputProtection (agn8700, "");
		if (agn8700)
			agn8700_close (agn8700);

	}
	return false;
#endif
}

bool charger_off()
{
#ifdef AGN8700
	checkErr(agn8700_ConfigureOutputEnabled(agn8700, "", VI_FALSE));	
#endif

	charger_rising_delay = true;
	
	return true;
	
Error:
	return false;
}

bool discharger_on()
{
	static unsigned waiting_cnt;

	waiting_cnt = 0;
	while(!discharger_relay)
	{
		if(waiting_cnt++>10)
		{
			waiting_cnt = 0;
			goto Error;
			break;
		}
		Delay(1);
	};
	
#ifndef CHR63200
	return true;
Error:
	return false;
#else
	ViReal64 tc = 0;
	// To-do apply ethernet Control
	tc = (ViReal64)TargetCurrentValue;
	checkErr(chr63200A_ElectronicLoadState (chr63200,0, 0,blnLoadStatus,0));
	checkErr(chr63200A_OperatingMode (chr63200, CHR63200A_VAL_MOD_CCL, 0));
	checkErr(chr63200A_ConfigureCCModeMeasureRange (chr63200, 1));
	checkErr(chr63200A_ConfigureCCModeLoadValue (chr63200, tc));
	checkErr(chr63200A_ConfigureCCModeSlewRate (chr63200, 1.0, 1.0));
	checkErr(chr63200A_ElectronicLoadState (chr63200,1, 0,blnLoadStatus,0));
	checkErr(chr63200A_Measure(chr63200,0,0,&fResult));

	SetCtrlVal(panelHandle,PANEL_TEXTMSG_6,blnLoadStatus);
	
	return true;
	
Error:
	if(error != VI_SUCCESS)
	{
		ViChar   errStr[2048];

		BuildErrorString (agn8700, error, errStr, CHR63200);

		if(BTBWorkingStatus < STS_ERROR)
		{
			SetErrorMessage(errStr,error,AutoRun);
			btb_status_change(STS_ERROR);
		}
		chr63200A_ElectronicLoadState (chr63200,0, 0,blnLoadStatus,0);
		
		if(chr63200)
			chr63200A_close(chr63200);
	}

	return false;
#endif
}

bool discharger_off()
{
#ifdef CHR63200
	checkErr(chr63200A_ElectronicLoadState (chr63200,0, 0,blnLoadStatus,0));
#endif
	
	discharger_rising_delay = true;
	return true;
	
Error:
	return false;
}
void ch_device_off()
{
#ifdef AGN8700
	SetCtrlVal(panelHandle,PANEL_CHARGER_LED,OFF);
	if (agn8700)
		agn8700_ConfigureOutputEnabled(agn8700, "", VI_FALSE);	
#endif

#ifdef CHR63200
	SetCtrlVal(panelHandle,PANEL_DISCHARGER_LED,OFF);
	if(chr63200)
		chr63200A_ElectronicLoadState (chr63200,0, 0,blnLoadStatus,0);
#endif
}
void ch_device_close()
{
#ifdef AGN8700
	if (agn8700)
		agn8700_close (agn8700);
#endif

#ifdef CHR63200
	if(chr63200)
		chr63200A_close(chr63200);
#endif
}

#ifdef DEVICE_CHECK
void charger_checking()
{
	double voltage_c;
	double current_c;
	int voltage_diff;
	int current_diff;

	agn8700_Measure(agn8700,"",AGN8700_VAL_MEASURE_VOLTAGE,&voltage_c);
	agn8700_Measure(agn8700,"",AGN8700_VAL_MEASURE_CURRENT,&current_c);

	voltage_diff = abs(BTBPackVoltage - voltage_c);
	current_diff = abs(BTBPackCurrent - current_c);
	

	if(voltage_diff > VOLT_DIFF)
	{
		if(BTBWorkingStatus < STS_ERROR)
		{
			SetErrorMessage(MSG_ERROR12,voltage_diff,AutoRun);
			btb_status_change(STS_ERROR);
		}
	}
	else if(current_diff > CURR_DIFF)
	{
		if(BTBWorkingStatus < STS_ERROR)
		{
			SetErrorMessage(MSG_ERROR13,current_diff,AutoRun);
			btb_status_change(STS_ERROR);
		}
	}
}

void charger_checking_target_current()
{
	double current_c;
	int current_diff;

	if(charger_rising_delay)
	{
		Delay(5);
		charger_rising_delay = false;
	}

	agn8700_Measure(agn8700,"",AGN8700_VAL_MEASURE_CURRENT,&current_c);

	current_diff = abs(TargetCurrentValue - current_c);
	SetCtrlVal(panelHandle,PANEL_NUMERIC_24,current_diff);	//To-Do: delete it
	
	if(current_diff > CURR_DIFF)
	{
		if(BTBWorkingStatus < STS_ERROR)
		{
			SetErrorMessage(MSG_ERROR13,current_diff,AutoRun);
			btb_status_change(STS_ERROR);
		}
	}
}

void discharger_checking()
{
	double voltage_d;
	double current_d;
	int voltage_diff;
	int current_diff;

	chr63200A_Measure(chr63200,0,0,&current_d);	// 0: current
	chr63200A_Measure(chr63200,0,2,&voltage_d);	// 2: voltage

	voltage_diff = abs(BTBPackVoltage - voltage_d);
	current_diff = abs(BTBPackCurrent - current_d);

	if(voltage_diff > VOLT_DIFF)
	{
		if(BTBWorkingStatus < STS_ERROR)
		{
			SetErrorMessage(MSG_ERROR14,voltage_diff,AutoRun);
			btb_status_change(STS_ERROR);
		}
	}
	else if(current_diff > CURR_DIFF)
	{
		if(BTBWorkingStatus < STS_ERROR)
		{
			SetErrorMessage(MSG_ERROR15,current_diff,AutoRun);
			btb_status_change(STS_ERROR);
		}
	}
}

void discharger_checking_target_current()
{
	double current_d;
	int current_diff;

	if(discharger_rising_delay)
	{
		Delay(5);
		discharger_rising_delay = false;
	}
	
	chr63200A_Measure(chr63200,0,0,&current_d);	// 0: current

	current_diff = abs(TargetCurrentValue - current_d);

	if(current_diff > CURR_DIFF)
	{
		if(BTBWorkingStatus < STS_ERROR)
		{
			SetErrorMessage(MSG_ERROR15,current_diff,AutoRun);
			btb_status_change(STS_ERROR);
		}
	}
}
#endif //#ifdef DEVICE_CHECK

void display_charger_measurment()
{
	double voltage_c;
	double current_c;

	agn8700_Measure(agn8700,"",AGN8700_VAL_MEASURE_VOLTAGE,&voltage_c);
	agn8700_Measure(agn8700,"",AGN8700_VAL_MEASURE_CURRENT,&current_c);

	SetCtrlVal(panelHandle,PANEL_CHARGEVOLTAGE,voltage_c);
	SetCtrlVal(panelHandle,PANEL_CHARGECURRENT,current_c);

	    /* checking device status and mode  */
	//agn8700_QueryOutputState (agn8700, "", AGN8700_VAL_OUTPUT_OVER_VOLTAGE, &overVoltage);
	//agn8700_QueryOutputState (agn8700, "", AGN8700_VAL_OUTPUT_OVER_CURRENT, &overCurrent);
	agn8700_QueryOutputState (agn8700, "", AGN8700_VAL_OUTPUT_CONSTANT_CURRENT, &constantCurrent);
	agn8700_QueryOutputState (agn8700, "", AGN8700_VAL_OUTPUT_CONSTANT_VOLTAGE, &constantVoltage);

	if(constantVoltage)
		SetCtrlVal(panelHandle,PANEL_CH_MODE,"CV");
	else if(constantCurrent)
		SetCtrlVal(panelHandle,PANEL_CH_MODE,"CC");
}

void display_discharger_measurment()
{
	double voltage_d;
	double current_d;

	chr63200A_Measure(chr63200,0,0,&current_d);	// 0: current
	chr63200A_Measure(chr63200,0,2,&voltage_d);	// 2: voltage

	SetCtrlVal(panelHandle,PANEL_DISCHARGEVOLTAGE,voltage_d);
	SetCtrlVal(panelHandle,PANEL_DISCHARGECURRENT,current_d);
}
