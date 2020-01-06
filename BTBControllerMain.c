/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FILE:    BTBControllerMain.c                                              */
/*                                                                           */
/* PURPOSE: This main functions to handle BTB System on UI Labwindows        */
/*          control.                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Include files                                                             */
/*---------------------------------------------------------------------------*/
#include <udpsupp.h>
#include <ansi_c.h>
#include "radioGroup.h"
#include <utility.h>
#include <tcpsupp.h>
#include <cvirte.h>		
#include <userint.h>
#include "BTBController.h"
#include "define.h"

ConfigValue operator;	// When we use it for operator's data
ConfigValue btbdefault;	// 

/*---------------------------------------------------------------------------*/
/* This is the application's entry-point.                                    */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Main Function                                                             */	
/*---------------------------------------------------------------------------*/

int main (int argc, char *argv[])
{
	int ret;
	
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	
	if ((panelHandle = LoadPanel (0, "BTBController.uir", PANEL)) < 0)
		goto Error;
	
	if ((panelCFGCharging = LoadPanel(0, "BTBController.uir", PNL_M1)) < 0)
		goto Error;	
	
	if ((panelCFGDischarging = LoadPanel(0, "BTBController.uir", PNL_M2)) < 0)
		goto Error;	
	
	if ((panelCFGFullcharging = LoadPanel(0, "BTBController.uir", PNL_M3)) < 0)
		goto Error;
	
	if ((panelCFGCellbalancing = LoadPanel(0, "BTBController.uir", PNL_M4)) < 0)
		goto Error;

	if ((panelCFGEndofstorage = LoadPanel(0, "BTBController.uir", PNL_M5)) < 0)
		goto Error;

	if ((panelCFGEndofstorage = LoadPanel(0, "BTBController.uir", PNL_M5)) < 0)
		goto Error;
	
	if ((panelInterval = LoadPanel(0, "BTBController.uir", PNL_M6)) < 0) 
		goto Error;
	
	if ((panelModelList = LoadPanel(0, "BTBController.uir", PNL_M7)) < 0) 
		goto Error;
	
	if ((panelOperator = LoadPanel(0, "BTBController.uir", PNL_M8)) < 0) 
		goto Error;

 	if ((panelTemperature = LoadPanel(0, "BTBController.uir", PNL_TEMP)) < 0) 
		goto Error;

	if ((panelCellVoltageLimit = LoadPanel(0, "BTBController.uir", PNL_CELLS)) < 0) 
		goto Error;
	
	if ((panelDecimalPoint = LoadPanel(0, "BTBController.uir", PNL_DECI)) < 0) 
		goto Error;	

	if ((panelErrMessage = LoadPanel(0, "BTBController.uir", PNL_ERRMSG)) < 0) 
		goto Error;	

	/* set up the BTB System with getting first setting values */
	btb_status_change(STS_GETVALUE);
	
	/* Set-up UPD Read/Writer */

	ret = CreateUDPChannelConfig(READER_PORT, BTBSYSTEM_IP, 0, UDPReader, NULL, &readerChannel);

	if(ret < 0)
	{
		event_send (EventTabNum, TABPANEL_TEXTBOX,sError,NULL,MSG_ERROR1);
		MessagePopup("UDP Receiver", MSG_ERROR1);
		goto Error;
	}
	
	ret = CreateUDPChannel(UDP_ANY_LOCAL_PORT, &writerChannel);
	if(ret < 0)
	{
		event_send (EventTabNum, TABPANEL_TEXTBOX,sError,NULL,MSG_ERROR1);
		MessagePopup("UDP Sender", MSG_ERROR1);
		goto Error;
	}

	/* Create Thread Locking Handler for data integrity */
	CmtNewLock ("", 0, &gLockRandom);

	/*** Handling BTB Control System ***/
	CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, BTBWorkingThread, NULL, NULL);
	
	/*** Handling BTB Control System ***/
	CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, ControlHandlingThread, NULL, NULL); 
	
	/*** Checking the connection of I/F Box ***/
	CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, IFBoxConnectCheckingThread, NULL, NULL);	
	
	/*** Checking Alarm Situation ***/
	CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, BTBCheckingThread, NULL, NULL);	

	/*** Handling Charger & Discharger ***/
	CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, ControlAllDeviceThread, NULL, NULL);	
	
	/*** Checking File Save ***/
	CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, FileSaveThread, NULL, NULL);
	
	/*** Display UI ***/
	CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, UIDisplayThread, NULL, NULL);

	/*** Pack Votage graph ***/
	CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, graph_packvoltageThread, NULL, NULL);	

	/*** Cell Votage graph ***/
	CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, graph_cellvoltageThread, NULL, NULL);

	/*** Checking Relays ***/
	CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, RelayCheckingThread, NULL, NULL);

	DisplayPanel (panelHandle);
	RunUserInterface();

#ifdef SAVE_USER_INI
	char CurrentDir[MAX_PATHNAME_LEN];
	char iniFile[MAX_PATHNAME_LEN];
	GetProjectDir(CurrentDir);
	sprintf(iniFile,"%s\\ini\\operator_config.ini",CurrentDir);
	ini_save(iniFile);					//	To save user's configurations
#endif
	
Error:  
	DiscardPanel (panelHandle);
	DiscardPanel (panelCFGCharging);
	DiscardPanel (panelCFGDischarging);
    DiscardPanel (panelCFGFullcharging);
	DiscardPanel (panelCFGCellbalancing);
    DiscardPanel (panelCFGEndofstorage);
	DiscardPanel (panelInterval);
	DiscardPanel (panelModelList);
	DiscardPanel (panelOperator);
	DiscardPanel (panelTemperature);
	DiscardPanel (panelCellVoltageLimit);
	DiscardPanel (panelDecimalPoint);
	DiscardPanel (panelErrMessage);
	CmtDiscardLock (gLockRandom);
	ch_device_close();
	return 0;
}

int CVICALLBACK cmdMainPNLExit (int panel, int event, void *callbackData,
						  int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			 QuitUserInterface (0);
			break;
	}
	return 0;
}


/*---------------------------------------------------------------------------*/
/* Display & Set Value for Configuration                                     */	
/*---------------------------------------------------------------------------*/
						  
	/***** Charge *****/
int CVICALLBACK cmdSetValueChargeCB (int panel, int control, int event,
									 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			DisplayPanel(panelCFGCharging); 
			break;
		case EVENT_CLOSE:
			break;
	}
	return 0;
}
void CVICALLBACK cmdSetValueChargeCBM(int menubar, int menuItem, void *callbackData, int panel) 
{
	DisplayPanel(panelCFGCharging); 
}

	/***** Discharge *****/ 
int CVICALLBACK cmdSetValueDischargeCB (int panel, int control, int event,
										void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			DisplayPanel(panelCFGDischarging);
			break;
	}
	return 0;
}
void CVICALLBACK cmdSetValueDischargeCBM(int menubar, int menuItem, void *callbackData, int panel) 
{
	DisplayPanel(panelCFGDischarging); 
}

	/***** Full charge *****/ 
int CVICALLBACK cmdSetValueFullChargeCB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			DisplayPanel(panelCFGFullcharging);
			break;
	}
	return 0;
}
void CVICALLBACK cmdSetValueFullChargeCBM(int menubar, int menuItem, void *callbackData, int panel) 
{
	DisplayPanel(panelCFGFullcharging); 
}

	/***** Cell Balancing *****/ 
int CVICALLBACK cmdSetValueCellBalanceCB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			DisplayPanel(panelCFGCellbalancing);
			break;
	}
	return 0;
}
void CVICALLBACK cmdSetValueCellBalanceCBM(int menubar, int menuItem, void *callbackData, int panel) 
{
	DisplayPanel(panelCFGCellbalancing); 
}

	/***** End Of Storage *****/ 
int CVICALLBACK cmdSetValueEndOfStorageCB (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			 DisplayPanel(panelCFGEndofstorage);
			break;
	}
	return 0;
}
void CVICALLBACK cmdSetValueEndOfStorageCBM(int menubar, int menuItem, void *callbackData, int panel) 
{
	DisplayPanel(panelCFGEndofstorage); 
}


/*---------------------------------------------------------------------------*/
/* Option & Execution Battery Function                                       */	
/*---------------------------------------------------------------------------*/
int CVICALLBACK cmdOptionCB (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal (panelHandle, PANEL_CELLUSABILITY, &btb_option);
			operator.BatteryCellOption = btb_option;
			panel_setting_value();
			//SetCtrlVal (panelCFGCharging, PNL_NUMERIC, &VoltageValue);  
			break;
	}
	return 0;
}

int CVICALLBACK cmdExecutionCB (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	int excutionValue;
	switch (event)
	{
		case EVENT_COMMIT:
			 GetCtrlVal (panelHandle, PANEL_EXECUTION, &excutionValue);
			 if(excutionValue==ON)
			 {
			 	methodexecution_flag = ON;
				btb_status_change(STS_STANDBY+OptionValue);
				SetCtrlAttribute (panelHandle, PANEL_METHODOPTION, ATTR_DIMMED, ON);
				SetCtrlAttribute (panelHandle, PANEL_CELLUSABILITY, ATTR_DIMMED, ON);
				filesave_autoexecute = ON;
				event_send (EventTabNum, TABPANEL_TEXTBOX,ExcutionStart,ManualRun,NULL);
				if(!filesave_manualexecute)	
				{
					SetCtrlVal(panelHandle, PANEL_FILESAVEBT, ON);
				}
				SetCtrlAttribute (panelHandle, PANEL_FILESAVEBT, ATTR_DIMMED, ON);
			 }
			 else
			 {
				excution_stop(ManualRun);
			 }
			break;
	}
	return 0;
}

/*---------------------------------------------------------------------------*/
/* Initialize all Configurations                                             */	
/*---------------------------------------------------------------------------*/
	/***** Charge *****/  
int CVICALLBACK cmdM1SaveCB (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	double VoltageValue;
	double CurrentValue;

	switch (event)
	{
		case EVENT_COMMIT:

			GetCtrlVal (panelCFGCharging, PNL_M1_TARGET_VOLTAGE, &VoltageValue);
			GetCtrlVal (panelCFGCharging, PNL_M1_TARGET_CURRENT, &CurrentValue);
			
			if(btb_option == 0)
			{
				operator.CfgCharge.ChargeVoltage = VoltageValue;	
			}
			else
			{
				operator.CfgCharge.ChargeVoltage1CellFailed = VoltageValue;
			}			
			operator.CfgCharge.ChargeCurrent = CurrentValue;
			
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
			
			HidePanel(panelCFGCharging);
			break;
	}
	return 0;
}

int CVICALLBACK cmdM1ResetCB (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(btb_option == 0)
			{
				SetCtrlVal (panelCFGCharging, PNL_M1_TARGET_VOLTAGE, btbdefault.CfgCharge.ChargeVoltage);	
			}
			else
			{
				SetCtrlVal (panelCFGCharging, PNL_M1_TARGET_VOLTAGE, btbdefault.CfgCharge.ChargeVoltage1CellFailed);
			}
			SetCtrlVal (panelCFGCharging, PNL_M1_TARGET_CURRENT, btbdefault.CfgCharge.ChargeCurrent);
			
 			if(btbdefault.CfgCharge.ChargeCurrent == 9.0)
			{
				SetCtrlVal (panelCFGCharging, PNL_M1_RING, 0);
			}
			else if(btbdefault.CfgCharge.ChargeCurrent == 18.0)
			{
				SetCtrlVal (panelCFGCharging, PNL_M1_RING, 1);
			}
			else
			{
				SetCtrlVal (panelCFGCharging, PNL_M1_RING, 2);
			}
			
			operator.CfgCharge.ChargeVoltage = btbdefault.CfgCharge.ChargeVoltage;
			operator.CfgCharge.ChargeVoltage1CellFailed = btbdefault.CfgCharge.ChargeVoltage1CellFailed;
			operator.CfgCharge.ChargeCurrent = btbdefault.CfgCharge.ChargeCurrent;

			HidePanel(panelCFGCharging);
			break;
	}
	return 0;
}

int CVICALLBACK cmdM1CurrentSelecCB (int panel, int control, int event,
									 void *callbackData, int eventData1, int eventData2)
{
	int flag;
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal (panelCFGCharging, PNL_M1_RING, &flag);
			switch(flag)
			{
				case 0:
					SetCtrlVal (panelCFGCharging, PNL_M1_TARGET_CURRENT, 9.0);		// C/10(9A)
					SetCtrlAttribute (panelCFGCharging, PNL_M1_TARGET_CURRENT, ATTR_DIMMED, ON); 
					break;
				case 1:	
					SetCtrlVal (panelCFGCharging, PNL_M1_TARGET_CURRENT, 18.0);		// C/5(18A)
					SetCtrlAttribute (panelCFGCharging, PNL_M1_TARGET_CURRENT, ATTR_DIMMED, ON);
					break;					
				case 2:
					SetCtrlAttribute (panelCFGCharging, PNL_M1_TARGET_CURRENT, ATTR_DIMMED, OFF);
				default:
				break;
			}
	}
	return 0;
}

int CVICALLBACK cmdM1CancelCB (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(btb_option == 0)
			{
				SetCtrlVal (panelCFGCharging, PNL_M1_TARGET_VOLTAGE, operator.CfgCharge.ChargeVoltage);	
			}
			else
			{
				SetCtrlVal (panelCFGCharging, PNL_M1_TARGET_VOLTAGE, operator.CfgCharge.ChargeVoltage1CellFailed);
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
			
			HidePanel(panelCFGCharging);
			break;
	}
	return 0;
}

int CVICALLBACK PNL_M1CBExit (int panelHandle, int event, void *callbackData,
							  int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_CLOSE:
			HidePanel(panelCFGCharging);
			break;
	}
	return 0;
}

	/***** Discharge *****/ 
 int CVICALLBACK cmdM2SaveCB (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	double VoltageValue;
	double CurrentValue;
	
	switch (event)
	{
		case EVENT_COMMIT:

			GetCtrlVal (panelCFGDischarging, PNL_M2_TARGET_VOLTAGE, &VoltageValue);
			GetCtrlVal (panelCFGDischarging, PNL_M2_TARGET_CURRENT, &CurrentValue);
			if(btb_option == 0)
			{
				operator.CfgDischarge.DischargeVoltage = VoltageValue;	
			}
			else
			{
				operator.CfgDischarge.DischargeVoltage1CellFailed = VoltageValue;
			}			
			operator.CfgDischarge.DischargeCurrent =  CurrentValue;
			
			if(operator.CfgDischarge.DischargeCurrent == 30.0)
			{
				SetCtrlVal (panelCFGDischarging, PNL_M2_RING, 0);
			}
			else
			{
				SetCtrlVal (panelCFGDischarging, PNL_M2_RING, 1);
			}
			HidePanel(panelCFGDischarging); 
			break;
	}
	return 0;
}

int CVICALLBACK cmdM2ResetCB (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(btb_option == 0)
			{
				SetCtrlVal (panelCFGDischarging, PNL_M2_TARGET_VOLTAGE, btbdefault.CfgDischarge.DischargeVoltage);	
			}
			else
			{
				SetCtrlVal (panelCFGDischarging, PNL_M2_TARGET_VOLTAGE, btbdefault.CfgDischarge.DischargeVoltage1CellFailed);
			}
			SetCtrlVal (panelCFGDischarging, PNL_M2_TARGET_CURRENT, btbdefault.CfgDischarge.DischargeCurrent);
			
			if(btbdefault.CfgDischarge.DischargeCurrent == 30.0)
			{
				SetCtrlVal (panelCFGDischarging, PNL_M2_RING, 0);
			}
			else
			{
				SetCtrlVal (panelCFGDischarging, PNL_M2_RING, 1);
			}

			operator.CfgDischarge.DischargeVoltage = btbdefault.CfgDischarge.DischargeVoltage;
			operator.CfgDischarge.DischargeVoltage1CellFailed = btbdefault.CfgDischarge.DischargeVoltage1CellFailed;
			operator.CfgDischarge.DischargeCurrent = btbdefault.CfgDischarge.DischargeCurrent;

			HidePanel(panelCFGDischarging);
			break;
	}
	return 0;
}

int CVICALLBACK cmdM2CancelCB (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(btb_option == 0)
			{
				SetCtrlVal (panelCFGDischarging, PNL_M2_TARGET_VOLTAGE, operator.CfgDischarge.DischargeVoltage);	
			}
			else
			{
				SetCtrlVal (panelCFGDischarging, PNL_M2_TARGET_VOLTAGE, operator.CfgDischarge.DischargeVoltage1CellFailed);
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
			HidePanel(panelCFGDischarging);  
			break;
	}
	return 0;
}

int CVICALLBACK cmdM2CurrentSelecCB (int panel, int control, int event,
									 void *callbackData, int eventData1, int eventData2)
{
	int flag;
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal (panelCFGDischarging, PNL_M2_RING, &flag);
			switch(flag)
			{
				case 0:
					SetCtrlVal (panelCFGDischarging, PNL_M2_TARGET_CURRENT, 30.0);
					SetCtrlAttribute (panelCFGDischarging, PNL_M2_TARGET_CURRENT, ATTR_DIMMED, ON); 
					break;
				case 1:
					SetCtrlAttribute (panelCFGDischarging, PNL_M2_TARGET_CURRENT, ATTR_DIMMED, OFF);
				default:
				break;
			}
	}
	return 0;
}
int CVICALLBACK PNL_M2CBExit (int panelHandle, int event, void *callbackData,
							  int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_CLOSE:
			HidePanel(panelCFGDischarging);
			break;
	}
	return 0;
}

		/***** Full-charge *****/  
 int CVICALLBACK cmdM3SaveCB (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	double VoltageValue;
	double CurrentValue;
	double TaperingFactorValue;
	double ChargeCurrentLimitValue;
	double SettlingTimeValue;
	switch (event)
	{
		case EVENT_COMMIT:

			GetCtrlVal (panelCFGFullcharging, PNL_M3_TARGET_VOLTAGE, &VoltageValue);
			GetCtrlVal (panelCFGFullcharging, PNL_M3_TARGET_CURRENT, &CurrentValue);
			GetCtrlVal (panelCFGFullcharging, PNL_M3_TAPERING, &TaperingFactorValue);
			GetCtrlVal (panelCFGFullcharging, PNL_M3_END_CURRENT, &ChargeCurrentLimitValue);
			GetCtrlVal (panelCFGFullcharging, PNL_M3_SETTLING_TIME, &SettlingTimeValue);
			
			if(btb_option == 0)
			{
				operator.CfgFullcharge.ChargeVoltage = VoltageValue;	
			}
			else
			{
				operator.CfgFullcharge.ChargeVoltage1CellFailed = VoltageValue;
			}			
			operator.CfgFullcharge.ChargeCurrent =  CurrentValue;
			operator.CfgFullcharge.TaperingFactor = TaperingFactorValue;
			operator.CfgFullcharge.ChargeCurrentLimit = ChargeCurrentLimitValue;
			operator.CfgFullcharge.SettlingTime = SettlingTimeValue;
			HidePanel(panelCFGFullcharging);
			break;
	}
	return 0;
}
int CVICALLBACK cmdM3CancelCB (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(btb_option == 0)
			{
				SetCtrlVal (panelCFGFullcharging, PNL_M3_TARGET_VOLTAGE, operator.CfgFullcharge.ChargeVoltage);	
			}
			else
			{
				SetCtrlVal (panelCFGFullcharging, PNL_M3_TARGET_VOLTAGE, operator.CfgFullcharge.ChargeVoltage1CellFailed);
			}
			SetCtrlVal (panelCFGFullcharging, PNL_M3_TARGET_CURRENT, operator.CfgFullcharge.ChargeCurrent);
			SetCtrlVal (panelCFGFullcharging, PNL_M3_TAPERING, operator.CfgFullcharge.TaperingFactor);
			SetCtrlVal (panelCFGFullcharging, PNL_M3_END_CURRENT, operator.CfgFullcharge.ChargeCurrentLimit);
			SetCtrlVal (panelCFGFullcharging, PNL_M3_SETTLING_TIME, operator.CfgFullcharge.SettlingTime);			
			HidePanel(panelCFGFullcharging);
			break;
	}
	return 0;
}

int CVICALLBACK cmdM3ResetCB (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(btb_option == 0)
			{
				SetCtrlVal (panelCFGFullcharging, PNL_M3_TARGET_VOLTAGE, btbdefault.CfgFullcharge.ChargeVoltage);	
			}
			else
			{
				SetCtrlVal (panelCFGFullcharging, PNL_M3_TARGET_VOLTAGE, btbdefault.CfgFullcharge.ChargeVoltage1CellFailed);
			}
			SetCtrlVal (panelCFGFullcharging, PNL_M3_TARGET_CURRENT, btbdefault.CfgFullcharge.ChargeCurrent);
			SetCtrlVal (panelCFGFullcharging, PNL_M3_TAPERING, btbdefault.CfgFullcharge.TaperingFactor);
			SetCtrlVal (panelCFGFullcharging, PNL_M3_END_CURRENT, btbdefault.CfgFullcharge.ChargeCurrentLimit);
			SetCtrlVal (panelCFGFullcharging, PNL_M3_SETTLING_TIME, btbdefault.CfgFullcharge.SettlingTime);

			operator.CfgFullcharge.ChargeVoltage = btbdefault.CfgFullcharge.ChargeVoltage;
			operator.CfgFullcharge.ChargeVoltage1CellFailed = btbdefault.CfgFullcharge.ChargeVoltage1CellFailed;
			operator.CfgFullcharge.TaperingFactor = btbdefault.CfgFullcharge.TaperingFactor;
			operator.CfgFullcharge.ChargeCurrent = btbdefault.CfgFullcharge.ChargeCurrent;
			operator.CfgFullcharge.TaperingFactor = btbdefault.CfgFullcharge.ChargeCurrentLimit;
			operator.CfgFullcharge.SettlingTime = btbdefault.CfgFullcharge.SettlingTime;

			HidePanel(panelCFGFullcharging);
			break;
	}
	return 0;
}

int CVICALLBACK PNL_M3CBExit (int panelHandle, int event, void *callbackData,
							  int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_CLOSE:
			HidePanel(panelCFGFullcharging);
			break;
	}
	return 0;
}

	/***** Cell Balancing *****/ 
 int CVICALLBACK cmdM4SaveCB (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	double ChargeVoltageValue;
	double ChargeCurrentValue;
	double DischargeVoltageValue;
	double DischargeCurrentValue;
	double TaperingFactorValue;
	double ChargeCurrentLimitValue;
	double SettlingTimeValue;
	double RepeatCountValue;
	
	switch (event)
	{
		case EVENT_COMMIT:

			GetCtrlVal (panelCFGCellbalancing, PNL_M4_TARGET_VOLTAGE, &ChargeVoltageValue);
			GetCtrlVal (panelCFGCellbalancing, PNL_M4_TARGET_CURRENT, &ChargeCurrentValue);
			GetCtrlVal (panelCFGCellbalancing, PNL_M4_D_TARGET_VOLTAGE, &DischargeVoltageValue);
			GetCtrlVal (panelCFGCellbalancing, PNL_M4_D_TARGET_CURRENT, &DischargeCurrentValue);
			GetCtrlVal (panelCFGCellbalancing, PNL_M4_TAPERING, &TaperingFactorValue);
			GetCtrlVal (panelCFGCellbalancing, PNL_M4_END_CURRENT, &ChargeCurrentLimitValue); 
			GetCtrlVal (panelCFGCellbalancing, PNL_M4_SETTLING_TIME, &SettlingTimeValue);
			GetCtrlVal (panelCFGCellbalancing, PNL_M4_REPEAT_CNT, &RepeatCountValue);
			
			if(btb_option == 0)
			{
				operator.CfgCellBalance.ChargeVoltage = ChargeVoltageValue;
				operator.CfgCellBalance.DischargeVoltage = ChargeVoltageValue;
			}
			else
			{
				operator.CfgCellBalance.ChargeVoltage1CellFailed = ChargeVoltageValue;
				operator.CfgCellBalance.DischargeVoltage1CellFailed = ChargeVoltageValue;
			}			
			
			operator.CfgCellBalance.ChargeCurrent =  ChargeCurrentValue;
			operator.CfgCellBalance.DischargeCurrent =  DischargeCurrentValue; 
			operator.CfgCellBalance.TaperingFactor = TaperingFactorValue;
			operator.CfgCellBalance.ChargeCurrentLimit = ChargeCurrentLimitValue;
			operator.CfgCellBalance.SettlingTime = SettlingTimeValue;
			operator.CfgCellBalance.RepeatCount = RepeatCountValue;
			HidePanel(panelCFGCellbalancing);
			break;
	}
	return 0;
}
int CVICALLBACK cmdM4CancelCB (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(btb_option == 0)
			{
				SetCtrlVal (panelCFGCellbalancing, PNL_M4_TARGET_VOLTAGE, operator.CfgCellBalance.ChargeVoltage);
				SetCtrlVal (panelCFGCellbalancing, PNL_M4_D_TARGET_VOLTAGE, operator.CfgCellBalance.DischargeVoltage); 
			}
			else
			{
				SetCtrlVal (panelCFGCellbalancing, PNL_M4_TARGET_VOLTAGE, operator.CfgCellBalance.ChargeVoltage1CellFailed);
				SetCtrlVal (panelCFGCellbalancing, PNL_M4_D_TARGET_VOLTAGE, operator.CfgCellBalance.DischargeVoltage1CellFailed);
			}
			SetCtrlVal (panelCFGCellbalancing, PNL_M4_TARGET_CURRENT, operator.CfgCellBalance.ChargeCurrent);
			SetCtrlVal (panelCFGCellbalancing, PNL_M4_D_TARGET_CURRENT, operator.CfgCellBalance.DischargeCurrent);
			SetCtrlVal (panelCFGCellbalancing, PNL_M4_TAPERING, operator.CfgCellBalance.TaperingFactor);
			SetCtrlVal (panelCFGCellbalancing, PNL_M4_END_CURRENT, operator.CfgCellBalance.ChargeCurrentLimit);
			SetCtrlVal (panelCFGCellbalancing, PNL_M4_SETTLING_TIME, operator.CfgCellBalance.SettlingTime);
			SetCtrlVal (panelCFGCellbalancing, PNL_M4_REPEAT_CNT, operator.CfgCellBalance.RepeatCount);			
			HidePanel(panelCFGCellbalancing);
			break;
	}
	return 0;
}

int CVICALLBACK cmdM4ResetCB (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(btb_option == 0)
			{
				SetCtrlVal (panelCFGCellbalancing, PNL_M4_TARGET_VOLTAGE, btbdefault.CfgCellBalance.ChargeVoltage);
				SetCtrlVal (panelCFGCellbalancing, PNL_M4_D_TARGET_VOLTAGE, btbdefault.CfgCellBalance.DischargeVoltage); 
			}
			else
			{
				SetCtrlVal (panelCFGCellbalancing, PNL_M4_TARGET_VOLTAGE, btbdefault.CfgCellBalance.ChargeVoltage1CellFailed);
				SetCtrlVal (panelCFGCellbalancing, PNL_M4_D_TARGET_VOLTAGE, btbdefault.CfgCellBalance.DischargeVoltage1CellFailed);
			}
			SetCtrlVal (panelCFGCellbalancing, PNL_M4_TARGET_CURRENT, btbdefault.CfgCellBalance.ChargeCurrent);
			SetCtrlVal (panelCFGCellbalancing, PNL_M4_D_TARGET_CURRENT, btbdefault.CfgCellBalance.DischargeCurrent);
			SetCtrlVal (panelCFGCellbalancing, PNL_M4_TAPERING, btbdefault.CfgCellBalance.TaperingFactor);
			SetCtrlVal (panelCFGCellbalancing, PNL_M4_END_CURRENT, btbdefault.CfgCellBalance.ChargeCurrentLimit);
			SetCtrlVal (panelCFGCellbalancing, PNL_M4_SETTLING_TIME, btbdefault.CfgCellBalance.SettlingTime);
			SetCtrlVal (panelCFGCellbalancing, PNL_M4_REPEAT_CNT, btbdefault.CfgCellBalance.RepeatCount);

			operator.CfgCellBalance.ChargeVoltage = btbdefault.CfgCellBalance.ChargeVoltage;
			operator.CfgCellBalance.DischargeVoltage = btbdefault.CfgCellBalance.DischargeVoltage;
			operator.CfgCellBalance.ChargeVoltage1CellFailed = btbdefault.CfgCellBalance.ChargeVoltage1CellFailed;
			operator.CfgCellBalance.DischargeVoltage1CellFailed = btbdefault.CfgCellBalance.DischargeVoltage1CellFailed;

			operator.CfgCellBalance.ChargeCurrent = btbdefault.CfgCellBalance.ChargeCurrent;
			operator.CfgCellBalance.DischargeCurrent = btbdefault.CfgCellBalance.DischargeCurrent;
			operator.CfgCellBalance.TaperingFactor = btbdefault.CfgCellBalance.TaperingFactor;
			operator.CfgCellBalance.ChargeCurrentLimit = btbdefault.CfgCellBalance.ChargeCurrentLimit;
			operator.CfgCellBalance.SettlingTime = btbdefault.CfgCellBalance.SettlingTime;
			operator.CfgCellBalance.RepeatCount = btbdefault.CfgCellBalance.RepeatCount;
			
			HidePanel(panelCFGCellbalancing);
			break;
	}
	return 0;
}

int CVICALLBACK PNL_M4CBExit (int panelHandle, int event, void *callbackData,
							  int eventData1, int eventData2)
{			
	switch (event)
	{
		case EVENT_CLOSE:
			HidePanel(panelCFGCellbalancing);
			break;
	}
	return 0;
}

	/***** End Of Storage *****/ 
int CVICALLBACK cmdM5SaveCB (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	double ChargeVoltageValue;
	double ChargeCurrentValue;
	double DischargeCurrentValue;
	double RepeatCountValue;
	
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal (panelCFGEndofstorage, PNL_M5_TARGET_VOLTAGE, &ChargeVoltageValue);
			GetCtrlVal (panelCFGEndofstorage, PNL_M5_TARGET_CURRENT, &ChargeCurrentValue);
			GetCtrlVal (panelCFGEndofstorage, PNL_M5_D_TARGET_CURRENT, &DischargeCurrentValue);
			GetCtrlVal (panelCFGEndofstorage, PNL_M5_REPEAT_CNT, &RepeatCountValue);
			
			if(btb_option == 0)
			{
				operator.CfgEndOfStorage.ChargeVoltage = ChargeVoltageValue;
			}
			else
			{
				operator.CfgEndOfStorage.ChargeVoltage1CellFailed = ChargeVoltageValue;
			}			
			
			operator.CfgEndOfStorage.ChargeCurrent =  ChargeCurrentValue;
			operator.CfgEndOfStorage.DischargeCurrent =  DischargeCurrentValue; 
			operator.CfgEndOfStorage.RepeatCount = RepeatCountValue;
			HidePanel(panelCFGEndofstorage); 
			break;
	}
	return 0;
}

int CVICALLBACK cmdM5CancelCB (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(btb_option == 0)
			{
				SetCtrlVal (panelCFGEndofstorage, PNL_M5_TARGET_VOLTAGE, operator.CfgEndOfStorage.ChargeVoltage);
			}
			else
			{
				SetCtrlVal (panelCFGEndofstorage, PNL_M5_TARGET_VOLTAGE, operator.CfgEndOfStorage.ChargeVoltage1CellFailed);
			}
			SetCtrlVal (panelCFGEndofstorage, PNL_M5_TARGET_CURRENT, operator.CfgEndOfStorage.ChargeCurrent);
			SetCtrlVal (panelCFGEndofstorage, PNL_M5_D_TARGET_CURRENT, operator.CfgEndOfStorage.DischargeCurrent);
			SetCtrlVal (panelCFGEndofstorage, PNL_M5_REPEAT_CNT, operator.CfgEndOfStorage.RepeatCount);
			HidePanel(panelCFGEndofstorage); 
			break;
	}
	return 0;
}

int CVICALLBACK cmdM5ResetCB (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(btb_option == 0)
			{
				SetCtrlVal (panelCFGEndofstorage, PNL_M5_TARGET_VOLTAGE, btbdefault.CfgEndOfStorage.ChargeVoltage);
			}
			else
			{
				SetCtrlVal (panelCFGEndofstorage, PNL_M5_TARGET_VOLTAGE, btbdefault.CfgEndOfStorage.ChargeVoltage1CellFailed);
			}
			SetCtrlVal (panelCFGEndofstorage, PNL_M5_TARGET_CURRENT, btbdefault.CfgEndOfStorage.ChargeCurrent);
			SetCtrlVal (panelCFGEndofstorage, PNL_M5_D_TARGET_CURRENT, btbdefault.CfgEndOfStorage.DischargeCurrent);
			SetCtrlVal (panelCFGEndofstorage, PNL_M5_REPEAT_CNT, btbdefault.CfgEndOfStorage.RepeatCount);
			
			operator.CfgEndOfStorage.ChargeVoltage = btbdefault.CfgEndOfStorage.ChargeVoltage;
		    operator.CfgEndOfStorage.ChargeVoltage1CellFailed	=  btbdefault.CfgEndOfStorage.ChargeVoltage1CellFailed;
			operator.CfgEndOfStorage.ChargeCurrent =  btbdefault.CfgEndOfStorage.ChargeCurrent;
			operator.CfgEndOfStorage.DischargeCurrent = btbdefault.CfgEndOfStorage.DischargeCurrent;
			operator.CfgEndOfStorage.RepeatCount = btbdefault.CfgEndOfStorage.RepeatCount;
			
			HidePanel(panelCFGEndofstorage); 
			break;
	}
	return 0;
}

int CVICALLBACK PNL_M5CBExit (int panelHandle, int event, void *callbackData,
							  int eventData1, int eventData2)
{			
	switch (event)
	{
		case EVENT_CLOSE:
			HidePanel(panelCFGEndofstorage);
			break;
	}
	return 0;
}


int CVICALLBACK selectMethodRadio (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_MARK_STATE_CHANGE:
			//This function gets the current selection from our radio button group.
			Radio_GetMarkedOption (panelHandle, PANEL_METHODOPTION, &OptionValue);	//This variable stores the value of the radio button control.
			if(OptionValue == 0)
			{
				SetCtrlAttribute (panelHandle, PANEL_EXECUTION, ATTR_DIMMED, ON);
			}
			else
			{
				SetCtrlAttribute (panelHandle, PANEL_EXECUTION, ATTR_DIMMED, OFF);
			}
		break;
	}
	return 0;
}

int CVICALLBACK cmdFileSaveCB (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	int FileSaveValue;
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(panelHandle, PANEL_FILESAVEBT,&FileSaveValue);
			if(FileSaveValue)
			{
				filesave_manualexecute = ON;
			}
			else
			{
				filesave_manualexecute = OFF;
				filesave_autoexecute = OFF;
			}
			filesave_progress==INIT;
			break;
	}
	return 0;
}

void CVICALLBACK cmdFileIntervalCB (int menuBar, int menuItem, void *callbackData,
									int panel)
{
	DisplayPanel(panelInterval);
}


int CVICALLBACK cmdM6SaveCB (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{

	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal (panelInterval, PNL_M6_NUMERIC, &interval_value); 
			operator.CfgFileGraph.Interval = interval_value;
			HidePanel(panelInterval); 
			break;
	}
	return 0;
}

int CVICALLBACK cmdM6CancelCB (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			SetCtrlVal (panelInterval, PNL_M6_NUMERIC, operator.CfgFileGraph.Interval); 
			HidePanel(panelInterval); 
			break;
	}
	return 0;
}  

int CVICALLBACK cmdM6ResetCB (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			SetCtrlVal (panelInterval, PNL_M6_NUMERIC, btbdefault.CfgFileGraph.Interval); 
			operator.CfgFileGraph.Interval = btbdefault.CfgFileGraph.Interval;		// 10s
			HidePanel(panelInterval); 
			break;
	}
	return 0;
}

int CVICALLBACK PNL_M6CBExit (int panelHandle, int event, void *callbackData,
							  int eventData1, int eventData2)
{			
	switch (event)
	{
		case EVENT_CLOSE:
			HidePanel(panelInterval);
			break;
	}
	return 0;
}

int CVICALLBACK cmdM7SaveCB (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	int ModelIDValue;
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal (panelModelList, PNL_M7_RING, &ModelIDValue);
			operator.ModelID = ModelIDValue;
			SetCtrlVal (panelHandle, PANEL_BTBMODELNAME, operator.ModelID);
			cell_number_setup();
			HidePanel(panelModelList);
			break;
	}
	return 0;
}

int CVICALLBACK cmdM7CancelCB (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			SetCtrlVal (panelModelList, PNL_M7_RING, operator.ModelID); 
			HidePanel(panelModelList);
			break;
	}
	return 0;
}
int CVICALLBACK PNL_M7CBExit (int panelHandle, int event, void *callbackData,
							  int eventData1, int eventData2)
{			
	switch (event)
	{
		case EVENT_CLOSE:
			HidePanel(panelModelList);
			break;
	}
	return 0;
}

void CVICALLBACK cmdModelListCB (int menuBar, int menuItem, void *callbackData,
								 int panel)
{
	DisplayPanel(panelModelList); 
}


int CVICALLBACK cmdModelIDCB (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			cell_number_setup();
			break;
	}
	return 0;
}

void cell_number_setup()
{
	GetCtrlVal (panelHandle, PANEL_BTBMODELNAME, &modelValue);
	
	switch(modelValue)
	{
		case K6FM:
		case K7FM:
		case EOIRSatFM:
		case K7AFM:
			cellTotalCount = CELL_NUM_12;
			break;
		case KOMPSAT:
			cellTotalCount = CELL_NUM_13;
			break;
		default:
			cellTotalCount = CELL_NUM_12;
			break;			
	}
	
	if(btb_option)
		cellTotalCount = cellTotalCount -1;
	
	for(int i=0;i<CELLMAXCOUNT;i++)
	{
		if(i<cellTotalCount)
			SetCtrlAttribute(panelHandle,(PANEL_CELL1+i),ATTR_DIMMED,OFF);
		else
			SetCtrlAttribute(panelHandle,(PANEL_CELL1+i),ATTR_DIMMED,ON);
	}	
	operator.ModelID = modelValue;
	SetCtrlVal (panelModelList, PNL_M7_RING, operator.ModelID);
}

int CVICALLBACK cmdM8SaveCB (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	char operatornameValue[21];
	
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal (panel, PNL_M8_STRING, operatornameValue);

			if(strlen(operatornameValue) > 20 || strlen(operatornameValue) < 3)
			{
				//MessagePopup ("Name", "Input charactors(3~20)");
				SetCtrlVal(panelErrMessage,PNL_ERRMSG_TEXTMSG,"Name: Input charactors(3~20)");
				DisplayPanel(panelErrMessage);
			}
			else
			{
				strcpy(operator.OperatorName,operatornameValue);
				SetCtrlVal(panelHandle,PANEL_STRING,operator.OperatorName);
				HidePanel(panelOperator);
			}
			break;
	}
	return 0;
}

int CVICALLBACK cmdM8ExitCB (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			HidePanel(panelOperator);  
			break;
	}
	return 0;
}

int CVICALLBACK PNL_M8CBExit (int panelHandle, int event, void *callbackData,
							  int eventData1, int eventData2)
{			
	switch (event)
	{
		case EVENT_CLOSE:
			HidePanel(panelOperator);
			break;
	}
	return 0;
}

void CVICALLBACK cmdOperatorCB (int menuBar, int menuItem, void *callbackData,
								int panel)
{
	DisplayPanel(panelOperator); 
}

int CVICALLBACK PNL_TEMPCBExit (int panelHandle, int event, void *callbackData,
							  int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_CLOSE:
			HidePanel(panelTemperature);
			break;
	}
	return 0;
}

void CVICALLBACK cmdTempLimitCB (int menuBar, int menuItem, void *callbackData,
								 int panel)
{
	DisplayPanel(panelTemperature); 	
}

int CVICALLBACK cmdTEMPSaveCB (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	double tempLimit1Value;
	double tempLimit2Value;
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal (panel, PNL_TEMP_NUMERIC, &tempLimit1Value);
			GetCtrlVal (panel, PNL_TEMP_NUMERIC_2, &tempLimit2Value);
			operator.Temperature.limit1 = tempLimit1Value;
			operator.Temperature.limit2 = tempLimit2Value;
			HidePanel(panelTemperature); 
			break;
	}
	return 0;
}

int CVICALLBACK cmdTEMPCancelCB (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			SetCtrlVal (panel, PNL_TEMP_NUMERIC, operator.Temperature.limit1);
			SetCtrlVal (panel, PNL_TEMP_NUMERIC_2, operator.Temperature.limit2);
			HidePanel(panelTemperature); 
			break;
	}
	return 0;
}

int CVICALLBACK cmdTEMPResetCB (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			SetCtrlVal (panel, PNL_TEMP_NUMERIC, btbdefault.Temperature.limit1);
			SetCtrlVal (panel, PNL_TEMP_NUMERIC_2, btbdefault.Temperature.limit2);
			operator.Temperature.limit1 = btbdefault.Temperature.limit1;
			operator.Temperature.limit2 = btbdefault.Temperature.limit2;
			
			HidePanel(panelTemperature); 
			break;
	}
	return 0;
}

void CVICALLBACK cmdHelp(int menubar, int menuItem, void *callbackData, int panel)
{
	MessagePopup ("BTB System", HELPTEXT);
}

void CVICALLBACK cmdMainExit (int menuBar, int menuItem, void *callbackData,
							  int panel)
{
	QuitUserInterface (0); 
}
								

void CVICALLBACK cmdCellVoltageLimitCB (int menuBar, int menuItem, void *callbackData,
										int panel)
{
	DisplayPanel(panelCellVoltageLimit); 
}

int CVICALLBACK cmdCELLSaveCB (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	double uplimitValue;
	double downlimitValue;
	
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal (panel, PNL_CELLS_UP_LIMIT, &uplimitValue);
			GetCtrlVal (panel, PNL_CELLS_DOWN_LIMIT, &downlimitValue);
			operator.Cell.uplimit = uplimitValue;
			operator.Cell.downlimit = downlimitValue;
			HidePanel(panelCellVoltageLimit); 
			break;
	}
	return 0;
}

int CVICALLBACK cmdCELLCancelCB (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			SetCtrlVal (panel, PNL_CELLS_UP_LIMIT, operator.Cell.uplimit);
			SetCtrlVal (panel, PNL_CELLS_DOWN_LIMIT, operator.Cell.downlimit);
			HidePanel(panelCellVoltageLimit); 

			break;
	}
	return 0;
}

int CVICALLBACK cmdCELLResetCB (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			SetCtrlVal (panel, PNL_CELLS_UP_LIMIT, btbdefault.Cell.uplimit);
			SetCtrlVal (panel, PNL_CELLS_DOWN_LIMIT, btbdefault.Cell.downlimit);  
			operator.Cell.uplimit =  btbdefault.Cell.uplimit;
			operator.Cell.downlimit =  btbdefault.Cell.downlimit;
			HidePanel(panelCellVoltageLimit); 

			break;
	}
	return 0;
}
int CVICALLBACK PNL_CELLCBExit (int panelHandle, int event, void *callbackData,
							  int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_CLOSE:
			HidePanel(panelCellVoltageLimit);
			break;
	}
	return 0;
}

void CVICALLBACK cmdDecimalPointCB (int menuBar, int menuItem, void *callbackData,
									int panel)
{
	DisplayPanel(panelDecimalPoint); 
}

int CVICALLBACK cmdDecimalSaveCB (int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)
{
	int decimalPointValue;
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal (panel, PNL_DECI_NUMERIC, &decimalPointValue);
			operator.DecimalPoint = decimalPointValue;
			HidePanel(panelDecimalPoint); 
			break;
	}
	return 0;
}

int CVICALLBACK cmdDecimalCancelCB (int panel, int control, int event,
									void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
 			SetCtrlVal (panel, PNL_DECI_NUMERIC, operator.DecimalPoint);
			HidePanel(panelDecimalPoint); 
			break;
	}
	return 0;
}

int CVICALLBACK cmdDecimalResetCB (int panel, int control, int event,
								   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			SetCtrlVal (panel, PNL_DECI_NUMERIC, btbdefault.DecimalPoint);
			operator.DecimalPoint =  btbdefault.DecimalPoint;
			HidePanel(panelDecimalPoint); 
			break;
	}
	return 0;
}
int CVICALLBACK PNL_DECIExit (int panelHandle, int event, void *callbackData,
							  int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_CLOSE:
			HidePanel(panelDecimalPoint);
			break;
	}
	return 0;
}

void CVICALLBACK cmdResetBTBCB (int menuBar, int menuItem, void *callbackData,
								int panel)
{
	btb_status_change(STS_INIT);
}

int CVICALLBACK cmdErrMessageHide (int panel, int control, int event,
								   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			HidePanel(panelErrMessage);
			break;
	}
	return 0;
}
