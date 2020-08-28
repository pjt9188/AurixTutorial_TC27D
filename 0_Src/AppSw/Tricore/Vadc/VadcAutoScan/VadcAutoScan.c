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

    /* change converting resolution */
    /* Background Scan Source의 Resolution 선택 */
    // adcConfig.globalInputClass[0].resolution = IfxVadc_ChannelResolution_8bit;
    // adcConfig.globalInputClass[1].resolution = IfxVadc_ChannelResolution_8bit;

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

    /* change converting resolution */
    /* Group Scan Source의 Resolution 선택*/
    adcGroupConfig.inputClass[0].resolution = IfxVadc_ChannelResolution_8bit;
    adcGroupConfig.inputClass[1].resolution = IfxVadc_ChannelResolution_8bit;

    /* initialize the group */
    /*IfxVadc_Adc_Group adcGroup;*/    //declared globally
    IfxVadc_Adc_initGroup(&g_VadcAutoScan.adcGroup, &adcGroupConfig);

    uint32                    chnIx;
    /* create channel config */
    IfxVadc_Adc_ChannelConfig adcChannelConfig[8];      // Group4에 channel이 8개 존재하므로
    for (chnIx = 4; chnIx < 8; ++chnIx)                 // 채널 4,5,6,7을 스캔으로 사용
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
    * Port 초기화
    * SAR 4.4 -> P23.2 -> Analog pin 3(ADCL.4)) : General Purpose Input
    * SAR 4.5 -> P32.1 -> Analog pin 2(ADCL.3)) : General Purpose Input
    * SAR 4.6 -> P32.4 -> Analog pin 1(ADCL.2)) : General Purpose Input
    * SAR 4.7 -> P32.3 -> Analog pin 0(ADCL.1)) : General Purpose Input
    * Default로 General Purpose Input으로 되어 있지만, 만약을 위해 init함수에 추가
    */
    g_VadcAutoScan.inputPin[4].port = &MODULE_P23;
    g_VadcAutoScan.inputPin[4].pinIndex = 2;
    IfxPort_setPinMode(g_VadcAutoScan.inputPin[4].port, g_VadcAutoScan.inputPin[4].pinIndex, IfxPort_Mode_inputNoPullDevice);
    
    g_VadcAutoScan.inputPin[5].port = &MODULE_P32;
    g_VadcAutoScan.inputPin[5].pinIndex = 1;
    IfxPort_setPinMode(g_VadcAutoScan.inputPin[5].port, g_VadcAutoScan.inputPin[5].pinIndex, IfxPort_Mode_inputNoPullDevice);

    g_VadcAutoScan.inputPin[6].port = &MODULE_P32;
    g_VadcAutoScan.inputPin[6].pinIndex = 4;
    IfxPort_setPinMode(g_VadcAutoScan.inputPin[6].port, g_VadcAutoScan.inputPin[6].pinIndex, IfxPort_Mode_inputNoPullDevice);

    g_VadcAutoScan.inputPin[7].port = &MODULE_P32;
    g_VadcAutoScan.inputPin[7].pinIndex = 3;
    IfxPort_setPinMode(g_VadcAutoScan.inputPin[7].port, g_VadcAutoScan.inputPin[7].pinIndex, IfxPort_Mode_inputNoPullDevice);

    /* start autoscan */
    IfxVadc_Adc_startScan(&g_VadcAutoScan.adcGroup);

    printf("Vadc Autoscan Initialized\n");
}


/** \brief Demo run API
 *
 * This function is called from main, background loop
 */
void VadcAutoScan_run(void)
{
    // printf("VadcAutoScan_run() called\n");

    uint32                    chnIx;

    /* check results */
    for (chnIx = 4; chnIx < 8; ++chnIx)
    {
        // unsigned     group   = g_VadcAutoScan.adcChannel[chnIx].group->groupId;
        // unsigned     channel = g_VadcAutoScan.adcChannel[chnIx].channel;

        /* wait for valid result */
        Ifx_VADC_RES conversionResult;

        do
        {
            conversionResult = IfxVadc_Adc_getResult(&g_VadcAutoScan.adcChannel[chnIx]);
        } while (!conversionResult.B.VF);
        
        /*  save the result in adcValue
         * 8bit, 10bit resolution인 경우 앞에서 부터 결과가 채워지므로, 뒤에 각각 2개/4개의 0bit가 남는다
         * 따라서 아래의 경우 8bit 분해능이므로 4개의 bit를 오른쪽으로 shift 해준다.
        */

        g_VadcAutoScan.adcValue[chnIx] = (conversionResult.B.RESULT >> 4);

        /* print result, check with expected value */
        // printf("Group %d Channel %lu  : %hu\n", g_VadcAutoScan.adcGroup.groupId, chnIx, g_VadcAutoScan.adcValue[chnIx]);
    }
}
