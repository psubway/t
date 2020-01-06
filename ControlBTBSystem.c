/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FILE:    ControlBTBSystem.c                                               */
/*                                                                           */
/* PURPOSE: To run BTB Controller with I/F Box & Charger/Discharger          */
/*                                                                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Include files                                                             */
/*---------------------------------------------------------------------------*/
#include <udpsupp.h>
#include <ansi_c.h>
#include <tcpsupp.h>
#include "radioGroup.h"
#include <userint.h>
#include <utility.h>
#include "BTBController.h"
#include "define.h"

static enum fullchargestatus FullCharge_WorkingStatus = Dispcheck_Fullcharge;
static enum cellbalancingstatus CellBalance_WorkingStatus = Dispcheck1_Cellbalance;
static enum endofstoragestatus EndOfStorage_WorkingStatus = Dispcheck_Endofstorage;
unsigned __int64 settingtimeStart, settingtimeElapsed;

static int device_check_flag;
char *btb_status_string(int status)
{
	switch(status)
	{
		case STS_GETVALUE:
			return 	"Booting";
		case STS_INIT:
			return "Initializing";
		case STS_CONNECT:
			return "IF connecting";
		case STS_DEVICEINIT:
			return "IF connected";
		case STS_STANDBY:
			return "Standby";
		case STS_CHARGING:
			return "Charging";
		case STS_DISCHARGING:
			return "Discharging";
		case STS_FULLCHARGING:
			return "Full Charging";
		case STS_CELLBALANCING:
			return "Cell Balancing";
		case STS_ENDOFSTORAGE:
			return "End of Storage";
		case STS_DISCONNECT:
			return "Connection Error";
		case STS_ALARM1:
			return "Alarm(Voltage)";
		case STS_ALARM2:
			return "Alarm(Temperature)";
		case STS_ALARM3:
			return "Alarm(Dispersion)";
		case STS_ERROR:
		case STS_OUTOFSTATUS:	
			return "Error";
		default:
			return "UNKOWN error";
	}
}

static char error_msg[MESSAGE_LEN];
static int error_data;
static int error_mode;
/*
 * We handle the btb status here.  We allow this status to be modified just here.
 */
void btb_status_change(btb_status status)
{
	BTBWorkingStatus = status;
}

void SetErrorMessage(char* message, int data, int mode)
{
	strcpy(error_msg,message);
	error_data = data;
	error_mode = mode;
}

/*---------------------------------------------------------------------------*/
/* initialize setting values on sub-panels                                   */	
/*---------------------------------------------------------------------------*/
void panel_setting_value(void)
{
	int flag;

	modelValue = operator.ModelID;
	SetCtrlVal (panelHandle, PANEL_BTBMODELNAME, modelValue); 
	cell_number_setup();

	if(btb_option == 0)
	{
		SetCtrlVal (panelCFGCharging, PNL_M1_TARGET_VOLTAGE, operator.CfgCharge.ChargeVoltage);
		SetCtrlVal (panelCFGDischarging, PNL_M2_TARGET_VOLTAGE, operator.CfgDischarge.DischargeVoltage);
		SetCtrlVal (panelCFGFullcharging, PNL_M3_TARGET_VOLTAGE, operator.CfgFullcharge.ChargeVoltage); 
		SetCtrlVal (panelCFGCellbalancing, PNL_M4_TARGET_VOLTAGE, operator.CfgCellBalance.ChargeVoltage);
		SetCtrlVal (panelCFGCellbalancing, PNL_M4_D_TARGET_VOLTAGE, operator.CfgCellBalance.DischargeVoltage);
		SetCtrlVal (panelCFGEndofstorage, PNL_M5_TARGET_VOLTAGE, operator.CfgEndOfStorage.ChargeVoltage);	
	}
	else
	{
		SetCtrlVal (panelCFGCharging, PNL_M1_TARGET_VOLTAGE, operator.CfgCharge.ChargeVoltage1CellFailed);
		SetCtrlVal (panelCFGDischarging, PNL_M2_TARGET_VOLTAGE, operator.CfgDischarge.DischargeVoltage1CellFailed);
		SetCtrlVal (panelCFGFullcharging, PNL_M3_TARGET_VOLTAGE, operator.CfgFullcharge.ChargeVoltage1CellFailed);
		SetCtrlVal (panelCFGCellbalancing, PNL_M4_TARGET_VOLTAGE, operator.CfgCellBalance.ChargeVoltage1CellFailed); 
		SetCtrlVal (panelCFGCellbalancing, PNL_M4_D_TARGET_VOLTAGE, operator.CfgCellBalance.DischargeVoltage1CellFailed); 
		SetCtrlVal (panelCFGEndofstorage, PNL_M5_TARGET_VOLTAGE, operator.CfgEndOfStorage.ChargeVoltage1CellFailed);
	}
	
	SetCtrlVal (panelCFGCharging, PNL_M1_TARGET_CURRENT, operator.CfgCharge.ChargeCurrent);
	if(operator.CfgCharge.ChargeCurrent == 9.0)
	{
		SetCtrlVal (panelCFGCharging, PNL_M1_RING, 0);
	}
	else if(operator.CfgCharge.ChargeCurrent == 18.0)
	{
		SetCtrlVal (panelCFGCharging, PNL_M1_RING, 1);
	}
	else
	{
		SetCtrlVal (panelCFGCharging, PNL_M1_RING, 2);
	}

	SetCtrlVal (panelCFGDischarging, PNL_M2_TARGET_CURRENT, operator.CfgDischarge.DischargeCurrent);
	if(operator.CfgDischarge.DischargeCurrent == 30.0)
	{
		SetCtrlVal (panelCFGDischarging, PNL_M2_RING, 0);
	}
	else
	{
		SetCtrlVal (panelCFGDischarging, PNL_M2_RING, 1);
	}
	
	GetCtrlVal (panelCFGCharging, PNL_M1_RING, &flag);
	if(flag !=2)
	{
		SetCtrlAttribute (panelCFGCharging, PNL_M1_TARGET_CURRENT, ATTR_DIMMED, ON);
	}

	GetCtrlVal (panelCFGDischarging, PNL_M2_RING, &flag);
	if(flag !=1)
	{
		SetCtrlAttribute (panelCFGDischarging, PNL_M2_TARGET_CURRENT, ATTR_DIMMED, ON);
	}

	SetCtrlVal(panelCFGFullcharging, PNL_M3_TARGET_CURRENT, operator.CfgFullcharge.ChargeCurrent); 
	SetCtrlVal(panelCFGFullcharging, PNL_M3_TAPERING, operator.CfgFullcharge.TaperingFactor);
	SetCtrlVal(panelCFGFullcharging, PNL_M3_END_CURRENT, operator.CfgFullcharge.ChargeCurrentLimit);
	SetCtrlVal(panelCFGFullcharging, PNL_M3_SETTLING_TIME, operator.CfgFullcharge.SettlingTime);

	SetCtrlVal(panelCFGCellbalancing, PNL_M4_TARGET_CURRENT, operator.CfgCellBalance.ChargeCurrent); 
	SetCtrlVal(panelCFGCellbalancing, PNL_M4_D_TARGET_CURRENT, operator.CfgCellBalance.DischargeCurrent);
	SetCtrlVal(panelCFGCellbalancing, PNL_M4_TAPERING, operator.CfgCellBalance.TaperingFactor);
	SetCtrlVal(panelCFGCellbalancing, PNL_M4_END_CURRENT, operator.CfgCellBalance.ChargeCurrentLimit);
	SetCtrlVal(panelCFGCellbalancing, PNL_M4_SETTLING_TIME, operator.CfgCellBalance.SettlingTime);
	SetCtrlVal(panelCFGCellbalancing, PNL_M4_REPEAT_CNT, operator.CfgCellBalance.RepeatCount);

	SetCtrlVal(panelCFGEndofstorage, PNL_M5_TARGET_CURRENT, operator.CfgEndOfStorage.ChargeCurrent);
	SetCtrlVal(panelCFGEndofstorage, PNL_M5_D_TARGET_CURRENT, operator.CfgEndOfStorage.DischargeCurrent);
	SetCtrlVal(panelCFGEndofstorage, PNL_M5_REPEAT_CNT, operator.CfgEndOfStorage.RepeatCount);
	
	SetCtrlVal(panelInterval, PNL_M6_NUMERIC, operator.CfgFileGraph.Interval);
	SetCtrlVal(panelModelList, PNL_M7_RING, operator.ModelID);
	SetCtrlVal(panelOperator,PNL_M8_STRING,operator.OperatorName);

	SetCtrlVal (panelTemperature, PNL_TEMP_NUMERIC, operator.Temperature.limit1);
	SetCtrlVal (panelTemperature, PNL_TEMP_NUMERIC_2, operator.Temperature.limit2);
}

int CVICALLBACK BTBWorkingThread(void *functionData)
{
	int ret;
	char CurrentDir[TEXT_SIZE];
	char iniFile[TEXT_SIZE];

	while(1)
	{
		switch(BTBWorkingStatus)
		{
			case STS_GETVALUE:
				event_send(EventTabNum, TABPANEL_TEXTBOX, EventMessage, NULL,MSG_INIT);
				SetCtrlAttribute (panelHandle, PANEL_METHODOPTION, ATTR_DIMMED, ON);
			
				/***** setting up BTB Configuration from INI file *****/
				/* Get the current directory */
				ret = GetProjectDir(CurrentDir);
				if(ret < 0)
				{
					SetErrorMessage(MSG_ERROR3,ret,AutoRun);
					btb_status_change(STS_ERROR);
				}
				
				sprintf(iniFile,"%s\\ini\\default_config.ini",CurrentDir);

				ret = ini_open_user(iniFile);	//	default setting values for user
				if(ret < 0)
				{
					SetErrorMessage(MSG_ERROR4,ret,AutoRun);
					btb_status_change(STS_ERROR);
				}
				else
				{
					ret = ini_open_default(iniFile);	//	default setting values for reset
					if(ret < 0)
					{
						SetErrorMessage(MSG_ERROR4,ret,AutoRun);
						btb_status_change(STS_ERROR);
					}
					else
					{
						btb_status_change(STS_INIT);
					}
				}
				filesave_progress=INIT;
				break;
			case STS_INIT:
				/* initialize all property */
				connection_flag = OFF;
				btb_option = operator.BatteryCellOption;
				interval_value = operator.CfgFileGraph.Interval;
				methodexecution_flag = OFF;
				TargetVoltageValue = 0;
				TargetCurrentValue = 0;
				TargetMode = MODE_CC;
				charger_live = OFF;
				discharger_live = OFF;
				emergency_bt = OFF;
				
				SetCtrlVal(panelHandle,PANEL_STRING,operator.OperatorName);
				SetCtrlVal(panelHandle,PANEL_CELLUSABILITY,operator.BatteryCellOption);
				//SetCtrlAttribute (panelHandle, PANEL_PICTURE_ALLOFF, ATTR_VISIBLE, ON);
				picture_all_relay_off();
				//SetCtrlAttribute (panelHandle, PANEL_PICTURE_CHARGEON, ATTR_VISIBLE, OFF);
				//SetCtrlAttribute (panelHandle, PANEL_PICTURE_DISCHARGEON, ATTR_VISIBLE, OFF);
				
				SetCtrlAttribute (panelHandle, PANEL_EXECUTION, ATTR_DIMMED, ON);
				SetCtrlVal(panelHandle, PANEL_EXECUTION, OFF);
				
				SetCtrlAttribute (panelHandle, PANEL_CH_MODE, ATTR_VISIBLE, OFF);
				SetCtrlAttribute (panelHandle, PANEL_AMPEREHOUR, ATTR_DIMMED, ON);
				SetCtrlAttribute (panelHandle, PANEL_RESISTANCE, ATTR_DIMMED, ON);
				SetCtrlAttribute (panelHandle, PANEL_ENERGY, ATTR_DIMMED, ON);

				panel_setting_value();	// for option
				
				event_send(EventTabNum, TABPANEL_TEXTBOX, EventMessage, NULL, MSG_UDP_START);
				btb_status_change(STS_CONNECT);
				break;
			case STS_CONNECT:
				if(connection_flag)
				{
					SetCtrlVal (panelHandle, PANEL_CONNECTED, ON);
					timeStartBTB = 0;
					timeElapsed = 0;
					btb_status_change(STS_DEVICEINIT);
				}
				break;
			case STS_DEVICEINIT:
				if(ch_device_init()<0)
				{
					btb_status_change(STS_OUTOFSTATUS);
				}
				else
				{
					device_check_flag = ON;
					SetCtrlAttribute (panelHandle, PANEL_METHODOPTION, ATTR_DIMMED, OFF);
					SetCtrlAttribute (panelHandle, PANEL_CELLUSABILITY, ATTR_DIMMED, OFF);
					ClearStripChart (VoltageTabNum, TABPANEL_2_STRIPCHART);
					btb_status_change(STS_STANDBY);
				}
				break;
			case STS_STANDBY:
				;
				break;
			case STS_CHARGING:
				func_charging();
				break;
			case STS_DISCHARGING:
				func_discharging();
				break;
			case STS_FULLCHARGING:
				func_fullcharging();
				break;
			case STS_CELLBALANCING:
				func_cellbalancing();
				break;
			case STS_ENDOFSTORAGE:
				func_endofstorage();
				break;
#ifdef DISCONNECTION
			case STS_DISCONNECT:
				if(connection_flag)
					btb_status_change(STS_STANDBY);
				break;
#endif
			case STS_ALARM1:		//voltage alarm
			case STS_ALARM2:		//temperature alarm
			case STS_ALARM3:		//etc
				break;
			case STS_ERROR:
				//ch_device_off();  To-Do: needs to check device when it needs to be reset.
				event_send (EventTabNum, TABPANEL_TEXTBOX,sError,error_data,error_msg);
				excution_stop(error_mode);
				SetCtrlAttribute (panelHandle, PANEL_METHODOPTION, ATTR_DIMMED, ON);
				SetCtrlVal(panelErrMessage,PNL_ERRMSG_TEXTMSG,error_msg);
				DisplayPanel(panelErrMessage); 
				btb_status_change(STS_OUTOFSTATUS);
				break;
			case STS_OUTOFSTATUS:	// btb needs to be reset
			default:
				break;
		}

		SetCtrlVal(panelHandle,PANEL_BTBSTATUS,btb_status_string(BTBWorkingStatus));
		
		Delay(2);  // To-Do : Have to check this interval time
	}

	return 0;
}

void func_charging()
{
	if(methodexecution_flag)
	{
		TargetCurrentValue = operator.CfgCharge.ChargeCurrent;
		if(btb_option==0)
		{
			TargetVoltageValue = operator.CfgCharge.ChargeVoltage;
		}
		else
		{
			TargetVoltageValue = operator.CfgCharge.ChargeVoltage1CellFailed;
		}
		
		if(BTBPackVoltage < TargetVoltageValue)
		{
			charge_flag = ON;
		}
		else
		{
			excution_stop(AutoRun);
		}
	}
}

void func_discharging()
{
	if(methodexecution_flag)
	{
		TargetCurrentValue = operator.CfgDischarge.DischargeCurrent; 
		if(btb_option==0)
		{
			TargetVoltageValue = operator.CfgDischarge.DischargeVoltage;
		}
		else
		{
			TargetVoltageValue = operator.CfgDischarge.DischargeVoltage1CellFailed;
		}
		
		if(BTBPackVoltage > TargetVoltageValue)
		{
			discharge_flag = ON;
		}
		else
		{
			excution_stop(AutoRun);
		}
	}
}

void func_fullcharging()
{
	if(methodexecution_flag)
	{
		switch(FullCharge_WorkingStatus)
		{
			case Dispcheck_Fullcharge:
				if(cellDispersion >= (operator.CfgFullcharge.DispersionLimit1/mV))
				{
					event_send(EventTabNum, TABPANEL_TEXTBOX, AlarmON, Dispersion, MSG_OVDISPER);
					excution_stop(AutoRun);
					SetCtrlVal(panelErrMessage,PNL_ERRMSG_TEXTMSG,MSG_OVDISPER);
					DisplayPanel(panelErrMessage);
					btb_status_change(STS_ALARM3);
				}
				else
				{
					TargetCurrentValue = operator.CfgFullcharge.ChargeCurrent;
					if(btb_option==0)
					{
						TargetVoltageValue = operator.CfgFullcharge.ChargeVoltage;
					}
					else
					{
						TargetVoltageValue = operator.CfgFullcharge.ChargeVoltage1CellFailed;
					}
					
					FullCharge_WorkingStatus = Charging_Fullcharge; 
				}
				break;
			case Charging_Fullcharge:
				charge_flag = ON;
				TargetMode = MODE_CC;
				
				if(BTBPackVoltage >= TargetVoltageValue)
				{
					FullCharge_WorkingStatus = Tapering_Fullcharge;
				}	
				break;
			case Tapering_Fullcharge:
				#ifdef _MODE_CV_
				TargetMode = MODE_CV;
				if(BTBPackCurrent < operator.CfgFullcharge.ChargeCurrentLimit)
				{
					settingtimeStart = Timer();
					charge_flag = OFF;
					FullCharge_WorkingStatus = Waiting_Fullcharge;    //charge for 3hour
					TargetVoltageValue = 0;
					TargetCurrentValue = 0;
					TargetMode = MODE_CC;
				}
				#else
				if(BTBPackVoltage >= TargetVoltageValue)
				{
					TargetCurrentValue = TargetCurrentValue*operator.CfgFullcharge.TaperingFactor;
					ControlData.ChargeCurrent = TargetCurrentValue; // To-Do: delete it cause simulation
					if(TargetCurrentValue < operator.CfgFullcharge.ChargeCurrentLimit)
					{
						settingtimeStart = Timer();
						charge_flag = OFF;
						FullCharge_WorkingStatus = Waiting_Fullcharge;    //charge for 3hour
						TargetVoltageValue = 0;
						TargetCurrentValue = 0; 
					}
				}
				#endif
				break;
			case Waiting_Fullcharge:
				settingtimeElapsed =  (Timer()-settingtimeStart)/60;
				if(settingtimeElapsed>=operator.CfgFullcharge.SettlingTime)
				{
					FullCharge_WorkingStatus = End_Fullcharge;
				}
				break;
			case End_Fullcharge:
			default:
				excution_stop(AutoRun);
				break;
		}
	}
}
void func_cellbalancing()
{
	static int repeat_count = 0;

	if(methodexecution_flag)
	{
		switch(CellBalance_WorkingStatus)
		{
			case Dispcheck1_Cellbalance:
				if(cellDispersion >= (operator.CfgCellBalance.DispersionLimit1/mV))
				{
					event_send(EventTabNum, TABPANEL_TEXTBOX, AlarmON, Dispersion, MSG_OVDISPER);
					excution_stop(AutoRun);
					SetCtrlVal(panelErrMessage,PNL_ERRMSG_TEXTMSG,MSG_OVDISPER);
					DisplayPanel(panelErrMessage);
				}
				else
				{
					CellBalance_WorkingStatus = Charging1_Cellbalance; 
				}
				break;
			case Charging1_Cellbalance:
				charge_flag = ON;
				TargetMode = MODE_CC;
			
				TargetCurrentValue = operator.CfgCellBalance.ChargeCurrent;
				if(btb_option==0)
				{
					TargetVoltageValue = operator.CfgCellBalance.ChargeVoltage;
				}
				else
				{
					TargetVoltageValue = operator.CfgCellBalance.ChargeVoltage1CellFailed;
				}
				
				if(BTBPackVoltage >= TargetVoltageValue)
				{
					CellBalance_WorkingStatus = Tapering_Cellbalance;
				}	
				break;
			case Tapering_Cellbalance:
				#ifdef _MODE_CV_
				TargetMode = MODE_CV;
				if((BTBPackCurrent < operator.CfgCellBalance.ChargeCurrentLimit)&&(cellDispersion < (operator.CfgCellBalance.DispersionLimit2/mV))) 
				{
					settingtimeStart = Timer();
					CellBalance_WorkingStatus = Charging2_Cellbalance;    //charge for 3hour
				}
				else if((BTBPackCurrent < operator.CfgCellBalance.ChargeCurrentLimit)&&(cellDispersion >= (operator.CfgCellBalance.DispersionLimit2/mV)))
				{
					settingtimeStart = Timer();
					CellBalance_WorkingStatus = Waiting_Cellbalance; 	   //settling for 1hour
					charge_flag = OFF;
					TargetMode = MODE_CC;
				}
				#else
				if(BTBPackVoltage >= TargetVoltageValue)
				{
					TargetCurrentValue = TargetCurrentValue*operator.CfgCellBalance.TaperingFactor; // To-Do input tapering current;
					ControlData.ChargeCurrent = TargetCurrentValue; // To-Do: delete it cause simulation
					if((TargetCurrentValue < operator.CfgCellBalance.ChargeCurrentLimit)&&(cellDispersion < (operator.CfgCellBalance.DispersionLimit2/mV))) 
					{
						settingtimeStart = Timer();
						CellBalance_WorkingStatus = Charging2_Cellbalance;    //charge for 3hour
					}
					else if((TargetCurrentValue < operator.CfgCellBalance.ChargeCurrentLimit)&&(cellDispersion >= (operator.CfgCellBalance.DispersionLimit2/mV)))
					{
						settingtimeStart = Timer();
						CellBalance_WorkingStatus = Waiting_Cellbalance; 	   //settling for 1hour
						charge_flag = OFF;
					}
				}
				#endif
				break;
			case Charging2_Cellbalance:		//charge for 3hour
				settingtimeElapsed =  (Timer()-settingtimeStart)/60;
				if(settingtimeElapsed>=operator.CfgCellBalance.ChargingTime)
				{
					charge_flag = OFF;
					TargetVoltageValue = 0;
					TargetCurrentValue = 0;
					TargetMode = MODE_CC;
					settingtimeStart = Timer();
					CellBalance_WorkingStatus = Waiting_Cellbalance;
				}
				break;
			case Waiting_Cellbalance:	   //settling for 1hour
				settingtimeElapsed =  (Timer()-settingtimeStart)/60;
				if(settingtimeElapsed>=operator.CfgCellBalance.SettlingTime)
				{
					CellBalance_WorkingStatus = Dispcheck2_Cellbalance;
				}	 
				break;
			case Dispcheck2_Cellbalance:
				if(cellDispersion >= (operator.CfgCellBalance.DispersionLimit3/mV)) 
				{
					TargetCurrentValue = operator.CfgCellBalance.DischargeCurrent;
				
					if(btb_option==0)
					{
						TargetVoltageValue = operator.CfgCellBalance.DischargeVoltage;
					}
					else
					{
						TargetVoltageValue = operator.CfgCellBalance.DischargeVoltage1CellFailed;
					}
					CellBalance_WorkingStatus = Discharging_Cellbalance;
				}
				else
				{
					CellBalance_WorkingStatus = End_Cellbalance;
				}
				break;
			case Discharging_Cellbalance:
				if(BTBPackVoltage < TargetVoltageValue)
				{
					discharge_flag = OFF;
					TargetVoltageValue = 0;
					TargetCurrentValue = 0;
					if(repeat_count++ < operator.CfgCellBalance.RepeatCount)	//if(discharging_count++<operator.CfgEndOfStorage.RepeatCount)
					{
						CellBalance_WorkingStatus = Charging1_Cellbalance;
					}
					else
					{
						CellBalance_WorkingStatus = End_Cellbalance;
					}
				}
				else
				{
					discharge_flag = ON;
				}
				break;
			case End_Cellbalance:
				//event_send (EventTabNum, TABPANEL_TEXTBOX,ExcutionStop,AutoRun,NULL);
				excution_stop(AutoRun);
				break;
		}
	}
}

void func_endofstorage()
{
	static int discharging_count = 0;
	float interval;
	
	interval = 1;

	if(methodexecution_flag)
	{
		switch(EndOfStorage_WorkingStatus)
		{
			case Dispcheck_Endofstorage:
				if(cellDispersion >= (operator.CfgEndOfStorage.DispersionLimit1/mV))
				{
					event_send(EventTabNum, TABPANEL_TEXTBOX, AlarmON, Dispersion, MSG_OVDISPER);
					excution_stop(AutoRun);
					SetCtrlVal(panelErrMessage,PNL_ERRMSG_TEXTMSG,MSG_OVDISPER);
					DisplayPanel(panelErrMessage);
				}
				else
				{
					EndOfStorage_WorkingStatus = Charging_Endofstorage; 
					discharging_count = 0;
				}
				break;
			case Charging_Endofstorage:
				charge_flag = ON;
				TargetMode = MODE_CC;
			
				TargetCurrentValue = operator.CfgEndOfStorage.ChargeCurrent;
				if(btb_option==0)
				{
					TargetVoltageValue = operator.CfgEndOfStorage.ChargeVoltage;
				}
				else
				{
					TargetVoltageValue = operator.CfgEndOfStorage.ChargeVoltage1CellFailed;
				}
				
				if(BTBPackVoltage >= TargetVoltageValue)
				{
					EndOfStorage_WorkingStatus = Tapering_Endofstorage; 
				}
				break;
			case Tapering_Endofstorage:
				TargetMode = MODE_CV;
				
				#ifdef _MODE_CV_
				if(BTBPackCurrent < operator.CfgFullcharge.ChargeCurrentLimit)
				{
					charge_flag = OFF;
					TargetMode = MODE_CC;
					SetCtrlAttribute (panelHandle, PANEL_AMPEREHOUR, ATTR_DIMMED, OFF);
					SetCtrlAttribute (panelHandle, PANEL_RESISTANCE, ATTR_DIMMED, OFF);
					SetCtrlAttribute (panelHandle, PANEL_ENERGY, ATTR_DIMMED, OFF);
					ampereHour = 0;
					energy = 0;
					resistance = 0;

					EndOfStorage_WorkingStatus = Discharging_Endofstorage;    //charge for 3hour
				}
				#else
				if(BTBPackVoltage >= TargetVoltageValue)
				{
					TargetCurrentValue = TargetCurrentValue*operator.CfgFullcharge.TaperingFactor;; // To-Do Discharger Current Input
					ControlData.ChargeCurrent = TargetCurrentValue; // To-Do: delete it cause simulation
					if(TargetCurrentValue < operator.CfgFullcharge.ChargeCurrentLimit)
					{
						charge_flag = OFF;
						TargetMode = MODE_CC;
						SetCtrlAttribute (panelHandle, PANEL_AMPEREHOUR, ATTR_DIMMED, OFF);
						SetCtrlAttribute (panelHandle, PANEL_RESISTANCE, ATTR_DIMMED, OFF);
						SetCtrlAttribute (panelHandle, PANEL_ENERGY, ATTR_DIMMED, OFF);
						ampereHour = 0;
						energy = 0;
						resistance = 0;

						EndOfStorage_WorkingStatus = Discharging_Endofstorage;    //charge for 3hour
					}
				}
				#endif
				break;
			case Discharging_Endofstorage:
				TargetCurrentValue = operator.CfgEndOfStorage.DischargeCurrent;
				
				ampereHour += (TargetCurrentValue * (interval/3600));
				energy += (TargetCurrentValue * (interval/3600) * BTBPackVoltage);
				resistance = BTBPackVoltage / BTBPackCurrent;
				discharge_flag = ON;
				//TargetVoltageValue = operator.CfgEndOfStorage.DischargeCellVoltageLimit;
				
				if(dispMinValue <= operator.CfgEndOfStorage.DischargeCellVoltageLimit)
				{
					discharge_flag = OFF;
					if(discharging_count++<operator.CfgEndOfStorage.RepeatCount)
					{
						charge_flag = OFF;
						discharge_flag = OFF;
						TargetVoltageValue = 0;
						TargetCurrentValue = 0;
						
						EndOfStorage_WorkingStatus = Charging_Endofstorage;	
					}
					else
					{
						discharging_count = 0;
						SetCtrlAttribute (panelHandle, PANEL_AMPEREHOUR, ATTR_DIMMED, ON);
						SetCtrlAttribute (panelHandle, PANEL_RESISTANCE, ATTR_DIMMED, ON);
						SetCtrlAttribute (panelHandle, PANEL_ENERGY, ATTR_DIMMED, ON);
						EndOfStorage_WorkingStatus = End_Endofstorage;	
					}
					ampereHour = 0;
					energy = 0;
					resistance = 0;
				}
				break;
			case End_Endofstorage:
			default:
				excution_stop(AutoRun);
				ampereHour = 0;
				energy = 0;
				resistance = 0;
				break;	
		}
	}
}

int CVICALLBACK ControlHandlingThread (void *functionData)
{
	char destAddr[MAX_IP_INPUT_LENGTH + 1] = ""; 
	int ret;
	
	strcpy(destAddr, IFBOX_IP);

	while(1)
	{
		if(connection_flag)
		{
			ret = UDPWrite(writerChannel, WRITER_PORT, destAddr, &ControlData, sizeof(ControlData) );
			if(ret < 0)
			{
				if(BTBWorkingStatus < STS_ERROR)
				{
					SetErrorMessage(MSG_ERROR16,ret,AutoRun);
					btb_status_change(STS_ERROR);
				}
			}
		}
		Delay(1);  // To-Do : Have to check the Delay time=Interval time
	}
	return 0;
}

void excution_stop(int runningmode)
{
	charge_flag = OFF;
	discharge_flag = OFF;
	TargetVoltageValue = 0;
	TargetCurrentValue = 0;

	ch_device_off();
	
	if(methodexecution_flag)
	{
		event_send (EventTabNum, TABPANEL_TEXTBOX,ExcutionStop,runningmode,NULL);
	}	
	
	if(BTBWorkingStatus < STS_DISCONNECT)
	{
		filesave_autoexecute = OFF;
		if(!filesave_manualexecute)	
		{
			SetCtrlVal(panelHandle, PANEL_FILESAVEBT, OFF);
		}
		btb_status_change(STS_STANDBY);
	}
	
	SetCtrlAttribute (panelHandle, PANEL_FILESAVEBT, ATTR_DIMMED, OFF);
	SetCtrlAttribute (panelHandle, PANEL_METHODOPTION, ATTR_DIMMED, OFF);
	Radio_SetMarkedOption (panelHandle, PANEL_METHODOPTION, 0);
	SetCtrlAttribute (panelHandle, PANEL_EXECUTION, ATTR_DIMMED, ON);
	SetCtrlAttribute (panelHandle, PANEL_CELLUSABILITY, ATTR_DIMMED, OFF);
	SetCtrlVal(panelHandle,PANEL_EXECUTION,OFF);

	methodexecution_flag = OFF;
	
	FullCharge_WorkingStatus = Dispcheck_Fullcharge;
	CellBalance_WorkingStatus = Dispcheck1_Cellbalance;
	EndOfStorage_WorkingStatus = Dispcheck_Endofstorage;

}

int CVICALLBACK ControlAllDeviceThread(void *functionData)
{
	static int charger_active = OFF;
	static int discharger_active = OFF;

	while(1)
	{	
		if(connection_flag)
		{
			/*** Charger & Discharger ON/FF related to Relay ***/
			if(device_check_flag)
			{	
				if(charge_flag == ON)
				{
					ControlData.intf_ctrl |= CHARGER_BIT;
					ControlData.ChargeCurrent = TargetCurrentValue;
					if(charger_active == OFF)
					{
						if(charger_on())
						{
							charger_active = ON;
							SetCtrlVal(panelHandle,PANEL_CHARGER_LED,ON);
							SetCtrlAttribute (panelHandle, PANEL_CH_MODE, ATTR_VISIBLE, ON);
						}
						else
						{
							if(BTBWorkingStatus < STS_ERROR)
							{
								SetErrorMessage(MSG_ERROR8,NULL,AutoRun);
								btb_status_change(STS_ERROR);
								device_check_flag = OFF;
							}
						}
					}
					else
					{
						display_charger_measurment();
					}
				}
				else if(charge_flag == OFF)
				{
					ControlData.intf_ctrl &= ~CHARGER_BIT;
					ControlData.ChargeCurrent = 0;
					if(charger_active == ON)
					{
						if(charger_off())
						{
							charger_active = OFF;
							SetCtrlVal(panelHandle,PANEL_CHARGER_LED,OFF);
							SetCtrlAttribute (panelHandle, PANEL_CH_MODE, ATTR_VISIBLE, OFF);
						}
						else
						{
							if(BTBWorkingStatus < STS_ERROR)
							{
								SetErrorMessage(MSG_ERROR10,NULL,AutoRun);
								btb_status_change(STS_ERROR);
								device_check_flag = OFF;
							}
						}
					}
				}

				if(discharge_flag == ON)
				{
					ControlData.intf_ctrl |= DISCHARGER_BIT;
					ControlData.DischargeCurrent = TargetCurrentValue;
					if(discharger_active == OFF)
					{
						if(discharger_on())
						{
							discharger_active = ON;
							SetCtrlVal(panelHandle,PANEL_DISCHARGER_LED,ON);
						}
						else
						{
							if(BTBWorkingStatus < STS_ERROR)
							{
								SetErrorMessage(MSG_ERROR9,NULL,AutoRun);
								btb_status_change(STS_ERROR);
								device_check_flag = OFF;
							}
						}
					}
					else
					{
						display_discharger_measurment();
					}

				}
				else if(discharge_flag == OFF)
				{
					ControlData.intf_ctrl &= ~DISCHARGER_BIT;
					ControlData.DischargeCurrent = 0;
					if(discharger_active == ON)
					{
						if(discharger_off())
						{
							discharger_active = OFF;
							SetCtrlVal(panelHandle,PANEL_DISCHARGER_LED,OFF);
						}
						else
						{
							if(BTBWorkingStatus < STS_ERROR)
							{
								SetErrorMessage(MSG_ERROR11,NULL,AutoRun);
								btb_status_change(STS_ERROR);
								device_check_flag = OFF;
							}
						}
					}
				}

				
#ifdef DEVICE_CHECK
				/* checking to compare pack voltage<->device voltage & pack current <-> device current */
				if(charger_active)
					charger_checking();

				if(discharger_active)
					discharger_checking();
				
				/* checking to compare target current<->device current */
				if(charger_active)
					charger_checking_target_current();

				if(discharger_active)
					discharger_checking_target_current();

#endif	
			}

		}
		
		Delay(0.5);
	}
	return 0;
}

int CVICALLBACK RelayCheckingThread (void *functionData)
{
	static int charger_failed_cnt = 0;
	static int discharger_failed_cnt = 0;
	static int ch_relay_previous = 0;
	static int disch_relay_previous = 0;

	while(1)
	{	
		if(connection_flag)
		{
			// To-Do: change relay pictures...
			/* Charger & Discharger Relay ON/OFF UI */
			if(charger_relay)
			{
				picture_charger_relay_on();
				//SetCtrlAttribute (panelHandle, PANEL_PICTURE_ALLOFF, ATTR_VISIBLE, OFF);
				//SetCtrlAttribute (panelHandle, PANEL_PICTURE_CHARGEON, ATTR_VISIBLE, ON);
				//SetCtrlAttribute (panelHandle, PANEL_PICTURE_DISCHARGEON, ATTR_VISIBLE, OFF);
			}
			else
			{
				picture_charger_relay_off();
				//SetCtrlAttribute (panelHandle, PANEL_PICTURE_ALLOFF, ATTR_VISIBLE, ON);
				//SetCtrlAttribute (panelHandle, PANEL_PICTURE_CHARGEON, ATTR_VISIBLE, OFF);
				//SetCtrlAttribute (panelHandle, PANEL_PICTURE_DISCHARGEON, ATTR_VISIBLE, OFF);
			}

			if(discharger_relay)
			{
				picture_discharger_relay_on();
				//SetCtrlAttribute (panelHandle, PANEL_PICTURE_ALLOFF, ATTR_VISIBLE, OFF);
				//SetCtrlAttribute (panelHandle, PANEL_PICTURE_CHARGEON, ATTR_VISIBLE, OFF);
				//SetCtrlAttribute (panelHandle, PANEL_PICTURE_DISCHARGEON, ATTR_VISIBLE, ON);
			}
			else
			{
				picture_discharger_relay_off();
				//SetCtrlAttribute (panelHandle, PANEL_PICTURE_ALLOFF, ATTR_VISIBLE, ON);
				//SetCtrlAttribute (panelHandle, PANEL_PICTURE_CHARGEON, ATTR_VISIBLE, OFF);
				//SetCtrlAttribute (panelHandle, PANEL_PICTURE_DISCHARGEON, ATTR_VISIBLE, OFF);
			}

			/* Charger & Discharger Relay ON/OFF Event */
			if(charger_relay != ch_relay_previous)
			{
				if(charger_relay)
				{
					event_send (EventTabNum, TABPANEL_TEXTBOX,RelayON,ChargerRelay,NULL);
				}
				else
				{
					event_send (EventTabNum, TABPANEL_TEXTBOX,RelayOFF,ChargerRelay,NULL);
				}
				ch_relay_previous = charger_relay;
			}

			if(discharger_relay != disch_relay_previous)
			{
				if(discharger_relay)
				{
					event_send (EventTabNum, TABPANEL_TEXTBOX,RelayON,DischargerRelay,NULL);
				}
				else
				{
					event_send (EventTabNum, TABPANEL_TEXTBOX,RelayOFF,DischargerRelay,NULL);
				}
				disch_relay_previous = discharger_relay;
			}

			/* checking abnormal activation on Charger Relay */
			if(charge_flag != charger_relay)
			{
				if(charger_failed_cnt++ > 5)
				{
					if(charge_flag)
					{
						if(BTBWorkingStatus < STS_ERROR)
						{
							SetErrorMessage(MSG_ERROR8,NULL,AutoRun);
							btb_status_change(STS_ERROR);
							device_check_flag = OFF;
						}		
					}
					else
					{
						if(BTBWorkingStatus < STS_ERROR)
						{
							SetErrorMessage(MSG_ERROR10,NULL,AutoRun);
							btb_status_change(STS_ERROR);
							device_check_flag = OFF;
						}		
					}
				}
			}
			else
			{
				charger_failed_cnt = 0;
			}

			/* checking abnormal activation on Discharger Relay */
			if(discharge_flag != discharger_relay)
			{
				if(discharger_failed_cnt++ > 5)
				{
					if(discharge_flag)
					{
						if(BTBWorkingStatus < STS_ERROR)
						{
							SetErrorMessage(MSG_ERROR9,NULL,AutoRun);
							btb_status_change(STS_ERROR);
							device_check_flag = OFF;
						}	
					}
					else
					{
						if(BTBWorkingStatus < STS_ERROR)
						{
							SetErrorMessage(MSG_ERROR11,NULL,AutoRun);
							btb_status_change(STS_ERROR);
							device_check_flag = OFF;
						}	
					}
				}
			}
			else
			{
				discharger_failed_cnt = 0;
			}
			
		}
		
		Delay(1);
	}
	return 0;
}

int CVICALLBACK IFBoxConnectCheckingThread (void *functionData)
{
	while(1)
	{	
		if(connection_flag)
		{
			if(BTBWorkingStatus != STS_ERROR)
			{
				timeElapsed = Timer() - timeStartBTB;
				if(timeElapsed > CONNECTION_LIMIT_TIME)
				{
					#if 1
					if(BTBWorkingStatus < STS_ERROR)
					{
						/***** Connection Error *****/
						connection_flag = OFF;
						SetCtrlVal (panelHandle, PANEL_CONNECTED, OFF);
						SetErrorMessage(MSG_ERROR7,NULL,AutoRun);
						btb_status_change(STS_ERROR);
					}
					#endif
				}
			}
		}
		Delay(1);
	}
	return 0;
}

void picture_all_relay_off()
{
	SetCtrlAttribute (panelHandle, PANEL_P_CH_RELAY_ON, ATTR_VISIBLE, OFF);
	SetCtrlAttribute (panelHandle, PANEL_P_DISCH_RELAY_ON, ATTR_VISIBLE, OFF);
	SetCtrlAttribute (panelHandle, PANEL_P_CH_RELAY_OFF, ATTR_VISIBLE, ON);
	SetCtrlAttribute (panelHandle, PANEL_P_DISCH_RELAY_OFF, ATTR_VISIBLE, ON);
}

void picture_charger_relay_on()
{
	SetCtrlAttribute (panelHandle, PANEL_P_CH_RELAY_OFF, ATTR_VISIBLE, OFF);
	SetCtrlAttribute (panelHandle, PANEL_P_CH_RELAY_ON, ATTR_VISIBLE, ON);
}

void picture_charger_relay_off()
{
	SetCtrlAttribute (panelHandle, PANEL_P_CH_RELAY_ON, ATTR_VISIBLE, OFF);
	SetCtrlAttribute (panelHandle, PANEL_P_CH_RELAY_OFF, ATTR_VISIBLE, ON);
}

void picture_discharger_relay_on()
{
	SetCtrlAttribute (panelHandle, PANEL_P_DISCH_RELAY_OFF, ATTR_VISIBLE, OFF);
	SetCtrlAttribute (panelHandle, PANEL_P_DISCH_RELAY_ON, ATTR_VISIBLE, ON);
}

void picture_discharger_relay_off()
{
	SetCtrlAttribute (panelHandle, PANEL_P_DISCH_RELAY_ON, ATTR_VISIBLE, OFF);
	SetCtrlAttribute (panelHandle, PANEL_P_DISCH_RELAY_OFF, ATTR_VISIBLE, ON);
}
