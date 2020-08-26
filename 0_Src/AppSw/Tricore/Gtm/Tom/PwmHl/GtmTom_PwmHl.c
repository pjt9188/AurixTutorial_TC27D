/**
 * \file GtmTom_PwmHl.c
 * \brief Demo GtmAtomPwmHlDemo
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

App_GtmTomPwmHl g_GtmTomPwmHl; /**< \brief Demo information */

/******************************************************************************/
/*-------------------------Function Prototypes--------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*------------------------Private Variables/Constants-------------------------*/
/******************************************************************************/

/******************************************************************************/
/*-------------------------Function Implementations---------------------------*/
/******************************************************************************/

/** \addtogroup IfxLld_Demo_GtmTomPwmHl_SrcDoc_Main_Interrupt
 * \{ */

/** \name Interrupts for Timer
 * \{ */

IFX_INTERRUPT(ISR_Timer, 0, ISR_PRIORITY_TIMER);

/** \} */

/** \} */

/** \brief Handle 1ms interrupt.
 *
 * \isrProvider \ref ISR_PROVIDER_TIMER
 * \isrPriority \ref ISR_PRIORITY_TIMER
 *
 */
void ISR_Timer(void)
{
    IfxCpu_enableInterrupts();

    IfxGtm_Tom_Timer_acknowledgeTimerIrq(&g_GtmTomPwmHl.drivers.timer);
    g_GtmTomPwmHl.isrCounter.slotTimer++;
}


void GtmTom_PwmHl_initTimer(void)
{
    /* GTM TOM configuration */
    // Create Timer Output Module(TOM) config for timer
    IfxGtm_Tom_Timer_Config timerConfig;
    // Create Timer Output Module(TOM) config for H-bridge
    IfxGtm_Tom_PwmHl_Config pwmHlConfig;

    IfxGtm_Tom_Timer_initConfig(&timerConfig, &MODULE_GTM);
    timerConfig.base.frequency                  = 10000;                                    // frequency = 10Khz, Period = 100us = 10000tics * 10ns/1tic (max tics = 65535 = 2^16 -1)
    timerConfig.base.isrPriority                = ISR_PRIORITY_TIMER;
    // timerConfig.base.isrPriority                = ISR_PRIORITY(INTERRUPT_TIMER);
    timerConfig.base.isrProvider                = ISR_PROVIDER_TIMER;
    // timerConfig.base.isrProvider                = ISR_PROVIDER(INTERRUPT_TIMER);
    timerConfig.base.minResolution              = (1.0 / timerConfig.base.frequency) / 1000; /*
                                                                                              * minResolution >= 1 / frequency 인지 init함수에서 검사
                                                                                              * 특별히 설정하는 것은 없음. init함수에서 확인만 해줌
                                                                                              */

    timerConfig.base.trigger.enabled            = FALSE;                                     /* 여기서 trigger는 internal trigger가 아니라
                                                                                              * Timer 채널(Ch0)이 만들어내는 PWM인데 
                                                                                              * 왜 굳이 FALSE시켰다가 TRUE하는지 모르겠음..
                                                                                              * clock = 100Mhz
                                                                                              */
    timerConfig.clock                           = IfxGtm_Tom_Ch_ClkSrc_cmuFxclk0;           // TOM의 Clock Source 선택   

    timerConfig.tom                             = IfxGtm_Tom_0;
    timerConfig.timerChannel                    = IfxGtm_Tom_Ch_0;
    timerConfig.triggerOut                      = &IfxGtm_TOM0_0_TOUT77_P15_6_OUT;          // Set p15.6 as TOM output

    timerConfig.base.trigger.outputEnabled      = TRUE;
    timerConfig.base.trigger.enabled            = TRUE;
    timerConfig.base.trigger.triggerPoint       = 500;                                      // Set trigger point(PWM Output of CH0), (CM1) = 500tics = 500 * 10ns
    timerConfig.base.trigger.risingEdgeAtPeriod = TRUE;

    IfxGtm_Tom_Timer_init(&g_GtmTomPwmHl.drivers.timer, &timerConfig);

    IfxGtm_Tom_PwmHl_initConfig(&pwmHlConfig);

    /* H-bridge 4개의 소자에 들어갈 output pin 할당 */
    IfxGtm_Tom_ToutMapP ccx[2], coutx[2];
    ccx[0]   = &IfxGtm_TOM0_3_TOUT105_P10_3_OUT;
    coutx[0] = &IfxGtm_TOM0_4_TOUT22_P33_0_OUT;
    ccx[1]   = &IfxGtm_TOM0_2_TOUT107_P10_5_OUT;
    coutx[1] = &IfxGtm_TOM0_5_TOUT23_P33_1_OUT;

    pwmHlConfig.timer                 = &g_GtmTomPwmHl.drivers.timer;
    pwmHlConfig.tom                   = timerConfig.tom;
    pwmHlConfig.base.deadtime         = 2e-6;
    pwmHlConfig.base.minPulse         = 1e-6;
    pwmHlConfig.base.channelCount     = 2;
    pwmHlConfig.base.emergencyEnabled = FALSE;
    pwmHlConfig.base.outputMode       = IfxPort_OutputMode_pushPull;
    pwmHlConfig.base.outputDriver     = IfxPort_PadDriver_cmosAutomotiveSpeed1;
    pwmHlConfig.base.ccxActiveState   = Ifx_ActiveState_high;
    pwmHlConfig.base.coutxActiveState = Ifx_ActiveState_high;
    pwmHlConfig.ccx                   = ccx;
    pwmHlConfig.coutx                 = coutx;

    IfxGtm_Tom_PwmHl_init(&g_GtmTomPwmHl.drivers.pwm, &pwmHlConfig);

    IfxGtm_Tom_Timer_run(&g_GtmTomPwmHl.drivers.timer);
}


/** \brief Demo init API
 *
 * This function is called from main during initialization phase
 */
void GtmTom_PwmHl_init(void)
{
    /* disable interrupts */
    boolean  interruptState = IfxCpu_disableInterrupts();

    /** - GTM clocks */
    Ifx_GTM *gtm = &MODULE_GTM;
    g_GtmTomPwmHl.info.gtmFreq = IfxGtm_Cmu_getModuleFrequency(gtm);
    IfxGtm_enable(gtm);

    /* Set the global clock frequencies */
    IfxGtm_Cmu_setGclkFrequency(gtm, g_GtmTomPwmHl.info.gtmFreq);
    g_GtmTomPwmHl.info.gtmGclkFreq = IfxGtm_Cmu_getGclkFrequency(gtm);

    /* Set the CMU CLK0(CMU_CLK0 = 100Mhz, Period = 10ns) */
    IfxGtm_Cmu_setClkFrequency(gtm, IfxGtm_Cmu_Clk_0, g_GtmTomPwmHl.info.gtmGclkFreq);
    g_GtmTomPwmHl.info.gtmCmuClk0Freq = IfxGtm_Cmu_getClkFrequency(gtm, IfxGtm_Cmu_Clk_0, TRUE);
    g_GtmTomPwmHl.info.state          = GtmTomPwmHl_State_init;
    g_GtmTomPwmHl.info.stateDeadline  = now();
    
    /* Select Clock source of the FXCLK -> CMU_CLK0 */
    GtmCmu_selectFxclkSource(&MODULE_GTM, GtmCmu_Clk0);

    /** - Initialise the GTM part */
    GtmTom_PwmHl_initTimer();
    printf("Gtm Tom PwmHl is initialized\n");

    g_GtmTomPwmHl.info.timerValue =  IfxGtm_Tom_Timer_getPeriod(&g_GtmTomPwmHl.drivers.timer);
    g_GtmTomPwmHl.tOn[0] = 0;
    g_GtmTomPwmHl.tOn[1] = 0;

    /* enable interrupts again */
    IfxCpu_restoreInterrupts(interruptState);

    IfxGtm_Cmu_enableClocks(gtm, IFXGTM_CMU_CLKEN_FXCLK | IFXGTM_CMU_CLKEN_CLK0);
}


/** \brief Demo run API
 *
 * This function is called from main, background loop
 */
void GtmTom_PwmHl_run(void)
{
    /* Set PWM duty */
    IfxGtm_Tom_PwmHl *pwmHl = &g_GtmTomPwmHl.drivers.pwm;
    IfxGtm_Tom_Timer *timer = &g_GtmTomPwmHl.drivers.timer;
    Ifx_TimerValue   *tOn   = g_GtmTomPwmHl.tOn;

    if (isDeadLine(g_GtmTomPwmHl.info.stateDeadline))
    {
        g_GtmTomPwmHl.info.stateDeadline = addTTime(g_GtmTomPwmHl.info.stateDeadline, TimeConst_1s);
        g_GtmTomPwmHl.info.state++;

        if (g_GtmTomPwmHl.info.state >= GtmTomPwmHl_State_count)
        {
            g_GtmTomPwmHl.info.state = GtmTomPwmHl_State_off;
        }

        switch (g_GtmTomPwmHl.info.state)
        {
        case GtmTomPwmHl_State_off:
            /* Set PWM to off */
            IfxGtm_Tom_PwmHl_setMode(pwmHl, Ifx_Pwm_Mode_off);
            IfxGtm_Tom_Timer_disableUpdate(timer);
            IfxGtm_Tom_PwmHl_setOnTime(pwmHl, NULL_PTR);
            IfxGtm_Tom_Timer_applyUpdate(timer);
            break;
        case GtmTomPwmHl_State_duty0:
            /* Set 0% duty cycle, center aligned */
            IfxGtm_Tom_PwmHl_setMode(pwmHl, Ifx_Pwm_Mode_centerAligned);
            tOn[0] = 0;
            tOn[1] = 0;
            IfxGtm_Tom_Timer_disableUpdate(timer);
            IfxGtm_Tom_PwmHl_setOnTime(pwmHl, tOn);
            IfxGtm_Tom_Timer_applyUpdate(timer);
            break;
        case GtmTomPwmHl_State_duty50:
            /* Set 50% duty cycle, center aligned */
            tOn[0] = IfxGtm_Tom_Timer_getPeriod(timer) / 2;
            tOn[1] = IfxGtm_Tom_Timer_getPeriod(timer) / 2;
            IfxGtm_Tom_Timer_disableUpdate(timer);
            IfxGtm_Tom_PwmHl_setOnTime(pwmHl, tOn);
            IfxGtm_Tom_Timer_applyUpdate(timer);
            break;
        case GtmTomPwmHl_State_duty100:
            /* Set 100% duty cycle, center aligned */
            tOn[0] = IfxGtm_Tom_Timer_getPeriod(timer);
            tOn[1] = IfxGtm_Tom_Timer_getPeriod(timer);
            IfxGtm_Tom_Timer_disableUpdate(timer);
            IfxGtm_Tom_PwmHl_setOnTime(pwmHl, tOn);
            IfxGtm_Tom_Timer_applyUpdate(timer);
            break;
        default:
            /* Set PWM to off */
            IfxGtm_Tom_PwmHl_setMode(pwmHl, Ifx_Pwm_Mode_off);
            IfxGtm_Tom_Timer_disableUpdate(timer);
            IfxGtm_Tom_PwmHl_setOnTime(pwmHl, NULL_PTR);
            IfxGtm_Tom_Timer_applyUpdate(timer);
            break;
        }
    }
}

