/**************************************************************************/
/*     BTB Controller Include File                          			  */
/*     Ver. 9.0.001                                                       */
/**************************************************************************/

#ifndef __Definitions__
#define __Definitions__

#include <stdbool.h>
#ifdef __cplusplus
    extern "C" {
#endif


/***********************************************************/		
/************************** Defines ************************/
/***********************************************************/
#define HELPTEXT "\
        This system for the multipurpose satellite No. 7 Battery Test Bench is designed to store and manage\n\
              batteries in optimal condition during the ground storage process of the satellite batteries,\n\
 check battery performance after storage/management of batteries, and support battery operation at the launch site.\n\n\
                                       Version 9.0.001 Built on 2019-11-15\n\
                                        Copyright (c) 2019 by BnCTek, Inc."


#define MSG_INIT 				"Initializing BTB Control System"
#define MSG_UDP_START 			"Initializing UDP to comunicate with I/F box"
#define MSG_TCP_DISCONNECT 		"Disconnected from I/F box..."
#define MSG_CHARGER_ON 			"CHARGER: A-N8737A-5444R on board"
#define MSG_DISCHARGER_ON 		"DISCHARGER: C-63203A-150-300 on board"

#define MSG_ERROR1				"Error: UDP registration failed!"
#define MSG_ERROR2				"Error: an error occured receiving data from I/F box."
#define MSG_ERROR3				"Error: an error occured getting the current directory name on the Control PC."
#define MSG_ERROR4				"Error: an error occured getting default setting values."
#define MSG_ERROR5				"Error: an error occured trying to initialize charger."
#define MSG_ERROR6				"Error: an error occured trying to initialize discharger."
#define MSG_ERROR7				"Error: There is information from IF box!"
#define MSG_ERROR8				"Error: an error occured trying to turn on the charger relay."
#define MSG_ERROR9				"Error: an error occured trying to turn on the discharger relay."
#define MSG_ERROR10				"Error: an error occured trying to turn off the charger relay."
#define MSG_ERROR11				"Error: an error occured trying to turn off the discharger relay."
#define MSG_ERROR12				"Error: problem detected on the charger voltage."
#define MSG_ERROR13				"Error: problem detected on the charger current."
#define MSG_ERROR14				"Error: problem detected on the discharger voltage."
#define MSG_ERROR15				"Error: problem detected on the discharger current."
#define MSG_ERROR16				"Error: problem detected sending data to I/F box."
#define MSG_ERROR17				"Error: problem detected on the received interface data."
#define MSG_ERROR18				"Error: problem detected on the charger & discharger's relay."

#define MSG_ERROR100			"Emergency button pushed!!!"

#define MSG_OVDISPER			"Over the Dispersion limit(100mV)."
#define MSG_EEXCEL				"An error occurred trying to launch Excel."

#define CONNECTION_LIMIT_TIME		5		// if there's no data from i/f box within *s, Alarm!!!
#define ON							1
#define OFF							0

#define CELL_UP_LIMIT				4.1
#define CELL_DOWN_LIMIT				2.68  //2.7
#define NORMAL_UP_LIMIT				49.2
#define NORMAL_DOWN_LIMIT			30.1  //36.0
#define ONECELLFAILED_UP_LIMIT		45.1
#define ONECELLFAILED_DOWN_LIMIT	33.0

#define BTBSTATUSLENGTH				10
#define BTBMODELLENGTH				5
#define X_DATA_POINTS 				15
#define Y_DATA_POINTS				15
		
#define TIMESTRING_SIZE				32
#define TEXT_SIZE					500
#define LABEL_SIZE					15
#define VAL_FRAME_GRAY2				0xE0E0E0L /* 16 standard colors */ 
#define VAL_FRAME_GRAY				0xF0F0F0L
#define VAL_FRAME_BATT				0xFFCC66L 
#define VAL_CELL_BGTEXTCOLOR		0xC64439L
#define VAL_TEMP_COLOR				0x00CCFFL

#define CELLMAXCOUNT				13
#define NAMELEN						100
#define MESSAGE_LEN					100

#define PORT_NUM					10000
#define READER_PORT					60000	
#define WRITER_PORT					60001
#define MAX_IP_INPUT_LENGTH			20    
#define STATS_STR_LEN				40
#define MODEL_STR_LEN				20

#define CHART_POINTS				3601	// 1 hour range
#define nTimeUI						1 		// Interval time(sec) for UI display

#define EventTabNum					4
#define VoltageTabNum				5
#define CellVoltageTabNum			7

#define CELL_NUM_12					12
#define CELL_NUM_13					13

#define mV							1000

#define MODE_CC						1
#define MODE_CV						2
#define MODE_CR						3
#define MODE_CP						4

/* Charger and Discharger */
#define AGN8700						1
#define CHR63200					2
//#define _MODE_CV_	//for starting on cv mode with charger
//#define DEVICE_CHECK	// to check the working status of the devices
#define VOLT_DIFF					5
#define CURR_DIFF					5

#define IFBOX_IP					UDP_LAN_BROADCAST_ADDR		//"192.168.1.12"	//UDP_LAN_BROADCAST_ADDR //"192.168.1.12"
#define BTBSYSTEM_IP				UDP_ANY_ADDRESS				//"192.168.1.11"	//UDP_ANY_ADDRESS //"192.168.1.11"

#define CHARGER_NAME				"TCPIP0::192.168.1.101::inst0::INSTR"	//charger name(TCP): "A-N8737A-5444R.local" and ethernet device "inst0"		
#define DISCHARGER_NAME				"TCPIP0::192.168.1.102::2101::SOCKET"	//"TCPIP::C-63203A-150-300-63203AL00745.local::inst0::INSTR"

/* BTB-Identifier */
#ifndef _BTB_GENERIC_ERRNO_BASE_H
#define _BTB_GENERIC_ERRNO_BASE_H

#define ENOERR		 0
#define	EPERM		 1	/* Operation not permitted */
#define	ENOENT		 2	/* No such file or directory */
#define	ESRCH		 3	/* No such process */
#define EDEV		 4	/* Invalid charger or discharger */
#define EEXL		 5  /* error occured in excel file */
#endif

/* I/F box communicate information */
#define SR	0x0001
#define SP	0x0002
#define BR	0x0004
#define BP	0x0008
#define ER	0x0010
#define	EP	0x0020
#define EM	0x0100

#define CHARGER_BIT		0x02
#define DISCHARGER_BIT	0x04


/***********************************************************/		
/****************** Method Configurations ******************/
/***********************************************************/
// For Charge
typedef struct
{
	double ChargeVoltage;
	double ChargeVoltage1CellFailed;
	double ChargeCurrent;
	int ChargeCurrentSelect;
} Config_Charge_type;

// For Discharge
typedef struct
{
	double DischargeVoltage;
	double DischargeVoltage1CellFailed;
	double DischargeCurrent;
} Config_Discharge_type;

// For Full Charge
typedef struct
{
	double	ChargeVoltage;
	double	ChargeVoltage1CellFailed;
	double	ChargeCurrent;
	double	DispersionLimit1;
	double	TaperingFactor;
	double	ChargeCurrentLimit;
	double	SettlingTime; 
} Config_Fullcharge_type;  

// For Cell Balancing
typedef struct
{
	double ChargeVoltage;
	double ChargeVoltage1CellFailed;
	double ChargeCurrent;  	
	double DischargeVoltage;
	double DischargeVoltage1CellFailed;
	double DischargeCurrent;
	double DispersionLimit1;
	double DispersionLimit2;
	double DispersionLimit3;
	double TaperingFactor;
	double ChargeCurrentLimit;
	double ChargingTime;
	double SettlingTime;
	double RepeatCount;
} Config_Cellbalance_type;

// For End of Storage
typedef struct
{
	double ChargeVoltage;
	double ChargeVoltage1CellFailed;
	double ChargeCurrent;  	
	double DischargeCellVoltageLimit;
	double DischargeCurrent;
	double DispersionLimit1;
	double RepeatCount;
} Config_Endofstorage_type;

// I/F box data via TCP Client
typedef struct {
	/*
	 * Model ID contains 4 bytes
	 * K6FM 		= 0x00000001
	 * K7FM 		= 0x00000002
	 * EOIRSatFM 	= 0x00000003
	 * K7AFM		= 0x00000004
	 * KOMPSAT		= 0x00000005
	 */
	float	prefix;
	unsigned int ModelID;
	unsigned int BTBStatus;
	float BTBTemperature;
	float BTBVoltage;
	float BTBCurrent;
	float CellVoltage[CELLMAXCOUNT];
	float CellVoltageDispersion;
	float DispersionMax;
	float DispersionMin;
	unsigned int charger_relay;		// ON:1, OFF:0
	unsigned int discharger_relay;	// ON:1, OFF:0
	unsigned short	front_switch;
} IFBoardData_type;

// Information of System Control to handle the BTB System
typedef struct {
	unsigned int	prefix;
	unsigned int	msg_no;
	unsigned int	reserved;
	unsigned int	intf_ctrl;
	float   ChargeCurrent;		// To-Do delete: no needs to use it but only for simulating
	float	DischargeCurrent;	// To-Do delete: no needs to use it but only for simulating
} ControlPCData_type;

typedef struct{
	int Interval;
	int graph1;
	int graph2;
} FileGraph_type;

typedef struct{
	double limit1;
	double limit2;
} Temperature_type;

typedef struct{
	double uplimit;
	double downlimit;
} CellVLimit_type;


/***********************************************************/		
/*********************    INI File    **********************/
/***********************************************************/
typedef struct
{
	Config_Charge_type CfgCharge;
	Config_Discharge_type CfgDischarge;
	Config_Fullcharge_type CfgFullcharge;
	Config_Cellbalance_type CfgCellBalance;
	Config_Endofstorage_type CfgEndOfStorage;
	FileGraph_type CfgFileGraph;
	enum modelnum ModelID;
	int BatteryCellOption;
	char OperatorName[NAMELEN];
	Temperature_type Temperature;
	CellVLimit_type Cell;
	int DecimalPoint;
}ConfigValue;

ConfigValue operator;
ConfigValue btbdefault;

/********************* End of INI File *********************/ 
/***********************************************************/


/***********************************************************/		
/*********************  Define Panel Handler  **************/
/***********************************************************/

int panelHandle;
int panelCFGCharging;
int panelCFGDischarging;
int panelCFGFullcharging;
int panelCFGCellbalancing;
int panelCFGEndofstorage;
int panelInterval;
int panelModelList;
int panelOperator;
int panelTemperature;
int panelCellVoltageLimit;
int panelDecimalPoint;
int panelErrMessage;

unsigned int 	readerChannel;	// it's used for UDP read channel
unsigned int 	writerChannel;	// it's used for UDP write channel

CmtThreadLockHandle gLockRandom;


/***********************************************************/		
/******************   BTB Status & String types ************/
/***********************************************************/
typedef enum
{
	STS_GETVALUE = 			0,
	STS_INIT = 				1,
	STS_CONNECT = 			2,
	STS_DEVICEINIT = 		3,
	STS_STANDBY = 			4,
	STS_CHARGING = 			5,
	STS_DISCHARGING = 		6,
	STS_FULLCHARGING = 		7,
	STS_CELLBALANCING = 	8,
	STS_ENDOFSTORAGE = 		9,
	STS_DISCONNECT = 		10,
	STS_ALARM1 = 	   		11, //voltage alarm
	STS_ALARM2 = 	   		12, //temperature alarm
	STS_ALARM3 = 	   		13, //etc
	STS_ERROR = 	   		14,
	STS_OUTOFSTATUS =  		15,
} btb_status;

typedef enum 
{
	K6FM 		= 0x00000001,
	K7FM 		= 0x00000002,
	EOIRSatFM 	= 0x00000003,
	K7AFM		= 0x00000004,
	KOMPSAT		= 0x00000005,
} modelnum;

typedef enum
{
	Dispcheck_Fullcharge,
	Charging_Fullcharge,
	Tapering_Fullcharge,
	Waiting_Fullcharge,
	End_Fullcharge,
} fullchargestatus;

typedef enum 
{
	Dispcheck1_Cellbalance,
	Charging1_Cellbalance,
	Tapering_Cellbalance,
	Charging2_Cellbalance,
	Waiting_Cellbalance,
	Dispcheck2_Cellbalance,
	Discharging_Cellbalance,
	End_Cellbalance,
} cellbalancingstatus;
	
typedef enum 
{
	Dispcheck_Endofstorage,
	Charging_Endofstorage,
	Tapering_Endofstorage,
	Discharging_Endofstorage,
	End_Endofstorage,
} endofstoragestatus;
	
typedef enum 
{
	normal,
	onecellfailed,
} celloption;

typedef enum
{
	FileSaveON,
	FileSaveOFF,
	ExcutionStart,
	ExcutionStop,
	RelayON,
	RelayOFF,
	Warning,
	AlarmON,
	AlarmOFF,
	EventMessage,
	sError,
};

typedef enum
{
	ManualRun = 		1,
	AutoRun = 			2,
	ChargerRelay = 		3,
	DischargerRelay = 	4,
	Voltage = 			5,
	Temperature = 		6,
	Dispersion = 		7,
};

typedef enum
{
	INIT = 		0,
	RUN = 		1,
	FINISHED = 	2,
};


/*----------------------------------------------------------------------------*/
/* Global Variables                                                           */
/*----------------------------------------------------------------------------*/
ControlPCData_type ControlData; 
unsigned int g_hconversation; 

int connection_flag;
int methodexecution_flag;
int charge_flag;
int discharge_flag;

unsigned __int64 timeStartBTB, timeElapsed;

enum btb_status BTBWorkingStatus;
char *btb_status_string(int status);

unsigned int OptionValue;
int btb_option;

float BTBPackVoltage;
float BTBPackCurrent;
float cellDispersion;
float dispMaxValue;
float dispMinValue;
float cellVoltageMin;
float cellVoltageMax;
float TargetVoltageValue;
float TargetCurrentValue;

unsigned int charger_relay;		// ON:1, OFF:0
unsigned int discharger_relay;	// ON:1, OFF:0

int TargetMode;
double ampereHour;
double energy;
double resistance;
int	dispMaxIndex;
int dispMinIndex;
int interval_value;
int cellTotalCount;
int modelValue; 
int filesave_manualexecute; 
int filesave_autoexecute;
int filesave_progress;

unsigned int charger_live;
unsigned int discharger_live;
bool emergency_bt;

/*----------------------------------------------------------------------------*/
/* Functions                                                                  */
/*----------------------------------------------------------------------------*/

int CVICALLBACK UDPReader (unsigned handle, int event, int error, void *callbackData);
void data_receive(void);

int CVICALLBACK BTBWorkingThread(void *functionData);
int CVICALLBACK ControlHandlingThread(void *functionData);
int CVICALLBACK IFBoxConnectCheckingThread(void *functionData);
int CVICALLBACK BTBCheckingThread(void *functionData);
int CVICALLBACK ControlAllDeviceThread(void *functionData);
int CVICALLBACK FileSaveThread(void *functionData);
int CVICALLBACK UIDisplayThread(void *functionData);
int CVICALLBACK graph_packvoltageThread(void *functionData);
int CVICALLBACK graph_cellvoltageThread(void *functionData);
int CVICALLBACK RelayCheckingThread(void *functionData);

void btb_status_change(btb_status status); 
int ini_save(char *filepath);
int ini_open_user(char *filepath);
int ini_open_default(char *filepath);
void panel_setting_value(void); 
int  FileSaveHandlingFunc(int Interval);
void event_send(int panel, int control, int statsNum, int data,char * message); 
void file_open(char* fileName);
void cell_number_setup(void);

void func_charging(void);
void func_discharging(void);
void func_fullcharging(void);
void func_cellbalancing(void);
void func_endofstorage(void);

void excution_stop(int);
char *dec2string(int);
char *auto2string(int);

float ch_device_init();
bool charger_on();
bool charger_off(void);
bool discharger_on();
bool discharger_off(void);
void ch_device_close();
void ch_device_off();

#ifdef DEVICE_CHECK
void charger_checking(void);
void discharger_checking(void);
void charger_checking_target_current(void);
void discharger_checking_target_current(void);
#endif

void display_charger_measurment(void);
void display_discharger_measurment(void);
void SetErrorMessage(char* message, int data, int mode);

void picture_all_relay_off(void);
void picture_charger_relay_on(void);
void picture_charger_relay_off(void);
void picture_discharger_relay_on(void);
void picture_discharger_relay_off(void);


#ifdef __cplusplus
    }
#endif

#endif  /* ndef __Definitions__ */

