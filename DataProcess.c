/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FILE:    DataProcess.c                                                    */
/*                                                                           */
/* PURPOSE: To handel BTB Data & Display console & save files                */
/*                                                                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Include files                                                             */
/*---------------------------------------------------------------------------*/

#include "excelreport.h"
#include <udpsupp.h>
#include "radioGroup.h"
#include <formatio.h>
#include "inifile.h"
#include <utility.h>
#include <tcpsupp.h>
#include <userint.h>  
#include "BTBController.h"
#include "define.h"
#include <cvirte.h>

static	IFBoardData_type receiveData;
static	ssize_t receivedataSize = sizeof(receiveData);	

static CAObjHandle applicationHandle = 0;
static CAObjHandle workbookHandle = 0;
static CAObjHandle worksheetHandle = 0;
static CAObjHandle chartHandle = 0;
static int PlotType = ExRConst_GalleryLine;

FILE *fp;
char file_name[MAX_FILENAME_LEN];

static const char BTBModel[][MODEL_STR_LEN] = {
	"KOMPSAT-6 FM",
	"KOMPSAT-7 FM",
	"EOIRSat FM",
	"K7A FM",			// default model
	"KOMPSAT-Z",
};

static const char EventStats[][STATS_STR_LEN] = {
	"File Save On",
	"File Save Off",
	"Excution Start",
	"Excution Stop",
	"Relay On",
	"Relay Off",
	"Warning",
	"Alarm On",
	"Alarm Off",
};

static long ltime;
static struct tm *newtime;
/*---------------------------------------------------------------------------*/
/* TCP Server to connect with I/F box                                        */	
/*---------------------------------------------------------------------------*/

int CVICALLBACK UDPReader (unsigned handle, int event, int error, void *callbackData)
{
	int dataSize;
	char			srcAddr[16];
	unsigned int	srcPort;
	
    switch (event)
    {
        case UDP_DATAREADY:
			
		// Pass NULL as the input buffer to determine the size of the arrived data.
		errChk (dataSize = UDPRead(handle, NULL, 0, UDP_DO_NOT_WAIT, NULL, NULL));
		
		// Read the waiting message into the allocated buffer.
		errChk(dataSize = UDPRead(handle, &receiveData, dataSize, UDP_DO_NOT_WAIT, &srcPort, srcAddr));

		connection_flag = ON;
		timeStartBTB = Timer(); 
		data_receive();
    }

Error:
	if (error < 0)
	{
		if(BTBWorkingStatus < STS_ERROR)
		{
			connection_flag = OFF;
			SetCtrlVal (panelHandle, PANEL_CONNECTED, OFF);
			SetErrorMessage(MSG_ERROR2,dataSize,AutoRun);
			btb_status_change(STS_ERROR);
		}
	}
	return 0;
}

void data_receive()
{
	static float pre_prefix = 0;
	CmtGetLock (gLockRandom);
	cellVoltageMin = 99;
	cellVoltageMax = 0;

	for(int i = 0;i < cellTotalCount;i++)
	{
		if(cellVoltageMax<receiveData.CellVoltage[i])
		{
			cellVoltageMax = receiveData.CellVoltage[i];
			dispMaxIndex = i;
		}
		if(cellVoltageMin>receiveData.CellVoltage[i])
		{
			cellVoltageMin = receiveData.CellVoltage[i];
			dispMinIndex = i;  
		}
		//BTBPackVoltage += receiveData.CellVoltage[i];
	}
	
	BTBPackVoltage = receiveData.BTBVoltage;
	BTBPackCurrent = receiveData.BTBCurrent;
	cellDispersion = cellVoltageMax - cellVoltageMin;
	dispMaxValue = receiveData.CellVoltage[dispMaxIndex];
	dispMinValue = receiveData.CellVoltage[dispMinIndex];

	charger_relay = receiveData.charger_relay;			// ON:1, OFF:0
	discharger_relay = receiveData.discharger_relay;	// ON:1, OFF:0
	
	CmtReleaseLock (gLockRandom);

	if(receiveData.prefix == pre_prefix)		// To-Do: needs to change comparing algorithm.
	{
		if(BTBWorkingStatus < STS_ERROR)
		{
			SetErrorMessage(MSG_ERROR17,NULL,AutoRun);
			btb_status_change(STS_ERROR);
			pre_prefix = receiveData.prefix;
		}
	}

	/****** check the switchs of the BTB front panel ******/

	// To-Do...
	/* checking Emergency Button */
	if(receiveData.front_switch & EM)		// Emergency Button
	{
		if(emergency_bt == OFF)
		{
			if(BTBWorkingStatus < STS_ERROR)
			{
				SetErrorMessage(MSG_ERROR100,NULL,AutoRun);
				btb_status_change(STS_ERROR);
			}
			else
			{
				event_send (EventTabNum, TABPANEL_TEXTBOX,EventMessage,NULL,MSG_ERROR100);
			}
			emergency_bt = ON;
		}
		
		SetCtrlVal(panelHandle,PANEL_LED_EMG,ON);
	}
	else
	{
		if(emergency_bt == ON)
		{
			emergency_bt = OFF;
		}
		SetCtrlVal(panelHandle,PANEL_LED_EMG,OFF);
	}

	/* checking ch. & disch. relay */
	if(charger_relay&&discharger_relay)
	{
		if(BTBWorkingStatus < STS_ERROR)
		{
			SetErrorMessage(MSG_ERROR18,NULL,AutoRun);
			btb_status_change(STS_ERROR);
		}
	}

	if(receiveData.front_switch & SR)		// Smart Redundance
	{
		SetCtrlVal(panelHandle,PANEL_LED_SMARTR,ON);
	}
	else
	{
		SetCtrlVal(panelHandle,PANEL_LED_SMARTR,OFF);
	}

	if(receiveData.front_switch & SP)		// Smart Primary
	{
		SetCtrlVal(panelHandle,PANEL_LED_SMARTP,ON);
	}
	else
	{
		SetCtrlVal(panelHandle,PANEL_LED_SMARTP,OFF);
	}

	if(receiveData.front_switch & BR)		// Bypass Redundance
	{
		SetCtrlVal(panelHandle,PANEL_LED_BYPASSR,ON);
	}
	else
	{
		SetCtrlVal(panelHandle,PANEL_LED_BYPASSR,OFF);
	}

	if(receiveData.front_switch & BP)		// Bypass Primary
	{
		SetCtrlVal(panelHandle,PANEL_LED_BYPASSP,ON);
	}
	else
	{
		SetCtrlVal(panelHandle,PANEL_LED_BYPASSP,OFF);
	}

	if(receiveData.front_switch & ER)		// Elect. Redundance
	{
		SetCtrlVal(panelHandle,PANEL_LED_ELECR,ON);
	}
	else
	{
		SetCtrlVal(panelHandle,PANEL_LED_ELECR,OFF);
	}

	if(receiveData.front_switch & EP)		// Elect. Redundance
	{
		SetCtrlVal(panelHandle,PANEL_LED_ELECP,ON);
	}
	else
	{
		SetCtrlVal(panelHandle,PANEL_LED_ELECP,OFF);
	}
	
	//BTBCheckingThread();
	//UIDisplayThread();
	//graph_packvoltageCB();
	//graph_cellvoltageCB();
}


int CVICALLBACK UIDisplayThread (void *functionData)
{
	char dispMaxLable[LABEL_SIZE];
	char dispMinLable[LABEL_SIZE];

	while(1)
	{
		if(connection_flag) 
		{
			for(int i=0;i<cellTotalCount;i++)
			{
				SetCtrlVal(panelHandle,(PANEL_CELL1+i),receiveData.CellVoltage[i]);
			}
			SetCtrlVal (panelHandle, PANEL_NUMERICTHERM,receiveData.BTBTemperature); 
			SetCtrlVal (panelHandle, PANEL_NUMERIC_14, BTBPackVoltage);
			SetCtrlVal (panelHandle, PANEL_NUMERIC_22, BTBPackCurrent);
			
			for(int i=0;i<cellTotalCount;i++)
			{
				SetCtrlAttribute (panelHandle, (PANEL_CELL1+i), ATTR_TEXT_BGCOLOR, VAL_CELL_BGTEXTCOLOR);
				SetCtrlAttribute (panelHandle, (PANEL_CELL1+i), ATTR_LABEL_BOLD, OFF);
			}

			SetCtrlAttribute (panelHandle, (PANEL_CELL1+dispMinIndex), ATTR_TEXT_BGCOLOR, VAL_BLUE); 
			SetCtrlAttribute (panelHandle, (PANEL_CELL1+dispMaxIndex), ATTR_TEXT_BGCOLOR, VAL_DK_GREEN); 
			SetCtrlAttribute (panelHandle, (PANEL_CELL1+dispMinIndex), ATTR_LABEL_BOLD, ON); 
			SetCtrlAttribute (panelHandle, (PANEL_CELL1+dispMaxIndex), ATTR_LABEL_BOLD, ON); 

			sprintf(dispMaxLable,"Cell Max(%d)",dispMaxIndex+1);
			sprintf(dispMinLable,"Cell Min(%d)",dispMinIndex+1); 
			SetCtrlAttribute (panelHandle, PANEL_NUMERIC_16, ATTR_LABEL_TEXT, dispMaxLable);
			SetCtrlAttribute (panelHandle, PANEL_NUMERIC_17, ATTR_LABEL_TEXT, dispMinLable); 
		
			SetCtrlVal (panelHandle, PANEL_NUMERIC_15, cellDispersion);
			SetCtrlVal (panelHandle, PANEL_NUMERIC_16, dispMaxValue);
			SetCtrlVal (panelHandle, PANEL_NUMERIC_17, dispMinValue);

			if(methodexecution_flag)
			{
				if(charge_flag)
				{
					SetCtrlAttribute (panelHandle, PANEL_CHARGEVOLTAGE, ATTR_DIMMED, OFF);
					SetCtrlAttribute (panelHandle, PANEL_CHARGECURRENT, ATTR_DIMMED, OFF);
				}
				else if(discharge_flag)
				{
					SetCtrlAttribute (panelHandle, PANEL_DISCHARGEVOLTAGE, ATTR_DIMMED, OFF);
					SetCtrlAttribute (panelHandle, PANEL_DISCHARGECURRENT, ATTR_DIMMED, OFF);
				}
				else
				{
					SetCtrlAttribute (panelHandle, PANEL_CHARGEVOLTAGE, ATTR_DIMMED, ON);
					SetCtrlAttribute (panelHandle, PANEL_CHARGECURRENT, ATTR_DIMMED, ON);
					SetCtrlVal(panelHandle,PANEL_CHARGEVOLTAGE,0.0);
					SetCtrlVal(panelHandle,PANEL_CHARGECURRENT,0.0);

					SetCtrlAttribute (panelHandle, PANEL_DISCHARGEVOLTAGE, ATTR_DIMMED, ON);
					SetCtrlAttribute (panelHandle, PANEL_DISCHARGECURRENT, ATTR_DIMMED, ON);
					SetCtrlVal(panelHandle,PANEL_DISCHARGEVOLTAGE,0.0);
					SetCtrlVal(panelHandle,PANEL_DISCHARGECURRENT,0.0);
				}

				if( BTBWorkingStatus == STS_ENDOFSTORAGE)
				{
					SetCtrlVal(panelHandle,PANEL_AMPEREHOUR,ampereHour);
					SetCtrlVal(panelHandle,PANEL_ENERGY,energy);
					SetCtrlVal(panelHandle,PANEL_RESISTANCE,resistance);
				}
			}
			else
			{
				SetCtrlAttribute (panelHandle, PANEL_CHARGEVOLTAGE, ATTR_DIMMED, ON);
				SetCtrlAttribute (panelHandle, PANEL_CHARGECURRENT, ATTR_DIMMED, ON);	
				SetCtrlAttribute (panelHandle, PANEL_DISCHARGEVOLTAGE, ATTR_DIMMED, ON);
				SetCtrlAttribute (panelHandle, PANEL_DISCHARGECURRENT, ATTR_DIMMED, ON);
				
				SetCtrlVal(panelHandle,PANEL_CHARGEVOLTAGE,0.0);
				SetCtrlVal(panelHandle,PANEL_CHARGECURRENT,0.0);
				SetCtrlVal(panelHandle,PANEL_DISCHARGEVOLTAGE,0.0);
				SetCtrlVal(panelHandle,PANEL_DISCHARGECURRENT,0.0);
			}
		}
		Delay(nTimeUI);
	}
	return 0;
}

int CVICALLBACK BTBCheckingThread (void *functionData)
{

	char msg[TEXT_SIZE];
	static int alarmchecked = ON;
	static int alarmcheckedtemp = ON;
	static int warningflag = ON;
	float vuplimit;
	float vdonwlimit;
	
	while(1)
	{
		if(btb_option==0)
		{
			vuplimit = NORMAL_UP_LIMIT;
			vdonwlimit = NORMAL_DOWN_LIMIT;
		}
		else
		{
			vuplimit = ONECELLFAILED_UP_LIMIT;
			vdonwlimit = ONECELLFAILED_DOWN_LIMIT;
		}

		if(methodexecution_flag)  // if method running, start checking voltage
		{
			if(alarmchecked)
			{
				if(cellVoltageMax>=operator.Cell.uplimit)
				{
					btb_status_change(STS_ALARM1);
					excution_stop(AutoRun);
					sprintf(msg,"Cell(%d) Voltage(%.3f) over the limit voltage(%.3f)",(dispMaxIndex+1),cellVoltageMax,operator.Cell.uplimit);
					event_send (EventTabNum, TABPANEL_TEXTBOX,AlarmON,Voltage,msg);
					MessagePopup ("Alarm", msg);
					alarmchecked = 0;
					
				}
				else if(cellVoltageMin<operator.Cell.downlimit)	
				{
					btb_status_change(STS_ALARM1);
					excution_stop(AutoRun);
					sprintf(msg,"Cell(%d) Voltage(%.3f) under the limit voltage(%.3f)",(dispMinIndex+1),cellVoltageMin,operator.Cell.downlimit);
					event_send (EventTabNum, TABPANEL_TEXTBOX,AlarmON,Voltage,msg);
					MessagePopup ("Alarm", msg);
					alarmchecked = 0;
					
				}
				else if(BTBPackVoltage>=vuplimit)
				{
					btb_status_change(STS_ALARM1);
					excution_stop(AutoRun);
					sprintf(msg,"Pack Voltage(%.3f) over the limit voltage(%.3f)",BTBPackVoltage,vuplimit);
					event_send (EventTabNum, TABPANEL_TEXTBOX,AlarmON,Voltage,msg);
					MessagePopup ("Alarm", msg);
					alarmchecked = 0;
					
				}
				else if(BTBPackVoltage<vdonwlimit)
				{
					btb_status_change(STS_ALARM1);
					excution_stop(AutoRun);
					sprintf(msg,"Pack Voltage(%.3f) under the limit voltage(%.3f)",BTBPackVoltage,vdonwlimit);
					event_send (EventTabNum, TABPANEL_TEXTBOX,AlarmON,Voltage,msg);
					MessagePopup ("Alarm", msg);
					alarmchecked = 0;
				}
			}
			
			if((cellVoltageMax<operator.Cell.uplimit && cellVoltageMin>operator.Cell.downlimit) && (BTBPackVoltage<vuplimit && BTBPackVoltage>=vdonwlimit))
			{
				if(alarmchecked==0)
				{
					alarmchecked = 1;
				}
			}
		}
		else
		{
			if((cellVoltageMax<operator.Cell.uplimit && cellVoltageMin>operator.Cell.downlimit) && (BTBPackVoltage<vuplimit && BTBPackVoltage>=vdonwlimit))
			{
				if(alarmchecked==0)
				{
					if(BTBWorkingStatus==STS_ALARM1)
					{
						btb_status_change(STS_STANDBY);
					}
					alarmchecked = 1;
				}
			}
		}

		if(receiveData.BTBTemperature > operator.Temperature.limit2)
		{
			SetCtrlAttribute (panelHandle, PANEL_DECORATION_2,ATTR_FRAME_COLOR,VAL_RED);

			if(alarmcheckedtemp)
			{
				btb_status_change(STS_ALARM2);
				excution_stop(AutoRun);
				sprintf(msg,"Temperature(%.2f) is higher than %.2f!!!",receiveData.BTBTemperature,operator.Temperature.limit2);
				event_send (EventTabNum, TABPANEL_TEXTBOX,AlarmON,Temperature,msg);
				MessagePopup ("Alarm", msg);
				alarmcheckedtemp = 0;
			}
		}
		else if(receiveData.BTBTemperature > operator.Temperature.limit1)
		{
			SetCtrlAttribute (panelHandle, PANEL_DECORATION_2,ATTR_FRAME_COLOR,VAL_YELLOW);

			if(warningflag)
			{
				sprintf(msg,"Temperature(%.2f) is higher than %.2f!!!",receiveData.BTBTemperature,operator.Temperature.limit1);
				event_send (EventTabNum, TABPANEL_TEXTBOX,Warning,Temperature,msg);
				warningflag = 0;
			}
			
			if(alarmcheckedtemp==0)
			{
				if(BTBWorkingStatus==STS_ALARM2)
				{
					btb_status_change(STS_STANDBY);
					Radio_SetMarkedOption (panelHandle, PANEL_METHODOPTION, 0); 
					SetCtrlAttribute (panelHandle, PANEL_METHODOPTION, ATTR_DIMMED, OFF);
				}
				alarmcheckedtemp = 1;
			}
		}
		else
		{			
			SetCtrlAttribute (panelHandle, PANEL_NUMERICTHERM,ATTR_FILL_COLOR,VAL_BLUE);
			SetCtrlAttribute (panelHandle, PANEL_NUMERICTHERM,ATTR_TEXT_COLOR,VAL_TEMP_COLOR);
			SetCtrlAttribute (panelHandle, PANEL_DECORATION_2,ATTR_FRAME_COLOR,VAL_FRAME_GRAY2);
			if(BTBWorkingStatus==STS_ALARM2)
			{
				btb_status_change(STS_STANDBY);
				Radio_SetMarkedOption (panelHandle, PANEL_METHODOPTION, 0); 
				SetCtrlAttribute (panelHandle, PANEL_METHODOPTION, ATTR_DIMMED, OFF);
			}
			alarmcheckedtemp = 1;
			warningflag = 1;
		}

		Delay(1);
	}
	return 0;
}


/*---------------------------------------------------------------------------*/
/* INI File                                                                  */	
/*---------------------------------------------------------------------------*/
int ini_save(char *filepath)
{
	int ret;
	IniText iniText;
	
	ret = 0;
	
	iniText = Ini_New(TRUE);
	
	if(iniText != 0)
	{
		// charge
		Ini_PutDouble(iniText, "Charge", "ChargeVoltage", operator.CfgCharge.ChargeVoltage); 	
		Ini_PutDouble(iniText, "Charge", "ChargeVoltage1CellFailed", operator.CfgCharge.ChargeVoltage1CellFailed);
		Ini_PutDouble(iniText, "Charge", "ChargeCurrent", operator.CfgCharge.ChargeCurrent);
		Ini_PutInt(iniText, "Charge", "ChargeCurrentSelect", operator.CfgCharge.ChargeCurrentSelect);
		
		// discharge
		Ini_PutDouble(iniText, "Discharge", "DischargeVoltage", operator.CfgDischarge.DischargeVoltage); 	
		Ini_PutDouble(iniText, "Discharge", "DischargeVoltage1CellFailed", operator.CfgDischarge.DischargeVoltage1CellFailed);
		Ini_PutDouble(iniText, "Discharge", "DischargeCurrent", operator.CfgDischarge.DischargeCurrent);			
		
		// full-charge
		Ini_PutDouble(iniText, "Fullcharge", "ChargeVoltage", operator.CfgFullcharge.ChargeVoltage); 	
		Ini_PutDouble(iniText, "Fullcharge", "ChargeVoltage1CellFailed", operator.CfgFullcharge.ChargeVoltage1CellFailed);
		Ini_PutDouble(iniText, "Fullcharge", "ChargeCurrent", operator.CfgFullcharge.ChargeCurrent);	
		Ini_PutDouble(iniText, "Fullcharge", "ChargeCurrentLimit", operator.CfgFullcharge.ChargeCurrentLimit); 	
		Ini_PutDouble(iniText, "Fullcharge", "DispersionLimit1", operator.CfgFullcharge.DispersionLimit1);
		Ini_PutDouble(iniText, "Fullcharge", "TaperingFactor", operator.CfgFullcharge.TaperingFactor);				
		Ini_PutDouble(iniText, "Fullcharge", "SafeTime", operator.CfgFullcharge.SettlingTime); 	

		// cell-balancing
		Ini_PutDouble(iniText, "Cellbalance", "ChargeVoltage", operator.CfgCellBalance.ChargeVoltage); 	
		Ini_PutDouble(iniText, "Cellbalance", "ChargeVoltage1CellFailed", operator.CfgCellBalance.ChargeVoltage1CellFailed);
		Ini_PutDouble(iniText, "Cellbalance", "ChargeCurrent", operator.CfgCellBalance.ChargeCurrent);	
		Ini_PutDouble(iniText, "Cellbalance", "DischargeVoltage", operator.CfgCellBalance.DischargeVoltage);
		Ini_PutDouble(iniText, "Cellbalance", "DischargeVoltage1CellFailed", operator.CfgCellBalance.DischargeVoltage1CellFailed);
		Ini_PutDouble(iniText, "Cellbalance", "DischargeCurrent", operator.CfgCellBalance.DischargeCurrent); 
		Ini_PutDouble(iniText, "Cellbalance", "DispersionLimit1", operator.CfgCellBalance.DispersionLimit1);
		Ini_PutDouble(iniText, "Cellbalance", "DispersionLimit2", operator.CfgCellBalance.DispersionLimit2);				
		Ini_PutDouble(iniText, "Cellbalance", "DispersionLimit3", operator.CfgCellBalance.DispersionLimit3); 	
		Ini_PutDouble(iniText, "Cellbalance", "TaperingFactor", operator.CfgCellBalance.TaperingFactor); 	
		Ini_PutDouble(iniText, "Cellbalance", "ChargeCurrentLimit", operator.CfgCellBalance.ChargeCurrentLimit);
		Ini_PutDouble(iniText, "Cellbalance", "ChargingTime", operator.CfgCellBalance.ChargingTime);	
		Ini_PutDouble(iniText, "Cellbalance", "SafeTime", operator.CfgCellBalance.SettlingTime); 	
		Ini_PutDouble(iniText, "Cellbalance", "RepeatCount", operator.CfgCellBalance.RepeatCount);
		
		// end of storage
		Ini_PutDouble(iniText, "Endofstorage", "ChargeVoltage", operator.CfgEndOfStorage.ChargeVoltage); 
		Ini_PutDouble(iniText, "Endofstorage", "ChargeVoltage1CellFailed", operator.CfgEndOfStorage.ChargeVoltage1CellFailed);
		Ini_PutDouble(iniText, "Endofstorage", "ChargeCurrent", operator.CfgEndOfStorage.ChargeCurrent);				
		Ini_PutDouble(iniText, "Endofstorage", "DischargeCellVoltageLimit", operator.CfgEndOfStorage.DischargeCellVoltageLimit); 	
		Ini_PutDouble(iniText, "Endofstorage", "DischargeCurrent", operator.CfgEndOfStorage.DischargeCurrent); 	
		Ini_PutDouble(iniText, "Endofstorage", "DispersionLimit1", operator.CfgEndOfStorage.DispersionLimit1);
		Ini_PutDouble(iniText, "Endofstorage", "RepeatCount", operator.CfgEndOfStorage.RepeatCount);
		
		// file & graph
		Ini_PutInt(iniText, "FileGraph", "Interval", operator.CfgFileGraph.Interval); 	
		Ini_PutInt(iniText, "FileGraph", "graph1", operator.CfgFileGraph.graph1);
		Ini_PutInt(iniText, "FileGraph", "graph2", operator.CfgFileGraph.graph2);
		
		// model
		Ini_PutInt(iniText, "Model", "ModelID", operator.ModelID);
		Ini_PutInt(iniText, "Model", "BatteryCellOption", operator.BatteryCellOption);

		// temperature
		Ini_PutDouble(iniText, "Temperature", "Limit1", operator.Temperature.limit1);
		Ini_PutDouble(iniText, "Temperature", "Limit2", operator.Temperature.limit2);
		
		// cell voltage limit
		Ini_PutDouble(iniText, "Cell", "UpLimit", operator.Cell.uplimit);
		Ini_PutDouble(iniText, "Cell", "DownLimit", operator.Cell.downlimit);

		// operator
		Ini_PutString(iniText, "Operator", "strSTRING", operator.OperatorName); 
		
		// Decimal Point
		Ini_PutInt(iniText, "Decimal", "PointNum", operator.DecimalPoint);
		
		Ini_WriteToFile(iniText,filepath);
		Ini_Dispose(iniText);
		
		ret = ENOERR;
	}
	else
	{
		MessagePopup ("Error", "Configuration file");
		ret = -ENOENT;
	}
	return ret;
}

void CIS_Ini_GetStringCopy(IniText iniText, char *section, char *tag, char *item)
{
	static char *str;

	Ini_GetStringCopy (iniText, section, tag, &str);

	if(str != NULL)
	{
		strcpy( item, str ); 
		free(str);
	}
}

int ini_open_user(char *filepath)
{
	int ret;
	IniText iniText;
	
	ret = 0;
	
	iniText = Ini_New(TRUE);
	
	if(iniText != 0)
	{
		if(Ini_ReadFromFile(iniText,filepath)==0)		// To get user's value
		{
			// charge
			Ini_GetDouble(iniText, "Charge", "ChargeVoltage", &operator.CfgCharge.ChargeVoltage);
			Ini_GetDouble(iniText, "Charge", "ChargeVoltage1CellFailed", &operator.CfgCharge.ChargeVoltage1CellFailed);
			Ini_GetDouble(iniText, "Charge", "ChargeCurrent", &operator.CfgCharge.ChargeCurrent);
			Ini_GetInt(iniText, "Charge", "ChargeCurrentSelect", &operator.CfgCharge.ChargeCurrentSelect);
			
			// discharge
			Ini_GetDouble(iniText, "Discharge", "DischargeVoltage", &operator.CfgDischarge.DischargeVoltage); 	
			Ini_GetDouble(iniText, "Discharge", "DischargeVoltage1CellFailed", &operator.CfgDischarge.DischargeVoltage1CellFailed);
			Ini_GetDouble(iniText, "Discharge", "DischargeCurrent", &operator.CfgDischarge.DischargeCurrent);			
			
			// full-charge
			Ini_GetDouble(iniText, "Fullcharge", "ChargeVoltage", &operator.CfgFullcharge.ChargeVoltage); 	
			Ini_GetDouble(iniText, "Fullcharge", "ChargeVoltage1CellFailed", &operator.CfgFullcharge.ChargeVoltage1CellFailed);
			Ini_GetDouble(iniText, "Fullcharge", "ChargeCurrent", &operator.CfgFullcharge.ChargeCurrent);	
			Ini_GetDouble(iniText, "Fullcharge", "ChargeCurrentLimit", &operator.CfgFullcharge.ChargeCurrentLimit); 	
			Ini_GetDouble(iniText, "Fullcharge", "DispersionLimit1", &operator.CfgFullcharge.DispersionLimit1);
			Ini_GetDouble(iniText, "Fullcharge", "TaperingFactor", &operator.CfgFullcharge.TaperingFactor);				
 			Ini_GetDouble(iniText, "Fullcharge", "SafeTime", &operator.CfgFullcharge.SettlingTime); 	
	
			// cell-balancing
			Ini_GetDouble(iniText, "Cellbalance", "ChargeVoltage", &operator.CfgCellBalance.ChargeVoltage); 	
			Ini_GetDouble(iniText, "Cellbalance", "ChargeVoltage1CellFailed", &operator.CfgCellBalance.ChargeVoltage1CellFailed);
			Ini_GetDouble(iniText, "Cellbalance", "ChargeCurrent", &operator.CfgCellBalance.ChargeCurrent);	
			Ini_GetDouble(iniText, "Cellbalance", "DischargeVoltage", &operator.CfgCellBalance.DischargeVoltage);
			Ini_GetDouble(iniText, "Cellbalance", "DischargeVoltage1CellFailed", &operator.CfgCellBalance.DischargeVoltage1CellFailed);
			Ini_GetDouble(iniText, "Cellbalance", "DischargeCurrent", &operator.CfgCellBalance.DischargeCurrent); 
			Ini_GetDouble(iniText, "Cellbalance", "DispersionLimit1", &operator.CfgCellBalance.DispersionLimit1);
			Ini_GetDouble(iniText, "Cellbalance", "DispersionLimit2", &operator.CfgCellBalance.DispersionLimit2);				
 			Ini_GetDouble(iniText, "Cellbalance", "DispersionLimit3", &operator.CfgCellBalance.DispersionLimit3); 	
			Ini_GetDouble(iniText, "Cellbalance", "TaperingFactor", &operator.CfgCellBalance.TaperingFactor); 	
			Ini_GetDouble(iniText, "Cellbalance", "ChargeCurrentLimit", &operator.CfgCellBalance.ChargeCurrentLimit);
			Ini_GetDouble(iniText, "Cellbalance", "ChargingTime", &operator.CfgCellBalance.ChargingTime);	
			Ini_GetDouble(iniText, "Cellbalance", "SafeTime", &operator.CfgCellBalance.SettlingTime); 	
			Ini_GetDouble(iniText, "Cellbalance", "RepeatCount", &operator.CfgCellBalance.RepeatCount);
			
			// end of storage
			Ini_GetDouble(iniText, "Endofstorage", "ChargeVoltage", &operator.CfgEndOfStorage.ChargeVoltage); 
			Ini_GetDouble(iniText, "Endofstorage", "ChargeVoltage1CellFailed", &operator.CfgEndOfStorage.ChargeVoltage1CellFailed);
			Ini_GetDouble(iniText, "Endofstorage", "ChargeCurrent", &operator.CfgEndOfStorage.ChargeCurrent);				
 			Ini_GetDouble(iniText, "Endofstorage", "DischargeCellVoltageLimit", &operator.CfgEndOfStorage.DischargeCellVoltageLimit); 	
			Ini_GetDouble(iniText, "Endofstorage", "DischargeCurrent", &operator.CfgEndOfStorage.DischargeCurrent); 	
			Ini_GetDouble(iniText, "Endofstorage", "DispersionLimit1", &operator.CfgEndOfStorage.DispersionLimit1);
			Ini_GetDouble(iniText, "Endofstorage", "RepeatCount", &operator.CfgEndOfStorage.RepeatCount);	

			// file & graph			
			Ini_GetInt(iniText, "FileGraph", "Interval", &operator.CfgFileGraph.Interval); 	
			Ini_GetInt(iniText, "FileGraph", "graph1", &operator.CfgFileGraph.graph1);
			Ini_GetInt(iniText, "FileGraph", "graph2", &operator.CfgFileGraph.graph2);
			
			// model
			Ini_GetInt(iniText, "Model", "ModelID", &operator.ModelID);
			Ini_GetInt(iniText, "Model", "BatteryCellOption", &operator.BatteryCellOption);

			// temperature
			Ini_GetDouble(iniText, "Temperature", "Limit1", &operator.Temperature.limit1);
			Ini_GetDouble(iniText, "Temperature", "Limit2", &operator.Temperature.limit2);

			// cell voltage limit
			Ini_GetDouble(iniText, "Cell", "UpLimit", &operator.Cell.uplimit);
			Ini_GetDouble(iniText, "Cell", "DownLimit", &operator.Cell.downlimit);

			
			// operator
			CIS_Ini_GetStringCopy(iniText, "Operator", "strSTRING", operator.OperatorName); 
			
			// Decimal Point
			Ini_GetInt(iniText, "Decimal", "PointNum", &operator.DecimalPoint);
			
			Ini_Dispose(iniText);
		
			ret = ENOERR;
		}
		else		// To get the default values from BTB initial setting if there's no user's configurations.
		{
			MessagePopup ("Error", "There is no the default configuration file");
			ret = -ENOENT;
		}
	}
	else
	{
		MessagePopup ("Error", "Configuration file");
		ret = -ENOENT;
	}

	return ret;
}

int ini_open_default(char *filepath)
{
	int ret;
	IniText iniText;
	
	ret = 0;
	
	iniText = Ini_New(TRUE);
	
	if(iniText != 0)
	{
		if(Ini_ReadFromFile(iniText,filepath)==0)		// To get user's value
		{
			// charge
			Ini_GetDouble(iniText, "Charge", "ChargeVoltage", &btbdefault.CfgCharge.ChargeVoltage);
			Ini_GetDouble(iniText, "Charge", "ChargeVoltage1CellFailed", &btbdefault.CfgCharge.ChargeVoltage1CellFailed);
			Ini_GetDouble(iniText, "Charge", "ChargeCurrent", &btbdefault.CfgCharge.ChargeCurrent);
			Ini_GetInt(iniText, "Charge", "ChargeCurrentSelect", &btbdefault.CfgCharge.ChargeCurrentSelect);
			
			// discharge
			Ini_GetDouble(iniText, "Discharge", "DischargeVoltage", &btbdefault.CfgDischarge.DischargeVoltage); 	
			Ini_GetDouble(iniText, "Discharge", "DischargeVoltage1CellFailed", &btbdefault.CfgDischarge.DischargeVoltage1CellFailed);
			Ini_GetDouble(iniText, "Discharge", "DischargeCurrent", &btbdefault.CfgDischarge.DischargeCurrent);			
			
			// full-charge
			Ini_GetDouble(iniText, "Fullcharge", "ChargeVoltage", &btbdefault.CfgFullcharge.ChargeVoltage); 	
			Ini_GetDouble(iniText, "Fullcharge", "ChargeVoltage1CellFailed", &btbdefault.CfgFullcharge.ChargeVoltage1CellFailed);
			Ini_GetDouble(iniText, "Fullcharge", "ChargeCurrent", &btbdefault.CfgFullcharge.ChargeCurrent);	
			Ini_GetDouble(iniText, "Fullcharge", "ChargeCurrentLimit", &btbdefault.CfgFullcharge.ChargeCurrentLimit); 	
			Ini_GetDouble(iniText, "Fullcharge", "DispersionLimit1", &btbdefault.CfgFullcharge.DispersionLimit1);
			Ini_GetDouble(iniText, "Fullcharge", "TaperingFactor", &btbdefault.CfgFullcharge.TaperingFactor);				
 			Ini_GetDouble(iniText, "Fullcharge", "SafeTime", &btbdefault.CfgFullcharge.SettlingTime); 	
	
			// cell-balancing
			Ini_GetDouble(iniText, "Cellbalance", "ChargeVoltage", &btbdefault.CfgCellBalance.ChargeVoltage); 	
			Ini_GetDouble(iniText, "Cellbalance", "ChargeVoltage1CellFailed", &btbdefault.CfgCellBalance.ChargeVoltage1CellFailed);
			Ini_GetDouble(iniText, "Cellbalance", "ChargeCurrent", &btbdefault.CfgCellBalance.ChargeCurrent);	
			Ini_GetDouble(iniText, "Cellbalance", "DischargeVoltage", &btbdefault.CfgCellBalance.DischargeVoltage);
			Ini_GetDouble(iniText, "Cellbalance", "DischargeVoltage1CellFailed", &btbdefault.CfgCellBalance.DischargeVoltage1CellFailed);
			Ini_GetDouble(iniText, "Cellbalance", "DischargeCurrent", &btbdefault.CfgCellBalance.DischargeCurrent); 
			Ini_GetDouble(iniText, "Cellbalance", "DispersionLimit1", &btbdefault.CfgCellBalance.DispersionLimit1);
			Ini_GetDouble(iniText, "Cellbalance", "DispersionLimit2", &btbdefault.CfgCellBalance.DispersionLimit2);				
 			Ini_GetDouble(iniText, "Cellbalance", "DispersionLimit3", &btbdefault.CfgCellBalance.DispersionLimit3); 	
			Ini_GetDouble(iniText, "Cellbalance", "TaperingFactor", &btbdefault.CfgCellBalance.TaperingFactor); 	
			Ini_GetDouble(iniText, "Cellbalance", "ChargeCurrentLimit", &btbdefault.CfgCellBalance.ChargeCurrentLimit);
			Ini_GetDouble(iniText, "Cellbalance", "ChargingTime", &btbdefault.CfgCellBalance.ChargingTime);	
			Ini_GetDouble(iniText, "Cellbalance", "SafeTime", &btbdefault.CfgCellBalance.SettlingTime); 	
			Ini_GetDouble(iniText, "Cellbalance", "RepeatCount", &btbdefault.CfgCellBalance.RepeatCount);
			
			// end of storage
			Ini_GetDouble(iniText, "Endofstorage", "ChargeVoltage", &btbdefault.CfgEndOfStorage.ChargeVoltage); 
			Ini_GetDouble(iniText, "Endofstorage", "ChargeVoltage1CellFailed", &btbdefault.CfgEndOfStorage.ChargeVoltage1CellFailed);
			Ini_GetDouble(iniText, "Endofstorage", "ChargeCurrent", &btbdefault.CfgEndOfStorage.ChargeCurrent);				
 			Ini_GetDouble(iniText, "Endofstorage", "DischargeCellVoltageLimit", &btbdefault.CfgEndOfStorage.DischargeCellVoltageLimit); 	
			Ini_GetDouble(iniText, "Endofstorage", "DischargeCurrent", &btbdefault.CfgEndOfStorage.DischargeCurrent); 	
			Ini_GetDouble(iniText, "Endofstorage", "DispersionLimit1", &btbdefault.CfgEndOfStorage.DispersionLimit1);
			Ini_GetDouble(iniText, "Endofstorage", "RepeatCount", &btbdefault.CfgEndOfStorage.RepeatCount);	

			// file & graph			
			Ini_GetInt(iniText, "FileGraph", "Interval", &btbdefault.CfgFileGraph.Interval); 	
			Ini_GetInt(iniText, "FileGraph", "graph1", &btbdefault.CfgFileGraph.graph1);
			Ini_GetInt(iniText, "FileGraph", "graph2", &btbdefault.CfgFileGraph.graph2);
			
			// model
			Ini_GetInt(iniText, "Model", "ModelID", &btbdefault.ModelID);
			Ini_GetInt(iniText, "Model", "BatteryCellOption", &btbdefault.BatteryCellOption);

			// temperature
			Ini_GetDouble(iniText, "Temperature", "Limit1", &btbdefault.Temperature.limit1);
			Ini_GetDouble(iniText, "Temperature", "Limit2", &btbdefault.Temperature.limit2);

			// cell voltage limit
			Ini_GetDouble(iniText, "Cell", "UpLimit", &btbdefault.Cell.uplimit);
			Ini_GetDouble(iniText, "Cell", "DownLimit", &btbdefault.Cell.downlimit);
			
			// operator
			CIS_Ini_GetStringCopy(iniText, "Operator", "strSTRING", btbdefault.OperatorName); 
		
			// Decimal Point
			Ini_GetInt(iniText, "Decimal", "PointNum", &btbdefault.DecimalPoint);
			
			
			Ini_Dispose(iniText);
		
			ret = ENOERR;
		}
		else		// To get the default values from BTB initial setting if there's no user's configurations.
		{
			MessagePopup ("Error", "There is no the default configuration file");
			ret = -ENOENT;
		}
	}
	else
	{
		MessagePopup ("Error", "Default configuration file");
		ret = -ENOENT;
	}

	return ret;
}


/*---------------------------------------------------------------------------*/
/* File Save                                                                 */	
/*---------------------------------------------------------------------------*/

char *dec2string(int input)
{
	if(input)
		return "ON";
	else
		return "OFF";
}

char *auto2string(int input)
{
	switch(input)
	{
		case ManualRun:
			return "Manual";
			break;
		case AutoRun:
			return "Auto";
			break;
		case ChargerRelay:
			return "Charger";
			break;
		case DischargerRelay:
			return "Discharger";
			break;
		case Voltage:
			return "Voltage";
			break;
		case Temperature:
			return "Temperature";
			break;
		case Dispersion:
			return "Dispersion";
			break;
		default:
			return NULL;
			break;
	}
}

int CVICALLBACK FileSaveThread (void *functionData)
{
	static unsigned int timeCnt=0;
	char datecell[TEXT_SIZE];
	char file_subline[TEXT_SIZE];
	char PrjDir[TEXT_SIZE];
	char dataFileName[MAX_FILENAME_LEN];
	int num;

	while(1)
	{
		num = operator.DecimalPoint;
		if( (filesave_manualexecute==ON)||(filesave_autoexecute==ON) )
		{
			if(filesave_progress==INIT)
			{
				//File Save
				time(&ltime);
				newtime = localtime(&ltime);
				GetProjectDir (PrjDir);
				
				sprintf(file_name,"%4d%02d%02d_%02d%02d%02d.xls",newtime->tm_year+1900, newtime->tm_mon+1, newtime->tm_mday, newtime->tm_hour, newtime->tm_min, newtime->tm_sec);
				sprintf(dataFileName,"%s\\data\\%s",PrjDir,file_name);
				
				sprintf(file_subline,\
"Time\
\tModel\
\tOperator\
\tStatus\
\tCharge Realy\
\tDischarg Relay\
\tTarget Voltage\
\tInput Current\
\tPack Voltage\
\tPack Current\
\tDispersion\
\tCell1(V)\tCell2(V)\tCell3(V)\tCell4(V)\tCell5(V)\tCell6(V)\tCell7(V)\tCell8(V)\tCell9(V)\tCell10(V)\tCell11(V)\tCell12(V)\tCell13(V)\
\tTemperature\
\n");
				if((fp=fopen(dataFileName,"wt"))!=NULL) 
				{
					event_send (EventTabNum, TABPANEL_TEXTBOX,FileSaveON,NULL,NULL);
					fprintf(fp,file_subline);
					filesave_progress = RUN;	
				}
				else
				{
					MessagePopup ("File", "Open File Error!!!"); // To-Do : insert error status
					
				}
			}
			else if(filesave_progress == RUN)
			{
				//interval_value = operator.CfgFileGraph.Interval;
				if(timeCnt% operator.CfgFileGraph.Interval==0)
				{
					time(&ltime);
					newtime = localtime(&ltime); 
					sprintf(datecell,"%4d-%02d-%02d_%02d:%02d:%02d",newtime->tm_year+1900, newtime->tm_mon+1, newtime->tm_mday, newtime->tm_hour, newtime->tm_min, newtime->tm_sec);
					
					fprintf(fp,"%s\t%s\t%s\t%s\t%s\t%s\t%lf\t%lf\t%lf\t%lf\t%.*f\t%.*f\t%.*f\t%.*f\t%.*f\t%.*f\t%.*f\t%.*f\t%.*f\t%.*f\t%.*f\t%.*f\t%.*f\t%.*f \t%lf\n",datecell,BTBModel[operator.ModelID],operator.OperatorName,btb_status_string(BTBWorkingStatus), 
					dec2string(charge_flag),dec2string(discharge_flag),TargetVoltageValue,TargetCurrentValue,BTBPackVoltage,BTBPackCurrent,\
					num,cellDispersion,num,receiveData.CellVoltage[0],num,receiveData.CellVoltage[1],num,receiveData.CellVoltage[2],num,receiveData.CellVoltage[3],num,receiveData.CellVoltage[4],num,receiveData.CellVoltage[5],\
					num,receiveData.CellVoltage[6],num,receiveData.CellVoltage[7],num,receiveData.CellVoltage[8],num,receiveData.CellVoltage[9],num,receiveData.CellVoltage[10],num,receiveData.CellVoltage[11],num,receiveData.CellVoltage[12],\
					receiveData.BTBTemperature);
					
				}
				timeCnt++;
				// data needs to be inserted here...
			}
		}
		else		//(filesave_flag==OFF)
		{
			if(filesave_progress == RUN) 
			{
				fclose(fp);
				event_send (EventTabNum, TABPANEL_TEXTBOX,FileSaveOFF,NULL,NULL);
				filesave_progress = INIT;
			}
		}
		
		Delay(1);
	}
	return 0;
}

/*----------------------------------------------------------------------------*/
/* Fill up the passed array with data, and display the data in the table.     */
/*----------------------------------------------------------------------------*/
void event_send(int panel, int control, int statsNum, int data, char * message)
{
	char timeString[TIMESTRING_SIZE];
	char statsString[TEXT_SIZE];
	char cellv[TEXT_SIZE];
	char temp[TEXT_SIZE];
	char printData[TEXT_SIZE];	
	char informationString[TEXT_SIZE];
	int num;
	
	num = operator.DecimalPoint;

	memset(timeString,0,sizeof(timeString));
	memset(statsString,0,sizeof(statsString));
	memset(cellv,0,sizeof(cellv));
	memset(temp,0,sizeof(temp));
	memset(informationString,0,sizeof(informationString));
	memset(printData,0,sizeof(printData));
	
	/* Generate the string of the current time */
	time(&ltime);
	newtime = localtime(&ltime); 

	/* Generate the string of all cell voltage */
	for(int j=0;j<cellTotalCount;j++)
	{
		sprintf(temp,"%.*f ",num,receiveData.CellVoltage[j]);
		strcat(cellv,temp);
	}

	sprintf(timeString,"[%4d/%02d/%02d %02d:%02d:%02d] ",newtime->tm_year+1900, newtime->tm_mon+1, newtime->tm_mday, newtime->tm_hour, newtime->tm_min, newtime->tm_sec);
	
	switch(statsNum)
	{
		case ExcutionStart:
		case ExcutionStop:
				sprintf(informationString, "%s %.2f(V) %.2f(A) %.*f(V) %.2lf('C)\
				\n                                                     Cell Voltage: %s",btb_status_string(BTBWorkingStatus),\
				BTBPackVoltage, BTBPackCurrent, num,cellDispersion,receiveData.BTBTemperature,cellv);
				sprintf(statsString,"%s: [%s] ",EventStats[statsNum],auto2string(data));
				sprintf(printData, "%s%s%s\n",timeString,statsString,informationString);
			 break;
		case FileSaveON:
		case FileSaveOFF:
				sprintf(statsString,"%s",EventStats[statsNum]);
				sprintf(printData, "%s%s\n",timeString,statsString);
			 break;
		case RelayON:
		case RelayOFF:
				sprintf(statsString,"%s: [%s]",EventStats[statsNum],auto2string(data));
				sprintf(printData, "%s%s\n",timeString,statsString);
			 break;
		case Warning:
		case AlarmON:
		case AlarmOFF:
				sprintf(statsString,"%s: [%s] ",EventStats[statsNum],auto2string(data));
				sprintf(printData, "%s%s%s\n",timeString,statsString,message);
			break;
		case EventMessage:
				sprintf(printData, "%s%s\n",timeString,message);
			 break;
		case sError:
				sprintf(printData, "%s%s(e:%d)\n",timeString,message,data);
			 break;
	}
			
	SetCtrlVal (panel, control, printData);
}

int CVICALLBACK graph_packvoltageThread(void *functionData)
{
	float RunVoltage[2];
	static int check_onetime = ON;	
	int traces;
	int i;
	double currTime;
	static int scaleAutoValue;

	int panel;
	panel = VoltageTabNum;

	while(1)
	{
		if(connection_flag) 
		{
			RunVoltage[0]= BTBPackVoltage;
			RunVoltage[1]= TargetVoltageValue;
			
			if(check_onetime == ON)
			{
				ClearStripChart (panel, TABPANEL_2_STRIPCHART);
				GetCtrlAttribute(panel, TABPANEL_2_STRIPCHART, ATTR_NUM_TRACES, &traces);
				GetCurrentDateTime (&currTime);
				SetCtrlAttribute (panel, TABPANEL_2_STRIPCHART, ATTR_XAXIS_OFFSET, currTime);

				for (i = 1;i <= traces;i++)
				{
					SetTraceAttribute(panel, TABPANEL_2_STRIPCHART, i, ATTR_TRACE_LG_VISIBLE, 1);
				}
				scale_y_setup(panel, TABPANEL_2_STRIPCHART,NULL,NULL,NULL,NULL);
				check_onetime = OFF;
			}
			
			GetCtrlVal(panel,TABPANEL_2_SCALE_Y,&scaleAutoValue);
			if(!scaleAutoValue)
			{
				SetAxisScalingMode (panel, TABPANEL_2_STRIPCHART, VAL_LEFT_YAXIS, VAL_MANUAL, BTBPackVoltage-10, BTBPackVoltage+10);
			}

			PlotStripChart (panel, TABPANEL_2_STRIPCHART, RunVoltage, 2, 0, 0, VAL_FLOAT); 
		}
		else
		{
			ClearStripChart (panel, TABPANEL_2_STRIPCHART);
			check_onetime = ON;
		}

		Delay(nTimeUI);
	}
	
	return 0;
}

int CVICALLBACK scale_y_setup (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	static int scaleAutoValue;
	static double limit = 10.0;
	static int scale_old = 0;
	
	GetCtrlVal(panel,TABPANEL_2_SCALE_Y,&scaleAutoValue);
	

	switch (event)
	{
		case EVENT_COMMIT:
			if(!scaleAutoValue)
			{
				if(scale_old != scaleAutoValue)
				{
					SetAxisScalingMode (panel, TABPANEL_2_STRIPCHART, VAL_LEFT_YAXIS, VAL_MANUAL, BTBPackVoltage-10, BTBPackVoltage+10);
					limit = 10.0;
					scale_old = scaleAutoValue;
				}
			}
			break;
		case EVENT_MOUSE_WHEEL_SCROLL:
			if(scaleAutoValue)
			{
				if(eventData1==MOUSE_WHEEL_SCROLL_UP)
				{
					if(limit<30)
					{
						if(limit>0.2)
						{
							limit+=0.1;
						}
						else
						{
							limit+=0.001;
						}
					}
					SetAxisScalingMode (panel, TABPANEL_2_STRIPCHART, VAL_LEFT_YAXIS, VAL_MANUAL, BTBPackVoltage-limit, BTBPackVoltage+limit);
				}
				else if(eventData1==MOUSE_WHEEL_SCROLL_DOWN)
				{
					if(limit>0.2)
					{
						limit-=0.1;
					}
					else if(limit<=0.2&&limit>0.0001)
					{
						limit-=0.001;
					}
					SetAxisScalingMode (panel, TABPANEL_2_STRIPCHART, VAL_LEFT_YAXIS, VAL_MANUAL, BTBPackVoltage-limit, BTBPackVoltage+limit);
				}
				
			}
		break;
	}

	return 0;
}
int CVICALLBACK scale_x_setup (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	static int limit=0;
	static int scale_old = 0;
	int scaleAutoValue;

	GetCtrlVal(panel,TABPANEL_2_SCALE_X,&scaleAutoValue);

	switch (event)
	{
		case EVENT_COMMIT:
			if(!scaleAutoValue)
			{
				SetCtrlAttribute(panel,TABPANEL_2_STRIPCHART,ATTR_POINTS_PER_SCREEN,CHART_POINTS);
				limit=0;
				scale_old = scaleAutoValue;
			}
			break;
		case EVENT_MOUSE_WHEEL_SCROLL:
			if(scaleAutoValue)
			{
				if(eventData1==MOUSE_WHEEL_SCROLL_UP)
				{
					if((CHART_POINTS+limit)< 9900) 		// limit point is 10000
					{
						limit += 60;
						SetCtrlAttribute(panel,TABPANEL_2_STRIPCHART,ATTR_POINTS_PER_SCREEN,(CHART_POINTS+limit));
					}
				}
				else if(eventData1==MOUSE_WHEEL_SCROLL_DOWN)
				{
					if((CHART_POINTS+limit)>121)
					{
						limit -= 60;
						SetCtrlAttribute(panel,TABPANEL_2_STRIPCHART,ATTR_POINTS_PER_SCREEN,(CHART_POINTS+limit));
					}
				}
			}
			break;
	}
	return 0;
}

int CVICALLBACK graph_cellvoltageThread(void *functionData)
{
	float cellVoltageValue[CELLMAXCOUNT+1];
	int i;
	int scaleAutoValue; 
	double scaleManualMaxValue;
	double scaleManualMinValue;

	while(1)
	{
		#if 0
		if(connection_flag) 
		{
			cellVoltageValue[0] = 0;
			for(i=0;i<CELLMAXCOUNT;i++)
			{
				cellVoltageValue[i+1] = receiveData.CellVoltage[i];
				SetCtrlVal(CellVoltageTabNum,(TABPANEL_3_NUMERIC_13+i),cellVoltageValue[i+1]);
			}
			
			GetCtrlVal(CellVoltageTabNum,TABPANEL_3_BINARYSWITCH,&scaleAutoValue);
			if(scaleAutoValue)
			{
				SetAxisScalingMode (CellVoltageTabNum, TABPANEL_3_GRAPH, VAL_LEFT_YAXIS, VAL_MANUAL, cellVoltageMin-0.1, cellVoltageMax+0.02);	
				SetCtrlAttribute(CellVoltageTabNum,TABPANEL_3_CELLVOLTAGEMAX,ATTR_DIMMED,ON);
				SetCtrlAttribute(CellVoltageTabNum,TABPANEL_3_CELLVOLTAGEMIN,ATTR_DIMMED,ON);
			}
			else
			{
				SetCtrlAttribute(CellVoltageTabNum,TABPANEL_3_CELLVOLTAGEMAX,ATTR_DIMMED,OFF);
				SetCtrlAttribute(CellVoltageTabNum,TABPANEL_3_CELLVOLTAGEMIN,ATTR_DIMMED,OFF);
				GetCtrlVal(CellVoltageTabNum,TABPANEL_3_CELLVOLTAGEMAX,&scaleManualMaxValue);
				GetCtrlVal(CellVoltageTabNum,TABPANEL_3_CELLVOLTAGEMIN,&scaleManualMinValue);
				SetAxisScalingMode (CellVoltageTabNum, TABPANEL_3_GRAPH, VAL_LEFT_YAXIS, VAL_MANUAL, (0+scaleManualMinValue), (4.16+scaleManualMaxValue));
			}

			PlotY (CellVoltageTabNum, TABPANEL_3_GRAPH,cellVoltageValue , (CELLMAXCOUNT+1), VAL_FLOAT, VAL_BASE_ZERO_VERTICAL_BAR, VAL_EMPTY_SQUARE, VAL_SOLID,1, VAL_LT_GRAY);
		}
		#endif
		
		Delay(nTimeUI);
	}
	return 0;
}

void CVICALLBACK cmdOpenExeclCB (int menuBar, int menuItem, void *callbackData,int panel)
{
	int  stat;
	char fileName[MAX_PATHNAME_LEN];

	/* Get fileName from user */    
	stat = FileSelectPopupEx ("\data", "*.*", "", "Choose a file to open", VAL_LOAD_BUTTON, 0, 0, fileName);
	
	if((stat == VAL_EXISTING_FILE_SELECTED)|| (stat == VAL_NEW_FILE_SELECTED))
	{

		SetCtrlVal(panel,PANEL_TEXTMSG_6,fileName);
		file_open(fileName);
		cmdFileChartCB(NULL,NULL,NULL,NULL);
	}
}

void file_open(char* fileName)
{
	char PrjDir[TEXT_SIZE];
	char *categoryAxisTitle = NULL;
	char *valueAxisTitle = NULL;
	HRESULT error = 0;  

	if(*fileName == NULL)
	{
		MessagePopup ("File", "There is no file.");
	}
	else
	{
		error = ExcelRpt_ApplicationNew(1, &applicationHandle);
		if (error<0) 
		{
			MessagePopup ("Error:  Microsoft Excel", MSG_EEXCEL);
			event_send(EventTabNum, TABPANEL_TEXTBOX, sError, EEXL, MSG_EEXCEL);
		}
		else
		{
			ExcelRpt_WorkbookOpen (applicationHandle,fileName,&workbookHandle);
			ExcelRpt_GetWorksheetFromIndex(workbookHandle, 1, &worksheetHandle);
			ExcelRpt_ChartAddtoWorksheet (worksheetHandle, 2.0 * 72, 7.0,8.0*72, 6.0*72, &chartHandle);
	        ExcelRpt_ChartActivate (chartHandle);
	        ExcelRpt_ChartSetSourceRange (chartHandle, worksheetHandle, "I:J" ) ;      
	        ExcelRpt_SetChartAttribute (chartHandle, ER_CH_ATTR_CHART_TYPE,  PlotType ) ;
	        ExcelRpt_SetChartAttribute (chartHandle, ER_CH_ATTR_CHART_PLOT_BY,  ExRConst_Columns ) ;
	        ExcelRpt_SetChartAttribute (chartHandle, ER_CH_ATTR_CHART_TITLE,  "BTB Data" ) ;
	        ExcelRpt_SetChartAttribute (chartHandle, ER_CH_ATTR_PLOTAREA_COLOR,0xffffff);
	        ExcelRpt_SetChartAxisAttribute (chartHandle, ExRConst_Category,ExRConst_Primary,ER_CH_ATTR_AXIS_TITLE, "Data" ) ;     //Category = X-axis
	        ExcelRpt_SetChartAxisAttribute (chartHandle, ExRConst_Value,ExRConst_Primary,ER_CH_ATTR_AXIS_TITLE, "Value" ) ;     //Values = Y-axis
		}
	}
}


void CVICALLBACK cmdFileChartCB (int menuBar, int menuItem, void *callbackData,int panel)
{
	char excelFileName[MAX_PATHNAME_LEN];
	char PrjDir[TEXT_SIZE];
	char *categoryAxisTitle = NULL;
	char *valueAxisTitle = NULL;
	HRESULT error = 0;  

	if(filesave_progress == RUN)
	{
		MessagePopup ("File Save", "Working File Save now.");
	}
	else
	{
		if(*file_name!=NULL)
		{
			error = ExcelRpt_ApplicationNew(1, &applicationHandle);
			if (error<0) 
			{
				MessagePopup ("Error:  Microsoft Excel", MSG_EEXCEL);
				event_send(EventTabNum, TABPANEL_TEXTBOX, sError, EEXL, MSG_EEXCEL);
			}
			else
			{
				GetProjectDir (PrjDir);
				sprintf(excelFileName,"%s\\data\\%s",PrjDir,file_name);
				ExcelRpt_WorkbookOpen (applicationHandle,excelFileName,&workbookHandle);
				ExcelRpt_GetWorksheetFromIndex(workbookHandle, 1, &worksheetHandle);
				ExcelRpt_ChartAddtoWorksheet (worksheetHandle, 2.0 * 72, 7.0,8.0*72, 6.0*72, &chartHandle);
		        ExcelRpt_ChartActivate (chartHandle);
		        ExcelRpt_ChartSetSourceRange (chartHandle, worksheetHandle, "I:J" ) ;      
		        ExcelRpt_SetChartAttribute (chartHandle, ER_CH_ATTR_CHART_TYPE,  PlotType ) ;
		        ExcelRpt_SetChartAttribute (chartHandle, ER_CH_ATTR_CHART_PLOT_BY,  ExRConst_Columns ) ;
		        ExcelRpt_SetChartAttribute (chartHandle, ER_CH_ATTR_CHART_TITLE,  "BTB Data" ) ;
		        ExcelRpt_SetChartAttribute (chartHandle, ER_CH_ATTR_PLOTAREA_COLOR,0xffffff);
		        ExcelRpt_SetChartAxisAttribute (chartHandle, ExRConst_Category,ExRConst_Primary,ER_CH_ATTR_AXIS_TITLE, "Data" ) ;     //Category = X-axis
		        ExcelRpt_SetChartAxisAttribute (chartHandle, ExRConst_Value,ExRConst_Primary,ER_CH_ATTR_AXIS_TITLE, "Value" ) ;     //Values = Y-axis
			}
		}
	}
}

void CVICALLBACK cmdEventHistoryClearCB (int menuBar, int menuItem, void *callbackData, int panel)
{
	ResetTextBox (EventTabNum, TABPANEL_TEXTBOX, ""); // clear text box
}

int CVICALLBACK timertest (int panel, int control, int event,
						   void *callbackData, int eventData1, int eventData2)
{

	float cellVoltageValue[CELLMAXCOUNT+1];
	int i;
	int scaleAutoValue; 
	double scaleManualMaxValue;
	double scaleManualMinValue;
#if 1	
	switch (event)
	{
		case EVENT_TIMER_TICK:
		if(connection_flag) 
		{
			cellVoltageValue[0] = 0;
			for(i=0;i<CELLMAXCOUNT;i++)
			{
				cellVoltageValue[i+1] = receiveData.CellVoltage[i];
				SetCtrlVal(CellVoltageTabNum,(TABPANEL_3_NUMERIC_13+i),cellVoltageValue[i+1]);
			}
			
			GetCtrlVal(CellVoltageTabNum,TABPANEL_3_BINARYSWITCH,&scaleAutoValue);
			if(scaleAutoValue)
			{
				SetAxisScalingMode (CellVoltageTabNum, TABPANEL_3_GRAPH, VAL_LEFT_YAXIS, VAL_MANUAL, cellVoltageMin-0.1, cellVoltageMax+0.02);	
				SetCtrlAttribute(CellVoltageTabNum,TABPANEL_3_CELLVOLTAGEMAX,ATTR_DIMMED,ON);
				SetCtrlAttribute(CellVoltageTabNum,TABPANEL_3_CELLVOLTAGEMIN,ATTR_DIMMED,ON);
			}
			else
			{
				SetCtrlAttribute(CellVoltageTabNum,TABPANEL_3_CELLVOLTAGEMAX,ATTR_DIMMED,OFF);
				SetCtrlAttribute(CellVoltageTabNum,TABPANEL_3_CELLVOLTAGEMIN,ATTR_DIMMED,OFF);
				GetCtrlVal(CellVoltageTabNum,TABPANEL_3_CELLVOLTAGEMAX,&scaleManualMaxValue);
				GetCtrlVal(CellVoltageTabNum,TABPANEL_3_CELLVOLTAGEMIN,&scaleManualMinValue);
				SetAxisScalingMode (CellVoltageTabNum, TABPANEL_3_GRAPH, VAL_LEFT_YAXIS, VAL_MANUAL, (0+scaleManualMinValue), (4.16+scaleManualMaxValue));
			}

			PlotY (CellVoltageTabNum, TABPANEL_3_GRAPH,cellVoltageValue , (CELLMAXCOUNT+1), VAL_FLOAT, VAL_BASE_ZERO_VERTICAL_BAR, VAL_EMPTY_SQUARE, VAL_SOLID,1, VAL_LT_GRAY);
		}
			break;
	}
#endif
	return 0;
}
