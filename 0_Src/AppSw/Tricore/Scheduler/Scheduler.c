/**
 * \file Scheduler.c
 * \brief 
 */

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

#include "Scheduler.h"

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

void Scheduler_appTaskFlag_1ms(void){

};
void Scheduler_appTaskFlag_10ms(void){

};
void Scheduler_appTaskFlag_100ms(void){
    
};
void Scheduler_appTaskFlag_1s(void)
{
    Led_toggle();
}
