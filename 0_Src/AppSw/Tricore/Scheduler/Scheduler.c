/**
 * \file Scheduler.c
 * \brief 
 */

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

#include "Scheduler.h"
#include "AsclinAscDemo.h"
#include "VadcAutoScan.h"
#include "GtmTom_PwmHl.h"

/******************************************************************************/
/*-----------------------------------Macros-----------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*--------------------------------Enumerations--------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*-----------------------------Data Structures--------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*------------------------------Global variables------------------------------*/
/******************************************************************************/

App_Scheduler g_Scheduler;

/******************************************************************************/
/*-------------------------Function Prototypes--------------------------------*/
/******************************************************************************/

void Scheduler_init(void);
void Scheduler_run(void);
void Scheduler_appTaskFlag_1ms(void);
void Scheduler_appTaskFlag_10ms(void);
void Scheduler_appTaskFlag_100ms(void);
void Scheduler_appTaskFlag_1s(void);

/******************************************************************************/
/*------------------------Private Variables/Constants-------------------------*/
/******************************************************************************/

/******************************************************************************/
/*-------------------------Function Implementations---------------------------*/
/******************************************************************************/

void Scheduler_init(void){
    g_Scheduler.taskFlag_1ms = FALSE;
    g_Scheduler.taskFlag_10ms = FALSE;
    g_Scheduler.taskFlag_100ms = FALSE;
    g_Scheduler.taskFlag_1s = FALSE;

    g_Scheduler.counter = 0;
};

void Scheduler_run(void){
    if(g_Scheduler.taskFlag_1ms == TRUE){
        Scheduler_appTaskFlag_1ms();
        g_Scheduler.taskFlag_1ms = FALSE;
    }
    if(g_Scheduler.taskFlag_10ms == TRUE){
        Scheduler_appTaskFlag_10ms();
        g_Scheduler.taskFlag_10ms = FALSE;
    }
    if(g_Scheduler.taskFlag_100ms == TRUE){
        Scheduler_appTaskFlag_100ms();
        g_Scheduler.taskFlag_100ms = FALSE;
    }
    if(g_Scheduler.taskFlag_1s == TRUE){
        Scheduler_appTaskFlag_1s();
        g_Scheduler.taskFlag_1s = FALSE;
    }
}

void Scheduler_appTaskFlag_1ms(void){
    GtmTom_PwmHl_run();
    /* Send Vadc Auto Scan data using ASCLIN*/
    VadcAutoScan_run();
    // IfxStm_waitTicks(g_Stm.stmSfr, TimeConst_1s);
    /* Save Vadc scanned data into Asclin txData */        
    // g_AsclinAsc.txData[0] = (uint8) ((g_VadcAutoScan.adcValue[7] & 0xFF00) >> 8);
    // g_AsclinAsc.txData[1] = (uint8) (g_VadcAutoScan.adcValue[7] & 0x00FF);

    g_AsclinAsc.txData[0] = (uint8) (g_VadcAutoScan.adcValue[7] & 0x00FF);
    g_AsclinAsc.txData[1] = (uint8) (g_VadcAutoScan.adcValue[6] & 0x00FF);
    g_AsclinAsc.txData[2] = (uint8) (g_VadcAutoScan.adcValue[5] & 0x00FF);
    g_AsclinAsc.txData[3] = (uint8) (g_VadcAutoScan.adcValue[4] & 0x00FF);

    /* Asclin transmit(write) data*/
    IfxAsclin_Asc_write(&g_AsclinAsc.drivers.asc3, g_AsclinAsc.txData, &g_AsclinAsc.count, TIME_INFINITE);

};
void Scheduler_appTaskFlag_10ms(void){

};
void Scheduler_appTaskFlag_100ms(void){
    
};
void Scheduler_appTaskFlag_1s(void)
{
    // Led_toggle();
    // AsclinAscDemo_run();
    GtmTom_PwmHl_run();
}
