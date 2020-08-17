/**
 * \file GtmTom_Pwm.c
 * \brief 
 */

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

#include "GtmTom_Pwm.h"
#include "ConfigurationIsr.h"
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

App_GtmTomPwm g_GtmTom_Pwm;

/******************************************************************************/
/*-------------------------Function Prototypes--------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*------------------------Private Variables/Constants-------------------------*/
/******************************************************************************/

/******************************************************************************/
/*-------------------------Function Implementations---------------------------*/
/******************************************************************************/

void GtmTom_Pwm_init(void){
    g_GtmTom_Pwm.gtm = &MODULE_GTM;

    /* enable GTM clock */
    // Get GTM frequeny(SYS_CLK = 100Mhz) from CCU(Clock Control Unit)
    float32 frequency = IfxGtm_Cmu_getModuleFrequency(g_GtmTom_Pwm.gtm);
    // Enables the GTM
    IfxGtm_enable(g_GtmTom_Pwm.gtm);
    // Set the global clock frequency to the max(CMU_GCLK_EN = 100Mhz)
    IfxGtm_Cmu_setGclkFrequency(g_GtmTom_Pwm.gtm, frequency);
    // Set the CMU CLK0(CMU_CLK0 = 100Mhz / 100 = 1Mhz)
    IfxGtm_Cmu_setClkFrequency(g_GtmTom_Pwm.gtm, IfxGtm_Cmu_Clk_0, frequency / 100);
    // Enable FXCLK of which source is set as CMU_CLK0 (FXCLK is used by TOM and CLK0: used by ATOM)
    IfxGtm_Cmu_enableClocks(g_GtmTom_Pwm.gtm, IFXGTM_CMU_CLKEN_FXCLK | IFXGTM_CMU_CLKEN_CLK0);
    
    IfxGtm_Tom_Pwm_initConfig(&g_GtmTom_Pwm.config, g_GtmTom_Pwm.gtm);
    g_GtmTom_Pwm.config.gtm                      = &MODULE_GTM;
    g_GtmTom_Pwm.config.tom                      = IfxGtm_Tom_0;
    g_GtmTom_Pwm.config.tomChannel               = IfxGtm_Tom_Ch_2;
    g_GtmTom_Pwm.config.clock                    = IfxGtm_Tom_Ch_ClkSrc_cmuFxclk0;  // CMU_FXCLK0 = CMU_CLK0 / 1 =  1Mhz 
    g_GtmTom_Pwm.config.signalLevel              = Ifx_ActiveState_high;
    g_GtmTom_Pwm.config.period                   = 20000;                           // period = 50ms = 20000 tics / 1Mhz
    g_GtmTom_Pwm.config.dutyCycle                = 1500;                            // dutyCycle = 1500us
    g_GtmTom_Pwm.config.synchronousUpdateEnabled = TRUE;
    // g_GtmTom_Pwm.config.interrupt.ccu0Enabled    = TRUE;
    // g_GtmTom_Pwm.config.interrupt.isrPriority    = ISR_PRIORITY_TOM0_CH2;
    g_GtmTom_Pwm.config.pin.outputPin            = &IfxGtm_TOM0_2_TOUT104_P10_2_OUT;
    IfxGtm_Tom_Pwm_init(&g_GtmTom_Pwm.driver, &g_GtmTom_Pwm.config);
}

void GtmTom_Pwm_changeDutyCycle(uint32 dutyCycle){
    /* Change Duty Cycle in configuration */
    g_GtmTom_Pwm.config.dutyCycle = dutyCycle;

    /* Apply changed DutyCycle */
    IfxGtm_Tom_Pwm_Config *config = &g_GtmTom_Pwm.config;
    IfxGtm_Tom_Pwm_Driver *driver = &g_GtmTom_Pwm.driver;
    Ifx_GTM_TOM *tomSFR = &config->gtm->TOM[config->tom];

    if (config->synchronousUpdateEnabled == 1)
    {
        IfxGtm_Tom_Ch_setCompareOneShadow(tomSFR, config->tomChannel, config->dutyCycle);
        IfxGtm_Tom_Tgc_trigger(driver->tgc[0]);
    }
    else
    {
        IfxGtm_Tom_Ch_setCompareOne(tomSFR, config->tomChannel, config->dutyCycle);
    }

    if (config->immediateStartEnabled == TRUE)
    {
        IfxGtm_Tom_Tgc_trigger(driver->tgc[0]);
    }
}

void GtmTom_Pwm_run(void){
    int i;
    for(i = 0; i<10; i++){
        GtmTom_Pwm_changeDutyCycle(2000 * i);
        IfxStm_waitTicks(g_Stm.stmSfr, TimeConst_10ms * 5);
    }
    for(i = 0; i<10; i++){
        GtmTom_Pwm_changeDutyCycle(20000 - 2000 * i);
        IfxStm_waitTicks(g_Stm.stmSfr, TimeConst_10ms * 5);
    }
}