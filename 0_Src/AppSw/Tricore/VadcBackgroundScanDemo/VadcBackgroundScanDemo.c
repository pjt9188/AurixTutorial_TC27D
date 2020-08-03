/**
 * \file VadcBackgroundScanDemo.c
 * \brief Demo VadcBackgroundScanDemo
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
#include "VadcBackgroundScanDemo.h"
#include <Cpu/Std/IfxCpu.h>
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
App_VadcBackgroundScan g_VadcBackgroundScan; /**< \brief Demo information */

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
void VadcBackgroundScanDemo_init(void)
{
    /* VADC Configuration */

    /* create module configuration */
    IfxVadc_Adc_Config          adcConfig;              // create module configuration
    /* initialize module config */
    IfxVadc_Adc_initModuleConfig(&adcConfig, &MODULE_VADC);
    /* initialize module */
    IfxVadc_Adc_initModule(&g_VadcBackgroundScan.vadc, &adcConfig);

    /* create gruop configuration */
    IfxVadc_Adc_GroupConfig     adcGroupConfig;         // create Group Configurtaion
    /* initialize group config */
    IfxVadc_Adc_initGroupConfig(&adcGroupConfig, &g_VadcBackgroundScan.vadc);
    /* with group 0 */
    adcGroupConfig.groupId = IfxVadc_GroupId_0;
    adcGroupConfig.master  = adcGroupConfig.groupId;
    /* enable background scan source */
    adcGroupConfig.arbiter.requestSlotBackgroundScanEnabled = TRUE;
    /* enable background auto scan */
    adcGroupConfig.backgroundScanRequest.autoBackgroundScanEnabled = TRUE;
    /* enable all gates in "always" mode (no edge detection) */
    adcGroupConfig.backgroundScanRequest.triggerConfig.gatingMode = IfxVadc_GatingMode_always;
    /* initialize the group */
    IfxVadc_Adc_initGroup(&g_VadcBackgroundScan.adcGroup, &adcGroupConfig);


    uint32                      chnIx;                  // Background scan에 2채널을 추가    
    /* create channel config */
    IfxVadc_Adc_ChannelConfig   adcChannelConfig[2];  

    /* initialize Channel Configuration */
    for (chnIx = 0; chnIx < 2; ++chnIx)
    {
       IfxVadc_Adc_initChannelConfig(&adcChannelConfig[chnIx], &g_VadcBackgroundScan.adcGroup);

       adcChannelConfig[chnIx].channelId         = (IfxVadc_ChannelId)(0 + chnIx);
       adcChannelConfig[chnIx].resultRegister    = (IfxVadc_ChannelResult)(0 + chnIx); // use register #0 and 1 for results
       adcChannelConfig[chnIx].backgroundChannel = TRUE;

       // 채널 초기화
       IfxVadc_Adc_initChannel(&g_VadcBackgroundScan.adcChannel[chnIx], &adcChannelConfig[chnIx]);

       // background scan에 추가
       unsigned channels = (1 << adcChannelConfig[chnIx].channelId);
       unsigned mask     = channels;
       IfxVadc_Adc_setBackgroundScan(&g_VadcBackgroundScan.vadc, &g_VadcBackgroundScan.adcGroup, channels, mask);
    }

    // start scan
    IfxVadc_Adc_startBackgroundScan(&g_VadcBackgroundScan.vadc);
}


/** \brief Demo run API
 *
 * This function is called from main, background loop
 */
void VadcBackgroundScanDemo_run(void)
{
    uint32              chnIx;
    // uint32              actual;
    // IfxVadc_GroupId     group;
    // IfxVadc_ChannelId   channel;

    volatile Ifx_VADC_RES conversionResult;

    printf("VadcBackgroundScanDemo_run() called\n");

    for (chnIx = 0; chnIx < 2; ++chnIx)
    {
        volatile unsigned     group   = g_VadcBackgroundScan.adcChannel[chnIx].group->groupId;
        volatile unsigned     channel = g_VadcBackgroundScan.adcChannel[chnIx].channel;
        
        /* wait for valid result */
        do
        {
            conversionResult = IfxVadc_Adc_getResult(&g_VadcBackgroundScan.adcChannel[chnIx]);
        } while (!conversionResult.B.VF);       // conversionResult.B.VF; 유효데이터임을 알려주는 valid flag

        uint32 actual = conversionResult.B.RESULT;

        printf("Group %d Channel %d  : %lu\n", group, channel, actual);
    }
}
