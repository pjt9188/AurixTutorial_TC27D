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
static void IfxBlinkLed_Task(void);
// static void IfxBlinkLed_Init(void);
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
IFX_INTERRUPT(STM_Int0Handler, 0, ISR_PRIORITY_STM_INT0);
/** \} */

/** \} */
/** \brief Handle SystemTimer(STM) interrupt
 *
 * \isrProvider \ref ISR_PROVIDER_SystemTimer(STM)
 * \isrPriority \ref ISR_PRIORITY_SystemTimer(STM)
 *
 */
void STM_Int0Handler(void)
{
    IfxStm_clearCompareFlag(g_Stm.stmSfr, g_Stm.stmConfig.comparator);
#ifdef SIMULATION
	IfxStm_increaseCompare(g_Stm.stmSfr, g_Stm.stmConfig.comparator, 1000);
#else
	IfxStm_increaseCompare(g_Stm.stmSfr, g_Stm.stmConfig.comparator, TimeConst_1s);
#endif
    IfxCpu_enableInterrupts();
    IfxBlinkLed_Task();
}


/** \brief LED Blinking
 *
 * This function blinks the LED connected to P 10.2 and counts the number
 *	of times the interrupt occurs.
 */
static void IfxBlinkLed_Task(void)
{
    Led_toggle();
    g_Stm.counter++;
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

    g_Stm.counter  = 0;

    initTime();

    g_Stm.stmSfr = &MODULE_STM0;
    IfxStm_initCompareConfig(&g_Stm.stmConfig);

    g_Stm.stmConfig.triggerPriority = ISR_PRIORITY_STM_INT0;
    g_Stm.stmConfig.typeOfService   = IfxSrc_Tos_cpu0;
#ifdef SIMULATION
    g_SrcSwInt.stmConfig.ticks      = 1000;
#else
    g_Stm.stmConfig.ticks           = TimeConst_1s;
#endif
    IfxStm_initCompare(g_Stm.stmSfr, &g_Stm.stmConfig);

    Led_init();

    /* enable interrupts again */
    IfxCpu_restoreInterrupts(interruptState);
}


/** \brief Demo run API
 *
 * This function is called from main, background loop
 */
void Stm_run(void)
{
    printf("Stm_run() called\n");

    while (g_Stm.counter < 10)
    {}

    printf("OK: checks passed \n");

    while(TRUE)
    {}
}
