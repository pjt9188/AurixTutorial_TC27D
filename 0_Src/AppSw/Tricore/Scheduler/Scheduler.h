/**
 * \file Scheduler.h
 * \brief 
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H 1

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

#include "Cpu/Std/Ifx_Types.h"
#include "Led.h"

/******************************************************************************/
/*-----------------------------------Macros-----------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*--------------------------------Enumerations--------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*-----------------------------Data Structures--------------------------------*/
/******************************************************************************/

typedef struct{
    volatile boolean    taskFlag_1ms;   
    volatile boolean    taskFlag_10ms;
    volatile boolean    taskFlag_100ms;
    volatile boolean    taskFlag_1s;
    
    volatile uint32     counter;
} App_Scheduler;

/******************************************************************************/
/*------------------------------Global variables------------------------------*/
/******************************************************************************/

IFX_EXTERN App_Scheduler g_Scheduler;

/******************************************************************************/
/*-------------------------Global Function Prototypes-------------------------*/
/******************************************************************************/

IFX_EXTERN void Scheduler_init(void);
IFX_EXTERN void Scheduler_run(void);
IFX_EXTERN void Scheduler_appTaskFlag_1ms(void);
IFX_EXTERN void Scheduler_appTaskFlag_10ms(void);
IFX_EXTERN void Scheduler_appTaskFlag_100ms(void);
IFX_EXTERN void Scheduler_appTaskFlag_1s(void);

/******************************************************************************/
/*-------------------------Inline Function Prototypes-------------------------*/
/******************************************************************************/

/******************************************************************************/
/*---------------------Inline Function Implementations------------------------*/
/******************************************************************************/

#endif /* SCHEDULER_H */
