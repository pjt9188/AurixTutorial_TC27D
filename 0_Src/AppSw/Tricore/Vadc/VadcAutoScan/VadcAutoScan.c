/**
 * \file VadcAutoScanDemo.c
 * \brief Demo VadcAutoScanDemo
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
#include <Cpu/Std/IfxCpu.h>
#include "VadcAutoScan.h"
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
App_VadcAutoScan g_VadcAutoScan; /**< \brief Demo information */

/******************************************************************************/
/*-------------------------Function Prototypes--------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*------------------------Private Variables/Constants-------------------------*/
/******************************************************************************/

/******************************************************************************/
/*-------------------------Function Implementations---------------------------*/
/******************************************************************************/

/** \brief Demo init API
 *
 * This function is called from main during initialization phase
 */
void VadcAutoScan_init(void)
{
    /* VADC Configuration */

    /* create configuration */
    IfxVadc_Adc_Config adcConfig;
    IfxVadc_Adc_initModuleConfig(&adcConfig, &MODULE_VADC);

    /* initialize module */
    IfxVadc_Adc_initModule(&g_VadcAutoScan.vadc, &adcConfig);

    /* create group config */
    IfxVadc_Adc_GroupConfig adcGroupConfig;
    IfxVadc_Adc_initGroupConfig(&adcGroupConfig, &g_VadcAutoScan.vadc);

    /* with group 0 */
    adcGroupConfig.groupId = IfxVadc_GroupId_4;         // Group 4번 사용
    adcGroupConfig.master  = adcGroupConfig.groupId;

    /* enable scan source */
    adcGroupConfig.arbiter.requestSlotScanEnabled = TRUE;

    /* enable auto scan */
    adcGroupConfig.scanRequest.autoscanEnabled = TRUE;

    /* enable all gates in "always" mode (no edge detection) */
    adcGroupConfig.scanRequest.triggerConfig.gatingMode = IfxVadc_GatingMode_always;

    /* initialize the group */
    /*IfxVadc_Adc_Group adcGroup;*/    //declared globally
    IfxVadc_Adc_initGroup(&g_VadcAutoScan.adcGroup, &adcGroupConfig);

    uint32                    chnIx;
    /* create channel config */
    IfxVadc_Adc_ChannelConfig adcChannelConfig[8];      // Group4에 channel이 8개 존재하므로
    for (chnIx = 7; chnIx < 8; ++chnIx)                 // 채널 7번만 사용
    {
        IfxVadc_Adc_initChannelConfig(&adcChannelConfig[chnIx], &g_VadcAutoScan.adcGroup);

        adcChannelConfig[chnIx].channelId      = (IfxVadc_ChannelId)(chnIx);
        adcChannelConfig[chnIx].resultRegister = (IfxVadc_ChannelResult)(chnIx);  /* use dedicated result register */

        /* initialize the channel */
        IfxVadc_Adc_initChannel(&g_VadcAutoScan.adcChannel[chnIx], &adcChannelConfig[chnIx]);

        /* add to scan */
        unsigned channels = (1 << adcChannelConfig[chnIx].channelId);
        unsigned mask     = channels;
        IfxVadc_Adc_setScan(&g_VadcAutoScan.adcGroup, channels, mask);
    }

    /* 
    * Port 초기화(ADC4.7 -> P32.3 -> Analog pin 0(ADCL.1)) : General Purpose Input
    * Default로 General Purpose Input으로 되어 있지만, 만약을 위해 init함수에 추가
    */
    g_VadcAutoScan.inputPin.port = &MODULE_P32;
    g_VadcAutoScan.inputPin.pinIndex = 3;
    IfxPort_setPinMode(g_VadcAutoScan.inputPin.port, g_VadcAutoScan.inputPin.pinIndex, IfxPort_Mode_inputNoPullDevice);

    /* start autoscan */
    IfxVadc_Adc_startScan(&g_VadcAutoScan.adcGroup);
}


/** \brief Demo run API
 *
 * This function is called from main, background loop
 */
void VadcAutoScan_run(void)
{
    printf("VadcAutoScan_run() called\n");

    uint32                    chnIx;

    /* check results */
    for (chnIx = 7; chnIx < 8; ++chnIx)
    {
        unsigned     group   = g_VadcAutoScan.adcChannel[chnIx].group->groupId;
        unsigned     channel = g_VadcAutoScan.adcChannel[chnIx].channel;

        /* wait for valid result */
        Ifx_VADC_RES conversionResult;

        do
        {
            conversionResult = IfxVadc_Adc_getResult(&g_VadcAutoScan.adcChannel[chnIx]);
        } while (!conversionResult.B.VF);

        uint32 actual = conversionResult.B.RESULT;
        /* print result, check with expected value */
        {
            /* FIXME result verification pending ?? */
            printf("Group %d Channel %d  : %lu\n", group, channel, actual);
        }
    }
}
