/**
 * \file VadcAutoScanDemo.h
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
 *
 * \defgroup IfxLld_Demo_VadcAutoScanDemo_SrcDoc_Main Demo Source
 * \ingroup IfxLld_Demo_VadcAutoScanDemo_SrcDoc
 * \defgroup IfxLld_Demo_VadcAutoScanDemo_SrcDoc_Main_Interrupt Interrupts
 * \ingroup IfxLld_Demo_VadcAutoScanDemo_SrcDoc_Main
 */

#ifndef VADCAUTOSCAN_H
#define VADCAUTOSCAN_H 1

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/
#include <Vadc/Std/IfxVadc.h>
#include <Vadc/Adc/IfxVadc_Adc.h>

/******************************************************************************/
/*-----------------------------------Macros-----------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*--------------------------------Enumerations--------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*-----------------------------Data Structures--------------------------------*/
/******************************************************************************/
typedef struct
{
    IfxVadc_Adc vadc; /* VADC handle */
    IfxVadc_Adc_Group adcGroup;
    IfxVadc_Adc_Channel adcChannel[8];
    uint16 adcValue[8];
    IfxPort_Pin inputPin[8];
} App_VadcAutoScan;

/******************************************************************************/
/*------------------------------Global variables------------------------------*/
/******************************************************************************/
IFX_EXTERN App_VadcAutoScan g_VadcAutoScan;

/******************************************************************************/
/*-------------------------Function Prototypes--------------------------------*/
/******************************************************************************/
IFX_EXTERN void VadcAutoScan_init(void);
IFX_EXTERN void VadcAutoScan_run(void);

#endif
