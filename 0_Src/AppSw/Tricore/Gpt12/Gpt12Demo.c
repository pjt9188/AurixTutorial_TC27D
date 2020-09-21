/**
 * \file Gpt12.c
 * \brief Demo Gpt12
 *
 * \version iLLD_Demos_1_0_1_8_0
 * \copyright Copyright (c) 2013 Infineon Technologies AG. All rights reserved.
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
#include "Gpt12Demo.h"
#include "_Utilities/Ifx_Assert.h"
#include "Port/Std/IfxPort.h"
#include "SysSe/Bsp/Bsp.h"

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
App_Gpt12 g_Gpt12; /**< \brief Demo information */

/******************************************************************************/
/*-------------------------Function Prototypes--------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*------------------------Private Variables/Constants-------------------------*/
/******************************************************************************/
/******************************************************************************/
/*-------------------------Function Implementations---------------------------*/
/******************************************************************************/
/** \addtogroup IfxLld_Demo_Gpt12_SrcDoc_Main_Interrupt
 * \{ */

/** \name Interrupts for IncrIncZero
 * \{ */
IFX_INTERRUPT(ISR_IncrIncZero, 0, ISR_PRIORITY_INCRENC_ZERO);
/** \} */

/** \} */

/** \brief Handle IncrIncZero interrupt.
 *
 * \isrProvider \ref ISR_PROVIDER_INCRENC_ZERO
 * \isrPriority \ref ISR_PRIORITY_INCRENC_ZERO
 *
 */
void ISR_IncrIncZero(void)
{
    __enable();
    IfxGpt12_IncrEnc_onZeroIrq(&g_Gpt12.incrEnc);
}


/** Demo init API
 *
 * This function is call from the main, before interrupt enable
 */
void Gpt12Demo_init(void)
{
    //    printf("Gpt12_init() called. \n");

    initTime();

    /* Initialize global clocks */
    /* FIXME Global prescaller should not be set by the driver as they are global resources */
    IfxGpt12_enableModule(&MODULE_GPT120);
    IfxGpt12_setGpt1BlockPrescaler(&MODULE_GPT120, IfxGpt12_Gpt1BlockPrescaler_8);
    IfxGpt12_setGpt2BlockPrescaler(&MODULE_GPT120, IfxGpt12_Gpt2BlockPrescaler_4);

    IfxGpt12_IncrEnc_Config config;
    IfxGpt12_IncrEnc_initConfig(&config, &MODULE_GPT120);
#if 1
    {
        /* Test implementation with T3 as core */
        config.base.offset                    = 0;                                          // Position sensor offset
        config.base.reversed                  = FALSE;
        config.base.resolution                = 2048;                                       // Sensor resolution. For encoder with 1024 pulse per revolution, the value should be 1024
        config.base.periodPerRotation         = 1;                                          // Number of period per rotation. Is usually 1 for encoder
        config.base.resolutionFactor          = IfxStdIf_Pos_ResolutionFactor_fourFold;     // Resolution multiplier for encoder interface, valid is 2, 4.
        config.base.updatePeriod              = 100e-6;                                     // refresh period in seconds, at which the application calls IfxStdIf_Pos_update()
        /*
        * Speed threshold(rad/s) used for the speed calculation mode.
        * For encoder, above the threshold the pulse count mode is used, -> updatePeriod 내에서 증가한 count 갯수 * 한 update period에 1 count가 올라갈 때의 속도(rad/s) (rad / (1count * 1 updatePeriod))
        * below the threshold, the time delta is used 
        * GPT12의 T5 timer는 Gpt12의 미리 정해진 주파수에 따라서 timer가 돌도록 되어있다.
        * T5에서는 T3IN의 모든 edge마다 T5의 timer를 캡쳐하도록 설정되어 있다.
        * -> 따라서 T5에 capture된 결과는 T3IN의 반주기 길이의 T5 timer ticks수가 저장된다.
        * -> T5 timer의 주파수를 반주기 길이의 T5 timer ticks수로 나누면 초당 몇번의 반주기가 실행될 수 있는지가 계산되고
        * -> 이를 반주기기의 각도 크기 (2pi / (resolution * 2))와 곱해주면 초당 encoder의 회전속도를 구할 수 있다.
        */
        config.base.speedModeThreshold        = 200;                                        // (rad/s)
        config.base.minSpeed                  = 10;                                         // (rad/s) Absolute minimal allowed speed. below speed is recognized as 0rad/s
        config.base.maxSpeed                  = 500; //(4774 rpm)                           // (rad/s) Absolute maximal allowed speed. Above speed is recognized as error
        config.zeroIsrPriority                = ISR_PRIORITY(INTERRUPT_INCRINC_ZERO);       // Interrupt isrPriority of the zero interrupt, if 0 the interrupt is disable
        config.zeroIsrProvider                = ISR_PROVIDER(INTERRUPT_INCRINC_ZERO);
        config.pinA                           = &IfxGpt120_T3INA_P02_6_IN;
        config.pinB                           = &IfxGpt120_T3EUDA_P02_7_IN;
        config.pinZ                           = &IfxGpt120_T4INA_P02_8_IN;
        config.pinMode                        = IfxPort_InputMode_noPullDevice;

        config.base.speedFilterEnabled        = TRUE;
        config.base.speedFilerCutOffFrequency = config.base.maxSpeed / 2 * IFX_PI * 2;      // Speed low pass filter cut off frequency
    }
#else
    {
        /* Test implementation with T2 as core */
        config.base.offset                    = 100;
        config.base.reversed                  = FALSE;
        config.base.resolution                = 2048;
        config.base.periodPerRotation         = 1;
        config.base.resolutionFactor          = IfxStdIf_Pos_ResolutionFactor_fourFold;
        config.base.updatePeriod              = 100e-6;
        config.base.speedModeThreshold        = 200;
        config.base.minSpeed                  = 10;
        config.base.maxSpeed                  = 500;
        config.zeroIsrPriority                = ISR_PRIORITY(INTERRUPT_INCRINC_ZERO);
        config.zeroIsrProvider                = ISR_PROVIDER(INTERRUPT_INCRINC_ZERO);
        config.pinA                           = &IfxGpt120_T2INA_P00_7_IN;
        config.pinB                           = &IfxGpt120_T2EUDA_P00_8_IN;
        config.pinZ                           = &IfxGpt120_T4EUDA_P00_9_IN;
        config.pinMode                        = IfxPort_InputMode_noPullDevice;

        config.base.speedFilterEnabled        = TRUE;
        config.base.speedFilerCutOffFrequency = config.base.maxSpeed / 2 * IFX_PI * 10;
    }
#endif
    IFX_VALIDATE(IFX_VERBOSE_LEVEL_ERROR, IfxGpt12_IncrEnc_init(&g_Gpt12.incrEnc, &config));

    g_Gpt12.control.run                  = TRUE;
    g_Gpt12.control.direction            = IfxStdIf_Pos_Dir_forward;
    g_Gpt12.control.resolution           = config.base.resolution;
    g_Gpt12.control.step                 = 3;                                           // Current step of the emulated encoder
    g_Gpt12.control.rawPosition          = config.base.offset;
    g_Gpt12.control.speed                = IFX_PI * 2.0;                 // Requested encoder speed in rad/s
    g_Gpt12.control.multiplicationFactor = config.base.resolutionFactor;
    g_Gpt12.control.updatePeriod         = config.base.updatePeriod;
    g_Gpt12.control.positionMask         = g_Gpt12.control.resolution * g_Gpt12.control.multiplicationFactor - 1;       
    // raw Position 의 경우 예를들어 PPR = 2048이고, 두 신호의 모든 edge에서 count를 할 때, 한 바퀴당 edge는 2048*4 번 만나게 된다.
    // 즉 한바퀴를 8192개로 쪼개서 encoder의 위치를 알 수 있고, 각 위치를 0~8191의 숫자로 나타낼 수 있다.
    // 이를 넘어갔을 때 0으로 reset하기 위해서 position 8191을 나타내는 position mask를 만든다.

    g_Gpt12.interface.A                  = config.pinA->pin;
    g_Gpt12.interface.B                  = config.pinB->pin;
    g_Gpt12.interface.Z                  = config.pinZ->pin;

    /* Overwrite port configuration to enable feature check
    * Emulate한 신호를 Gpt12의 input port에 발생시키기 이해서 input port들을 output(push-pull general)으로 설정하여 output 신호를 모사
    * 해당 포트를 output으로 바꾸어도 Pxx_In에서 신호를 받아 들이기 때문에 emulate 한 신호를 GPT12가 읽을 수 있음
    */

    IfxPort_setPinModeOutput(g_Gpt12.interface.A.port, g_Gpt12.interface.A.pinIndex, IfxPort_OutputMode_pushPull,
        IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(g_Gpt12.interface.B.port, g_Gpt12.interface.B.pinIndex, IfxPort_OutputMode_pushPull,
        IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(g_Gpt12.interface.Z.port, g_Gpt12.interface.Z.pinIndex, IfxPort_OutputMode_pushPull,
        IfxPort_OutputIdx_general);
}


void Gpt12Demo_step(void)
{
    if (g_Gpt12.control.run)
    {
        sint32 rawPosition;
        rawPosition = g_Gpt12.control.rawPosition;

        if (g_Gpt12.control.direction == IfxStdIf_Pos_Dir_forward)
        {
            g_Gpt12.control.step++;
            g_Gpt12.control.step &= 0x3;        // 총 4개의 step으로 나뉘어지므로 항상 0~3의 값을 유지하기 위해 and 취해줌
            rawPosition++;
        }
        else
        {
            g_Gpt12.control.step--;
            g_Gpt12.control.step &= 0x3;
            rawPosition--;
        }

        rawPosition                &= g_Gpt12.control.positionMask;
        g_Gpt12.control.rawPosition = rawPosition;

#if 1

        /* Handle zero pin */
        if (g_Gpt12.control.direction == IfxStdIf_Pos_Dir_forward)          // Z상의 신호를 emulate하기 위한 코드로, 한바퀴 돌때(rawposion == 0 일 때) high signal을 보낸다
        {
            if (rawPosition == 0)
            {
                IfxPort_setPinHigh(g_Gpt12.interface.Z.port, g_Gpt12.interface.Z.pinIndex);
            }
            else if (rawPosition == 1)
            {
                IfxPort_setPinLow(g_Gpt12.interface.Z.port, g_Gpt12.interface.Z.pinIndex);
            }
        }
        else
        {
            if (rawPosition == g_Gpt12.control.positionMask)
            {
                IfxPort_setPinHigh(g_Gpt12.interface.Z.port, g_Gpt12.interface.Z.pinIndex);
            }
            else if (rawPosition == g_Gpt12.control.positionMask - 1)
            {
                IfxPort_setPinLow(g_Gpt12.interface.Z.port, g_Gpt12.interface.Z.pinIndex);
            }
        }

#endif

        switch (g_Gpt12.control.step)
        {
        case 0:
            IfxPort_setPinLow(g_Gpt12.interface.A.port, g_Gpt12.interface.A.pinIndex);
            IfxPort_setPinLow(g_Gpt12.interface.B.port, g_Gpt12.interface.B.pinIndex);
            break;
        case 1:
            IfxPort_setPinLow(g_Gpt12.interface.A.port, g_Gpt12.interface.A.pinIndex);
            IfxPort_setPinHigh(g_Gpt12.interface.B.port, g_Gpt12.interface.B.pinIndex);
            break;
        case 2:
            IfxPort_setPinHigh(g_Gpt12.interface.A.port, g_Gpt12.interface.A.pinIndex);
            IfxPort_setPinHigh(g_Gpt12.interface.B.port, g_Gpt12.interface.B.pinIndex);
            break;
        case 3:
            IfxPort_setPinHigh(g_Gpt12.interface.A.port, g_Gpt12.interface.A.pinIndex);
            IfxPort_setPinLow(g_Gpt12.interface.B.port, g_Gpt12.interface.B.pinIndex);
            break;
        }
    }
}


/** Demo run API
 *
 * This function is call from the main, background loop
 */
void Gpt12Demo_run(void)
{
    printf("Gpt12_run() called \n");
    Ifx_TickTime tickPeriod;
    Ifx_TickTime tickRefresh;

    Ifx_TickTime refreshDeadLine;
    Ifx_TickTime tickDeadLine;

    tickRefresh     = g_Gpt12.control.updatePeriod * TimeConst_1s;

    refreshDeadLine = now();
    tickDeadLine    = refreshDeadLine;

    while (1)
    {
        /* 가상으로 만든 encoder 신호를 발생시키는 code */
        g_Gpt12.control.run = g_Gpt12.control.speed != 0.0;

        if (!g_Gpt12.control.run)                                                                   
        {}
        else
        {
            float32 roundDuration_s = (2.0 * IFX_PI) / g_Gpt12.control.speed;
            float32 tickDuration_s  = roundDuration_s / (g_Gpt12.control.positionMask + 1);

            tickPeriod = tickDuration_s * TimeConst_1s;

            if (isDeadLine(tickDeadLine))
            {
                tickDeadLine = addTTime(tickDeadLine, tickPeriod);
                Gpt12Demo_step();
            }
        }

        /* encoder에서 신호를 읽는 code */
        if (isDeadLine(refreshDeadLine))
        {
            refreshDeadLine = addTTime(refreshDeadLine, tickRefresh);
            IfxGpt12_IncrEnc_update(&g_Gpt12.incrEnc);

            g_Gpt12.status.speed       = IfxGpt12_IncrEnc_getSpeed(&g_Gpt12.incrEnc);
            g_Gpt12.status.rawPosition = IfxGpt12_IncrEnc_getRawPosition(&g_Gpt12.incrEnc);
            g_Gpt12.status.direction   = IfxGpt12_IncrEnc_getDirection(&g_Gpt12.incrEnc);
        }
    }
}
