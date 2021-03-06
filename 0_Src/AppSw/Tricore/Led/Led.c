/**
 * \file LED.c
 * \brief 
 */

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/
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

/******************************************************************************/
/*------------------------------Global variables------------------------------*/
/******************************************************************************/

App_Led g_Led;

/******************************************************************************/
/*-------------------------Function Prototypes--------------------------------*/
/******************************************************************************/

void Led_init(void);
void Led_toggle(void);
void Led_on(void);
void Led_off(void);
void Led_BlinkSeveralTimes(int numberOfBlinking);

/******************************************************************************/
/*------------------------Private Variables/Constants-------------------------*/
/******************************************************************************/

/******************************************************************************/
/*-------------------------Function Implementations---------------------------*/
/******************************************************************************/

void Led_init(void){
    g_Led.outputPin.port = &MODULE_P10;
    g_Led.outputPin.pinIndex = 2;
    
    IfxPort_setPinMode(g_Led.outputPin.port, g_Led.outputPin.pinIndex, IfxPort_Mode_outputPushPullGeneral);
    Led_on();    
}

void Led_toggle(void){
    IfxPort_togglePin(g_Led.outputPin.port, g_Led.outputPin.pinIndex);
}

void Led_on(void){
    IfxPort_setPinHigh(g_Led.outputPin.port, g_Led.outputPin.pinIndex);
}

void Led_off(void){
    IfxPort_setPinLow(g_Led.outputPin.port, g_Led.outputPin.pinIndex);
}

void Led_BlinkSeveralTimes(int numberOfBlinking){
    int i = 0;
    for(i=0; i < numberOfBlinking; i++){
        Led_on();
        IfxStm_waitTicks(g_Stm.stmSfr, TimeConst_100ms * 2.5);
        Led_off();
        IfxStm_waitTicks(g_Stm.stmSfr, TimeConst_100ms * 2.5);
    }
    Led_on();
}