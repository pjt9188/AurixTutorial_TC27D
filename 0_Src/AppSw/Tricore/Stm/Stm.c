/**
 * \file Stm.c
 * \brief Stm
 *
 * \version iLLD_Demos_1_0_1_8_0
 * \copyright Copyright (c) 2014 Infineon Technologies AG. All rights reserved.
 *
 *
 *                                 IMPORTANT NOTICE
 *
 *
 * Infineon Technologies AG (Infineon) is supplying this file for use
 * exclusively with Infineon's microcontroller products. This file can be freely
 * distributed within development tools that are supporting such microcontroller
 * products.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * INFINEON SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 */

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

#include <stdio.h>

#include "Stm.h"

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

App_Stm g_Stm; /**< \brief Stm global data */
/******************************************************************************/
/*-------------------------Function Prototypes--------------------------------*/
/******************************************************************************/

void Stm_init(void);
void Stm_run(void);

/******************************************************************************/
/*------------------------Private Variables/Constants-------------------------*/
/******************************************************************************/

/******************************************************************************/
/*-------------------------Function Implementations---------------------------*/
/******************************************************************************/
/** \addtogroup IfxLld_Demo_Stm_SrcDoc_Main_Interrupt
 * \{ */

/** \name Interrupts for SystemTimer(STM) driver.
 * \{ */
IFX_INTERRUPT(STM_Stm0Sr0Handler, 0, ISR_PRIORITY_STM_STM0SR0);
/** \} */

/** \} */
/** \brief Handle SystemTimer(STM) interrupt
 *
 * \isrProvider \ref ISR_PROVIDER_SystemTimer(STM)
 * \isrPriority \ref ISR_PRIORITY_SystemTimer(STM)
 *
 */
void STM_Stm0Sr0Handler(void)
{
    IfxStm_clearCompareFlag(g_Stm.stmSfr, g_Stm.stmConfig.comparator);
#ifdef SIMULATION
	IfxStm_increaseCompare(g_Stm.stmSfr, g_Stm.stmConfig.comparator, 1000);
#else
	IfxStm_increaseCompare(g_Stm.stmSfr, g_Stm.stmConfig.comparator, g_Stm.stmConfig.ticks);
#endif
    IfxCpu_enableInterrupts();

    g_Scheduler.counter++;

    if(g_Scheduler.counter == 1000){
        g_Scheduler.counter = 0;
    }

    g_Scheduler.taskFlag_1ms = TRUE;
    
    if(g_Scheduler.counter % 10 == 0){
        g_Scheduler.taskFlag_10ms = TRUE;
    }

    if(g_Scheduler.counter % 100 == 0){
        g_Scheduler.taskFlag_100ms = TRUE;
    }

    if(g_Scheduler.counter % 1000 == 0){
        g_Scheduler.taskFlag_1s = TRUE;
        // Led_toggle();
    }
}


/** \brief Stm init API
 *
 * This function is called from main during initialization phase
 */
void Stm_init(void)
{
    printf("Stm_init() called\n");

    /* disable interrupts */
    boolean interruptState = IfxCpu_disableInterrupts();

    initTime();

    g_Stm.stmSfr = &MODULE_STM0;
    IfxStm_initCompareConfig(&g_Stm.stmConfig);

    g_Stm.stmConfig.triggerPriority = ISR_PRIORITY_STM_STM0SR0;
    g_Stm.stmConfig.typeOfService   = ISR_PROVIDER_STM_STM0SR0;
#ifdef SIMULATION
    g_SrcSwInt.stmConfig.ticks      = 1000;
#else
    g_Stm.stmConfig.ticks           = TimeConst_1ms;
#endif
    IfxStm_initCompare(g_Stm.stmSfr, &g_Stm.stmConfig);

    Scheduler_init();
    Led_init();

    /* enable interrupts again */
    IfxCpu_restoreInterrupts(interruptState);
}


/** \brief STM run API
 *
 * This function is called from main, background loop
 */
void Stm_run(void)
{}
