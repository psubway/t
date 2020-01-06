/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                            1       /* callback function: cmdMainPNLExit */
#define  PANEL_NUMERIC_17                 2       /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_16                 3       /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_15                 4       /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_22                 5       /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_14                 6       /* control type: numeric, callback function: (none) */
#define  PANEL_CELL1                      7       /* control type: numeric, callback function: (none) */
#define  PANEL_CELL2                      8       /* control type: numeric, callback function: (none) */
#define  PANEL_CELL3                      9       /* control type: numeric, callback function: (none) */
#define  PANEL_CELL4                      10      /* control type: numeric, callback function: (none) */
#define  PANEL_CELL5                      11      /* control type: numeric, callback function: (none) */
#define  PANEL_CELL6                      12      /* control type: numeric, callback function: (none) */
#define  PANEL_CELL7                      13      /* control type: numeric, callback function: (none) */
#define  PANEL_CELL8                      14      /* control type: numeric, callback function: (none) */
#define  PANEL_CELL9                      15      /* control type: numeric, callback function: (none) */
#define  PANEL_CELL10                     16      /* control type: numeric, callback function: (none) */
#define  PANEL_CELL11                     17      /* control type: numeric, callback function: (none) */
#define  PANEL_CELL12                     18      /* control type: numeric, callback function: (none) */
#define  PANEL_CELL13                     19      /* control type: numeric, callback function: (none) */
#define  PANEL_CONNECTED                  20      /* control type: LED, callback function: (none) */
#define  PANEL_NUMERICTHERM               21      /* control type: scale, callback function: (none) */
#define  PANEL_METHODOPTION               22      /* control type: tree, callback function: selectMethodRadio */
#define  PANEL_CONFIGSETENDOFSTORAGE      23      /* control type: command, callback function: cmdSetValueEndOfStorageCB */
#define  PANEL_CONFIGSETCELLBAL           24      /* control type: command, callback function: cmdSetValueCellBalanceCB */
#define  PANEL_CONFIGSETFULLCHARGE        25      /* control type: command, callback function: cmdSetValueFullChargeCB */
#define  PANEL_CONFIGSETDISCHARGE         26      /* control type: command, callback function: cmdSetValueDischargeCB */
#define  PANEL_CONFIGSETCHARGE            27      /* control type: command, callback function: cmdSetValueChargeCB */
#define  PANEL_BTBMODELNAME               28      /* control type: ring, callback function: cmdModelIDCB */
#define  PANEL_CELLUSABILITY              29      /* control type: ring, callback function: cmdOptionCB */
#define  PANEL_FILESAVEBT                 30      /* control type: textButton, callback function: cmdFileSaveCB */
#define  PANEL_EXECUTION                  31      /* control type: textButton, callback function: cmdExecutionCB */
#define  PANEL_TAB                        32      /* control type: tab, callback function: (none) */
#define  PANEL_STRING                     33      /* control type: string, callback function: (none) */
#define  PANEL_DECORATION_6               34      /* control type: deco, callback function: (none) */
#define  PANEL_DECORATION_7               35      /* control type: deco, callback function: (none) */
#define  PANEL_DECORATION_2               36      /* control type: deco, callback function: (none) */
#define  PANEL_BTBSTATUS                  37      /* control type: string, callback function: (none) */
#define  PANEL_DECORATION_3               38      /* control type: deco, callback function: (none) */
#define  PANEL_DECORATION_8               39      /* control type: deco, callback function: (none) */
#define  PANEL_DECORATION_4               40      /* control type: deco, callback function: (none) */
#define  PANEL_TEXTMSG_3                  41      /* control type: textMsg, callback function: (none) */
#define  PANEL_DISCHARGECURRENT           42      /* control type: numeric, callback function: (none) */
#define  PANEL_RESISTANCE                 43      /* control type: numeric, callback function: (none) */
#define  PANEL_ENERGY                     44      /* control type: numeric, callback function: (none) */
#define  PANEL_AMPEREHOUR                 45      /* control type: numeric, callback function: (none) */
#define  PANEL_DISCHARGEVOLTAGE           46      /* control type: numeric, callback function: (none) */
#define  PANEL_CHARGECURRENT              47      /* control type: numeric, callback function: (none) */
#define  PANEL_CHARGEVOLTAGE              48      /* control type: numeric, callback function: (none) */
#define  PANEL_DECORATION_11              49      /* control type: deco, callback function: (none) */
#define  PANEL_TEXTMSG_4                  50      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_5                  51      /* control type: textMsg, callback function: (none) */
#define  PANEL_PICTURE                    52      /* control type: picture, callback function: (none) */
#define  PANEL_SPLITTER                   53      /* control type: splitter, callback function: (none) */
#define  PANEL_PICTURE_2                  54      /* control type: picture, callback function: (none) */
#define  PANEL_TEXTMSG                    55      /* control type: textMsg, callback function: (none) */
#define  PANEL_SPLITTER_2                 56      /* control type: splitter, callback function: (none) */
#define  PANEL_DECORATION_9               57      /* control type: deco, callback function: (none) */
#define  PANEL_PICTURE_4                  58      /* control type: picture, callback function: (none) */
#define  PANEL_DECORATION_10              59      /* control type: deco, callback function: (none) */
#define  PANEL_DISCHARGER_LED             60      /* control type: LED, callback function: (none) */
#define  PANEL_PICTURE_CHARGEON           61      /* control type: picture, callback function: (none) */
#define  PANEL_CHARGER_LED                62      /* control type: LED, callback function: (none) */
#define  PANEL_NUMERIC_23                 63      /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_24                 64      /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC                    65      /* control type: numeric, callback function: (none) */
#define  PANEL_LED_SMARTR                 66      /* control type: LED, callback function: (none) */
#define  PANEL_LED_BYPASSR                67      /* control type: LED, callback function: (none) */
#define  PANEL_LED_EMG                    68      /* control type: LED, callback function: (none) */
#define  PANEL_LED_SMARTP                 69      /* control type: LED, callback function: (none) */
#define  PANEL_LED_ELECR                  70      /* control type: LED, callback function: (none) */
#define  PANEL_LED_BYPASSP                71      /* control type: LED, callback function: (none) */
#define  PANEL_LED_ELECP                  72      /* control type: LED, callback function: (none) */
#define  PANEL_TEXTMSG_2                  73      /* control type: textMsg, callback function: (none) */
#define  PANEL_PICTURE_DISCHARGEON        74      /* control type: picture, callback function: (none) */
#define  PANEL_CH_MODE                    75      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_6                  76      /* control type: textMsg, callback function: (none) */
#define  PANEL_PICTURE_7                  77      /* control type: picture, callback function: (none) */
#define  PANEL_PICTURE_10                 78      /* control type: picture, callback function: (none) */
#define  PANEL_P_CH_RELAY_ON              79      /* control type: picture, callback function: (none) */
#define  PANEL_P_CH_RELAY_OFF             80      /* control type: picture, callback function: (none) */
#define  PANEL_P_DISCH_RELAY_ON           81      /* control type: picture, callback function: (none) */
#define  PANEL_PICTURE_9                  82      /* control type: picture, callback function: (none) */
#define  PANEL_P_DISCH_RELAY_OFF          83      /* control type: picture, callback function: (none) */

#define  PNL_CELLS                        2       /* callback function: PNL_CELLCBExit */
#define  PNL_CELLS_CELLSAVEBT             2       /* control type: command, callback function: cmdCELLSaveCB */
#define  PNL_CELLS_TEXTMSG_2              3       /* control type: textMsg, callback function: (none) */
#define  PNL_CELLS_TEXTMSG                4       /* control type: textMsg, callback function: (none) */
#define  PNL_CELLS_CELLCANCELBT           5       /* control type: command, callback function: cmdCELLCancelCB */
#define  PNL_CELLS_CELLRESETBT            6       /* control type: command, callback function: cmdCELLResetCB */
#define  PNL_CELLS_DOWN_LIMIT             7       /* control type: numeric, callback function: (none) */
#define  PNL_CELLS_UP_LIMIT               8       /* control type: numeric, callback function: (none) */

#define  PNL_DECI                         3       /* callback function: PNL_DECIExit */
#define  PNL_DECI_DECIMALSAVEBT           2       /* control type: command, callback function: cmdDecimalSaveCB */
#define  PNL_DECI_NUMERIC                 3       /* control type: numeric, callback function: (none) */
#define  PNL_DECI_DECIMALCANCELBT         4       /* control type: command, callback function: cmdDecimalCancelCB */
#define  PNL_DECI_DECIMALRESETBT          5       /* control type: command, callback function: cmdDecimalResetCB */
#define  PNL_DECI_TEXTMSG                 6       /* control type: textMsg, callback function: (none) */

#define  PNL_ERRMSG                       4
#define  PNL_ERRMSG_COMMANDBUTTON         2       /* control type: command, callback function: cmdErrMessageHide */
#define  PNL_ERRMSG_TEXTMSG               3       /* control type: textMsg, callback function: (none) */

#define  PNL_M1                           5       /* callback function: PNL_M1CBExit */
#define  PNL_M1_TARGET_CURRENT            2       /* control type: numeric, callback function: (none) */
#define  PNL_M1_TARGET_VOLTAGE            3       /* control type: numeric, callback function: (none) */
#define  PNL_M1_M1RESETBT                 4       /* control type: command, callback function: cmdM1ResetCB */
#define  PNL_M1_M1CANCELBT                5       /* control type: command, callback function: cmdM1CancelCB */
#define  PNL_M1_M1SAVEBT                  6       /* control type: command, callback function: cmdM1SaveCB */
#define  PNL_M1_TEXTMSG_2                 7       /* control type: textMsg, callback function: (none) */
#define  PNL_M1_RING                      8       /* control type: ring, callback function: cmdM1CurrentSelecCB */
#define  PNL_M1_TEXTMSG                   9       /* control type: textMsg, callback function: (none) */

#define  PNL_M2                           6       /* callback function: PNL_M2CBExit */
#define  PNL_M2_TARGET_CURRENT            2       /* control type: numeric, callback function: (none) */
#define  PNL_M2_TEXTMSG                   3       /* control type: textMsg, callback function: (none) */
#define  PNL_M2_M2RESETBT                 4       /* control type: command, callback function: cmdM2ResetCB */
#define  PNL_M2_TARGET_VOLTAGE            5       /* control type: numeric, callback function: (none) */
#define  PNL_M2_M2CANCELBT                6       /* control type: command, callback function: cmdM2CancelCB */
#define  PNL_M2_M2SAVEBT                  7       /* control type: command, callback function: cmdM2SaveCB */
#define  PNL_M2_TEXTMSG_2                 8       /* control type: textMsg, callback function: (none) */
#define  PNL_M2_RING                      9       /* control type: ring, callback function: cmdM2CurrentSelecCB */

#define  PNL_M3                           7       /* callback function: PNL_M3CBExit */
#define  PNL_M3_SETTLING_TIME             2       /* control type: numeric, callback function: (none) */
#define  PNL_M3_END_CURRENT               3       /* control type: numeric, callback function: (none) */
#define  PNL_M3_M3RESETBT                 4       /* control type: command, callback function: cmdM3ResetCB */
#define  PNL_M3_TAPERING                  5       /* control type: numeric, callback function: (none) */
#define  PNL_M3_TARGET_CURRENT            6       /* control type: numeric, callback function: (none) */
#define  PNL_M3_M3CANCELBT                7       /* control type: command, callback function: cmdM3CancelCB */
#define  PNL_M3_M3SAVEBT                  8       /* control type: command, callback function: cmdM3SaveCB */
#define  PNL_M3_TEXTMSG                   9       /* control type: textMsg, callback function: (none) */
#define  PNL_M3_TARGET_VOLTAGE            10      /* control type: numeric, callback function: (none) */
#define  PNL_M3_TEXTMSG_5                 11      /* control type: textMsg, callback function: (none) */
#define  PNL_M3_TEXTMSG_4                 12      /* control type: textMsg, callback function: (none) */
#define  PNL_M3_TEXTMSG_3                 13      /* control type: textMsg, callback function: (none) */
#define  PNL_M3_TEXTMSG_2                 14      /* control type: textMsg, callback function: (none) */

#define  PNL_M4                           8       /* callback function: PNL_M4CBExit */
#define  PNL_M4_REPEAT_CNT                2       /* control type: numeric, callback function: (none) */
#define  PNL_M4_SETTLING_TIME             3       /* control type: numeric, callback function: (none) */
#define  PNL_M4_M4RESETBT                 4       /* control type: command, callback function: cmdM4ResetCB */
#define  PNL_M4_TEXTMSG_6                 5       /* control type: textMsg, callback function: (none) */
#define  PNL_M4_TEXTMSG                   6       /* control type: textMsg, callback function: (none) */
#define  PNL_M4_M4CANCELBT                7       /* control type: command, callback function: cmdM4CancelCB */
#define  PNL_M4_M4SAVEBT                  8       /* control type: command, callback function: cmdM4SaveCB */
#define  PNL_M4_END_CURRENT               9       /* control type: numeric, callback function: (none) */
#define  PNL_M4_D_TARGET_VOLTAGE          10      /* control type: numeric, callback function: (none) */
#define  PNL_M4_TARGET_VOLTAGE            11      /* control type: numeric, callback function: (none) */
#define  PNL_M4_TEXTMSG_7                 12      /* control type: textMsg, callback function: (none) */
#define  PNL_M4_TEXTMSG_2                 13      /* control type: textMsg, callback function: (none) */
#define  PNL_M4_TAPERING                  14      /* control type: numeric, callback function: (none) */
#define  PNL_M4_TEXTMSG_3                 15      /* control type: textMsg, callback function: (none) */
#define  PNL_M4_D_TARGET_CURRENT          16      /* control type: numeric, callback function: (none) */
#define  PNL_M4_TARGET_CURRENT            17      /* control type: numeric, callback function: (none) */
#define  PNL_M4_TEXTMSG_4                 18      /* control type: textMsg, callback function: (none) */
#define  PNL_M4_TEXTMSG_8                 19      /* control type: textMsg, callback function: (none) */
#define  PNL_M4_TEXTMSG_5                 20      /* control type: textMsg, callback function: (none) */

#define  PNL_M5                           9       /* callback function: PNL_M5CBExit */
#define  PNL_M5_REPEAT_CNT                2       /* control type: numeric, callback function: (none) */
#define  PNL_M5_TEXTMSG                   3       /* control type: textMsg, callback function: (none) */
#define  PNL_M5_M5RESETBT                 4       /* control type: command, callback function: cmdM5ResetCB */
#define  PNL_M5_TEXTMSG_2                 5       /* control type: textMsg, callback function: (none) */
#define  PNL_M5_TEXTMSG_7                 6       /* control type: textMsg, callback function: (none) */
#define  PNL_M5_TARGET_VOLTAGE            7       /* control type: numeric, callback function: (none) */
#define  PNL_M5_M5CANCELBT                8       /* control type: command, callback function: cmdM5CancelCB */
#define  PNL_M5_M5SAVEBT                  9       /* control type: command, callback function: cmdM5SaveCB */
#define  PNL_M5_TARGET_CURRENT            10      /* control type: numeric, callback function: (none) */
#define  PNL_M5_D_TARGET_CURRENT          11      /* control type: numeric, callback function: (none) */
#define  PNL_M5_TEXTMSG_8                 12      /* control type: textMsg, callback function: (none) */

#define  PNL_M6                           10      /* callback function: PNL_M6CBExit */
#define  PNL_M6_TEXTMSG                   2       /* control type: textMsg, callback function: (none) */
#define  PNL_M6_NUMERIC                   3       /* control type: numeric, callback function: (none) */
#define  PNL_M6_M5RESETBT                 4       /* control type: command, callback function: cmdM6ResetCB */
#define  PNL_M6_M5CANCELBT                5       /* control type: command, callback function: cmdM6CancelCB */
#define  PNL_M6_M5SAVEBT                  6       /* control type: command, callback function: cmdM6SaveCB */

#define  PNL_M7                           11      /* callback function: PNL_M7CBExit */
#define  PNL_M7_TEXTMSG                   2       /* control type: textMsg, callback function: (none) */
#define  PNL_M7_RING                      3       /* control type: ring, callback function: (none) */
#define  PNL_M7_M7CANCELBT                4       /* control type: command, callback function: cmdM7CancelCB */
#define  PNL_M7_M7SAVEBT                  5       /* control type: command, callback function: cmdM7SaveCB */

#define  PNL_M8                           12      /* callback function: PNL_M8CBExit */
#define  PNL_M8_M8EXITBT                  2       /* control type: command, callback function: cmdM8ExitCB */
#define  PNL_M8_M8SAVEBT                  3       /* control type: command, callback function: cmdM8SaveCB */
#define  PNL_M8_STRING                    4       /* control type: string, callback function: (none) */

#define  PNL_TEMP                         13      /* callback function: PNL_TEMPCBExit */
#define  PNL_TEMP_TEMPSAVEBT              2       /* control type: command, callback function: cmdTEMPSaveCB */
#define  PNL_TEMP_TEXTMSG_2               3       /* control type: textMsg, callback function: (none) */
#define  PNL_TEMP_TEXTMSG                 4       /* control type: textMsg, callback function: (none) */
#define  PNL_TEMP_TEMPANCELBT             5       /* control type: command, callback function: cmdTEMPCancelCB */
#define  PNL_TEMP_TEMPRESETBT             6       /* control type: command, callback function: cmdTEMPResetCB */
#define  PNL_TEMP_NUMERIC_2               7       /* control type: numeric, callback function: (none) */
#define  PNL_TEMP_NUMERIC                 8       /* control type: numeric, callback function: (none) */

     /* tab page panel controls */
#define  TABPANEL_TEXTBOX                 2       /* control type: textBox, callback function: (none) */

     /* tab page panel controls */
#define  TABPANEL_2_NUMERICSLIDE          2       /* control type: scale, callback function: (none) */
#define  TABPANEL_2_STRIPCHART            3       /* control type: strip, callback function: (none) */
#define  TABPANEL_2_SCALE_X               4       /* control type: textButton, callback function: scale_x_setup */
#define  TABPANEL_2_SCALE_Y               5       /* control type: textButton, callback function: scale_y_setup */

     /* tab page panel controls */
#define  TABPANEL_3_GRAPH                 2       /* control type: graph, callback function: (none) */
#define  TABPANEL_3_BINARYSWITCH          3       /* control type: binary, callback function: (none) */
#define  TABPANEL_3_NUMERIC_13            4       /* control type: numeric, callback function: (none) */
#define  TABPANEL_3_NUMERIC_12            5       /* control type: numeric, callback function: (none) */
#define  TABPANEL_3_NUMERIC_11            6       /* control type: numeric, callback function: (none) */
#define  TABPANEL_3_NUMERIC_10            7       /* control type: numeric, callback function: (none) */
#define  TABPANEL_3_NUMERIC_9             8       /* control type: numeric, callback function: (none) */
#define  TABPANEL_3_NUMERIC_8             9       /* control type: numeric, callback function: (none) */
#define  TABPANEL_3_NUMERIC_7             10      /* control type: numeric, callback function: (none) */
#define  TABPANEL_3_NUMERIC_6             11      /* control type: numeric, callback function: (none) */
#define  TABPANEL_3_NUMERIC_5             12      /* control type: numeric, callback function: (none) */
#define  TABPANEL_3_NUMERIC_4             13      /* control type: numeric, callback function: (none) */
#define  TABPANEL_3_NUMERIC_3             14      /* control type: numeric, callback function: (none) */
#define  TABPANEL_3_NUMERIC_2             15      /* control type: numeric, callback function: (none) */
#define  TABPANEL_3_NUMERIC               16      /* control type: numeric, callback function: (none) */
#define  TABPANEL_3_CELLVOLTAGEMIN        17      /* control type: scale, callback function: (none) */
#define  TABPANEL_3_CELLVOLTAGEMAX        18      /* control type: scale, callback function: (none) */
#define  TABPANEL_3_PICTURE               19      /* control type: picture, callback function: (none) */
#define  TABPANEL_3_PICTURE_2             20      /* control type: picture, callback function: (none) */
#define  TABPANEL_3_TIMER                 21      /* control type: timer, callback function: timertest */


     /* Control Arrays: */

#define  CTRLARRAY                        1
#define  CTRLARRAY_2                      2
#define  CTRLARRAY_3                      3

     /* Menu Bars, Menus, and Menu Items: */

#define  MENUBAR                          1
#define  MENUBAR_MENU1                    2
#define  MENUBAR_MENU1_ITEM1              3       /* callback function: cmdModelListCB */
#define  MENUBAR_MENU1_ITEM2              4       /* callback function: cmdOperatorCB */
#define  MENUBAR_MENU1_SEPARATOR          5
#define  MENUBAR_MENU1_ITEM1_4            6       /* callback function: cmdEventHistoryClearCB */
#define  MENUBAR_MENU1_ITEM1_7            7       /* callback function: cmdResetBTBCB */
#define  MENUBAR_MENU1_SEPARATOR_2        8
#define  MENUBAR_MENU1_SEPARATOR_3        9
#define  MENUBAR_MENU1_ITEM4              10      /* callback function: cmdMainExit */
#define  MENUBAR_MENU2                    11
#define  MENUBAR_MENU2_ITEM1_8            12      /* callback function: cmdOpenExeclCB */
#define  MENUBAR_MENU2_ITEM1_3            13      /* callback function: cmdFileChartCB */
#define  MENUBAR_MENU3                    14
#define  MENUBAR_MENU3_ITEM6              15      /* callback function: cmdSetValueChargeCBM */
#define  MENUBAR_MENU3_ITEM7              16      /* callback function: cmdSetValueDischargeCBM */
#define  MENUBAR_MENU3_ITEM8              17      /* callback function: cmdSetValueFullChargeCBM */
#define  MENUBAR_MENU3_ITEM9              18      /* callback function: cmdSetValueCellBalanceCBM */
#define  MENUBAR_MENU3_ITEM10             19      /* callback function: cmdSetValueEndOfStorageCBM */
#define  MENUBAR_MENU4                    20
#define  MENUBAR_MENU4_ITEM1_2            21      /* callback function: cmdFileIntervalCB */
#define  MENUBAR_MENU4_ITEM2_2            22      /* callback function: cmdTempLimitCB */
#define  MENUBAR_MENU4_ITEM1_5            23      /* callback function: cmdCellVoltageLimitCB */
#define  MENUBAR_MENU4_ITEM1_6            24      /* callback function: cmdDecimalPointCB */
#define  MENUBAR_MENU5                    25
#define  MENUBAR_MENU5_ITEM12             26      /* callback function: cmdHelp */


     /* Callback Prototypes: */

int  CVICALLBACK cmdCELLCancelCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cmdCELLResetCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cmdCELLSaveCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK cmdCellVoltageLimitCB(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK cmdDecimalCancelCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK cmdDecimalPointCB(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK cmdDecimalResetCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cmdDecimalSaveCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cmdErrMessageHide(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK cmdEventHistoryClearCB(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK cmdExecutionCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK cmdFileChartCB(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK cmdFileIntervalCB(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK cmdFileSaveCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK cmdHelp(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK cmdM1CancelCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cmdM1CurrentSelecCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cmdM1ResetCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cmdM1SaveCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cmdM2CancelCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cmdM2CurrentSelecCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cmdM2ResetCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cmdM2SaveCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cmdM3CancelCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cmdM3ResetCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cmdM3SaveCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cmdM4CancelCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cmdM4ResetCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cmdM4SaveCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cmdM5CancelCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cmdM5ResetCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cmdM5SaveCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cmdM6CancelCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cmdM6ResetCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cmdM6SaveCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cmdM7CancelCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cmdM7SaveCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cmdM8ExitCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cmdM8SaveCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK cmdMainExit(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK cmdMainPNLExit(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cmdModelIDCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK cmdModelListCB(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK cmdOpenExeclCB(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK cmdOperatorCB(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK cmdOptionCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK cmdResetBTBCB(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK cmdSetValueCellBalanceCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK cmdSetValueCellBalanceCBM(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK cmdSetValueChargeCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK cmdSetValueChargeCBM(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK cmdSetValueDischargeCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK cmdSetValueDischargeCBM(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK cmdSetValueEndOfStorageCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK cmdSetValueEndOfStorageCBM(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK cmdSetValueFullChargeCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK cmdSetValueFullChargeCBM(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK cmdTEMPCancelCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK cmdTempLimitCB(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK cmdTEMPResetCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cmdTEMPSaveCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PNL_CELLCBExit(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PNL_DECIExit(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PNL_M1CBExit(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PNL_M2CBExit(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PNL_M3CBExit(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PNL_M4CBExit(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PNL_M5CBExit(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PNL_M6CBExit(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PNL_M7CBExit(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PNL_M8CBExit(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PNL_TEMPCBExit(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK scale_x_setup(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK scale_y_setup(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK selectMethodRadio(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK timertest(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
