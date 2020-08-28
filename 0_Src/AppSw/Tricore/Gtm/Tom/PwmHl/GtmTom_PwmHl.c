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

    // Set TOM config for timer
    IfxGtm_Tom_Timer_initConfig(&timerConfig, &MODULE_GTM);
    /* 
    * Set Frequency of PwmHl
    * CMP0 = Gclk_Frequency / frequency 의 방식으로 Period tic을 구하므로,
    * CMP0의 한계인 65535를 넘지 않도록 Gclk의 주파수를 조정해 주어야한다. 
    */
    timerConfig.base.frequency                  = 1;                                         

    timerConfig.base.isrPriority                = ISR_PRIORITY_TIMER;
    // timerConfig.base.isrPriority                = ISR_PRIORITY(INTERRUPT_TIMER);
    timerConfig.base.isrProvider                = ISR_PROVIDER_TIMER;
    // timerConfig.base.isrProvider                = ISR_PROVIDER(INTERRUPT_TIMER);

    /* 
    * Set Frequency of PwmHl
    * minResolution >= 1 / frequency 인지 init함수에서 검사
    * 특별히 설정하는 것은 없음. init함수에서 확인만 해줌
    */
    timerConfig.base.minResolution              = (1.0 / timerConfig.base.frequency) / 1000; 
    
    /*
    * 여기서 trigger는 internal trigger가 아니라 Timer 채널(Ch0)이 만들어내는 PWM인데 
    * 왜 굳이 FALSE시켰다가 TRUE하는지 모르겠음..
    */
    timerConfig.base.trigger.enabled            = FALSE;          

    /* Select channel clock source as Fxclk0 */                           
    timerConfig.clock                           = IfxGtm_Tom_Ch_ClkSrc_cmuFxclk0;
    timerConfig.tom                             = IfxGtm_Tom_0;
    timerConfig.timerChannel                    = IfxGtm_Tom_Ch_0;
    /* Set p15.6 as Timer trigger output */
    timerConfig.triggerOut                      = &IfxGtm_TOM0_0_TOUT77_P15_6_OUT;
    /* Activate timer trigger */
    timerConfig.base.trigger.outputEnabled      = TRUE;
    timerConfig.base.trigger.enabled            = TRUE;
    /*
    * Set trigger point(PWM Output of CH0)
    * TOM0_CH0_CM1 = 5000tics
    */
    timerConfig.base.trigger.triggerPoint       = 5000;                                      
    timerConfig.base.trigger.risingEdgeAtPeriod = TRUE;

    IfxGtm_Tom_Timer_init(&g_GtmTomPwmHl.drivers.timer, &timerConfig);

    IfxGtm_Tom_PwmHl_initConfig(&pwmHlConfig);

    /* H-bridge 4개의 소자에 들어갈 output pin 할당 */
    IfxGtm_Tom_ToutMapP ccx[2], coutx[2];
    ccx[0]   = &IfxGtm_TOM0_3_TOUT105_P10_3_OUT;
    coutx[0] = &IfxGtm_TOM0_4_TOUT22_P33_0_OUT;
    ccx[1]   = &IfxGtm_TOM0_2_TOUT107_P10_5_OUT;
    coutx[1] = &IfxGtm_TOM0_5_TOUT23_P33_1_OUT;
    
    IfxGtm_PinMap_setTomTout(ccx[0], IfxPort_OutputMode_pushPull, IfxPort_PadDriver_cmosAutomotiveSpeed1);
    IfxGtm_PinMap_setTomTout(coutx[0], IfxPort_OutputMode_pushPull, IfxPort_PadDriver_cmosAutomotiveSpeed1);
    IfxGtm_PinMap_setTomTout(ccx[1], IfxPort_OutputMode_pushPull, IfxPort_PadDriver_cmosAutomotiveSpeed1);
    IfxGtm_PinMap_setTomTout(coutx[1], IfxPort_OutputMode_pushPull, IfxPort_PadDriver_cmosAutomotiveSpeed1);

    pwmHlConfig.timer                 = &g_GtmTomPwmHl.drivers.timer;
    pwmHlConfig.tom                   = timerConfig.tom;
    pwmHlConfig.base.deadtime         = 10e-4;
    pwmHlConfig.base.minPulse         = 1e-4;
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
    // IfxGtm_Cmu_setGclkFrequency(gtm, g_GtmTomPwmHl.info.gtmFreq / 10000);        // 큰 값으로 나눌 때, Gclk Den와 Num 계산에 시간이 너무 오래걸려 직접 넣음
    (&MODULE_GTM)->CMU.GCLK_NUM.B.GCLK_NUM = 10000;
    (&MODULE_GTM)->CMU.GCLK_DEN.B.GCLK_DEN = 1;
    g_GtmTomPwmHl.info.gtmGclkFreq = IfxGtm_Cmu_getGclkFrequency(gtm);

    /* Set the CMU CLK0 */
    IfxGtm_Cmu_setClkFrequency(gtm, IfxGtm_Cmu_Clk_0, g_GtmTomPwmHl.info.gtmGclkFreq);
    g_GtmTomPwmHl.info.gtmCmuClk0Freq = IfxGtm_Cmu_getClkFrequency(gtm, IfxGtm_Cmu_Clk_0, TRUE);
    g_GtmTomPwmHl.info.state          = GtmTomPwmHl_State_init;
    /* deadline이후 Pwm 수정이 가능함*/
    g_GtmTomPwmHl.info.stateDeadline  = now();
    
    /*
    * Select Clock source of the FXCLK -> CMU_CLK0
    * 원래 FxCLK의 clock source로 CMU_CLK0로 하고 쓰려 했으나,
    * PwmHl의 duty나 Period의 tick을 계산하는 ILLD 함수들이 FxCLK의 clock source를 찾지 않고
    * Gclk의 주파수를 가져와 사용하기 때문에, Gclk의 주파수를 수정하여 Fxclk의 clock source로 사용하도록 한다.
    */
    // GtmCmu_selectFxclkSource(&MODULE_GTM, GtmCmu_Clk0);

    /** - Initialise the GTM part */
    GtmTom_PwmHl_initTimer();
    printf("Gtm Tom PwmHl is initialized\n");

    g_GtmTomPwmHl.info.timerValue =  IfxGtm_Tom_Timer_getPeriod(&g_GtmTomPwmHl.drivers.timer);
    g_GtmTomPwmHl.tOn[0] = 0;
    g_GtmTomPwmHl.tOn[1] = 0;

    /* enable interrupts again */
    IfxCpu_restoreInterrupts(interruptState);

    IfxGtm_Cmu_enableClocks(gtm, IFXGTM_CMU_CLKEN_FXCLK | IFXGTM_CMU_CLKEN_CLK0);

    GtmTom_PwmHl_setState(g_GtmTomPwmHl.info.state);
}


/** \brief Demo run API
 *
 * This function is called from main, background loop
 */
void GtmTom_PwmHl_run(void)
{
    IfxGtm_Tom_PwmHl *pwmHl = &g_GtmTomPwmHl.drivers.pwm;
    IfxGtm_Tom_Timer *timer = &g_GtmTomPwmHl.drivers.timer;
    Ifx_TimerValue   *tOn   = g_GtmTomPwmHl.tOn;

    /* Deadline이 넘어간 이후부터 state 수정이 가능하도록 짜여져있음*/
    if (isDeadLine(g_GtmTomPwmHl.info.stateDeadline))
    {
        g_GtmTomPwmHl.info.stateDeadline = addTTime(g_GtmTomPwmHl.info.stateDeadline, TimeConst_1s * 5);
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
        case GtmTomPwmHl_State_init:
        /* Set PWM to off */
            IfxGtm_Tom_PwmHl_setMode(pwmHl, Ifx_Pwm_Mode_off);
            IfxGtm_Tom_Timer_disableUpdate(timer);
            IfxGtm_Tom_PwmHl_setOnTime(pwmHl, NULL_PTR);
            IfxGtm_Tom_Timer_applyUpdate(timer);
            break;
        case GtmTomPwmHl_State_duty0:
            /* Set 0% duty cycle, center aligned */
            tOn[0] = 0;
            tOn[1] = 0;
            IfxGtm_Tom_PwmHl_setMode(pwmHl, Ifx_Pwm_Mode_centerAligned);
            IfxGtm_Tom_Timer_disableUpdate(timer);
            IfxGtm_Tom_PwmHl_setOnTime(pwmHl, tOn);
            IfxGtm_Tom_Timer_applyUpdate(timer);
            break;
        case GtmTomPwmHl_State_duty50:
            /* Set 50% duty cycle, center aligned */
            tOn[0] = IfxGtm_Tom_Timer_getPeriod(timer) / 2;
            tOn[1] = IfxGtm_Tom_Timer_getPeriod(timer) / 2;
            IfxGtm_Tom_PwmHl_setMode(pwmHl, Ifx_Pwm_Mode_centerAligned);
            IfxGtm_Tom_Timer_disableUpdate(timer);
            IfxGtm_Tom_PwmHl_setOnTime(pwmHl, tOn);
            IfxGtm_Tom_Timer_applyUpdate(timer);
            break;
        case GtmTomPwmHl_State_duty100:
            /* Set 100% duty cycle, center aligned */
            IfxGtm_Tom_PwmHl_setMode(pwmHl, Ifx_Pwm_Mode_centerAligned);
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

void GtmTom_PwmHl_setState(GtmTomPwmHl_State state)
{
    IfxGtm_Tom_PwmHl *pwmHl = &g_GtmTomPwmHl.drivers.pwm;
    IfxGtm_Tom_Timer *timer = &g_GtmTomPwmHl.drivers.timer;
    Ifx_TimerValue   *tOn   = g_GtmTomPwmHl.tOn;

    g_GtmTomPwmHl.info.state = state;

    /* Deadline이 넘어간 이후부터 state 수정이 가능하도록 짜여져있음*/
    if (isDeadLine(g_GtmTomPwmHl.info.stateDeadline))
    {
        g_GtmTomPwmHl.info.stateDeadline = addTTime(g_GtmTomPwmHl.info.stateDeadline, TimeConst_1s);
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
        case GtmTomPwmHl_State_init:
        /* Set PWM to off */
            IfxGtm_Tom_PwmHl_setMode(pwmHl, Ifx_Pwm_Mode_off);
            IfxGtm_Tom_Timer_disableUpdate(timer);
            IfxGtm_Tom_PwmHl_setOnTime(pwmHl, NULL_PTR);
            IfxGtm_Tom_Timer_applyUpdate(timer);
            break;
        case GtmTomPwmHl_State_duty0:
            /* Set 0% duty cycle, center aligned */
            tOn[0] = 0;
            tOn[1] = 0;
            IfxGtm_Tom_PwmHl_setMode(pwmHl, Ifx_Pwm_Mode_centerAligned);
            IfxGtm_Tom_Timer_disableUpdate(timer);
            IfxGtm_Tom_PwmHl_setOnTime(pwmHl, tOn);
            IfxGtm_Tom_Timer_applyUpdate(timer);
            break;
        case GtmTomPwmHl_State_duty50:
            /* Set 50% duty cycle, center aligned */
            tOn[0] = IfxGtm_Tom_Timer_getPeriod(timer) / 2;
            tOn[1] = IfxGtm_Tom_Timer_getPeriod(timer) / 2;
            IfxGtm_Tom_PwmHl_setMode(pwmHl, Ifx_Pwm_Mode_centerAligned);
            IfxGtm_Tom_Timer_disableUpdate(timer);
            IfxGtm_Tom_PwmHl_setOnTime(pwmHl, tOn);
            IfxGtm_Tom_Timer_applyUpdate(timer);
            break;
        case GtmTomPwmHl_State_duty100:
            /* Set 100% duty cycle, center aligned */
            IfxGtm_Tom_PwmHl_setMode(pwmHl, Ifx_Pwm_Mode_centerAligned);
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
