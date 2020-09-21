/**
 * \file ConfigurationIsr.h
 * \brief Interrupts configuration.
 *
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
 * \defgroup IfxLld_Demo_StmDemo_SrcDoc_InterruptConfig Interrupt configuration
 * \ingroup IfxLld_Demo_StmDemo_SrcDoc
 */

#ifndef CONFIGURATIONISR_H
#define CONFIGURATIONISR_H
/******************************************************************************/
/*-----------------------------------Macros-----------------------------------*/
/******************************************************************************/

/** \brief Build the ISR configuration object
 * \param no interrupt priority
 * \param cpu assign CPU number
 */
#define ISR_ASSIGN(no, cpu)  ((no << 8) + cpu)

/** \brief extract the priority out of the ISR object */
#define ISR_PRIORITY(no_cpu) (no_cpu >> 8)

/** \brief extract the service provider  out of the ISR object */
#define ISR_PROVIDER(no_cpu) (no_cpu % 8)
/**
 * \addtogroup IfxLld_Demo_StmDemo_SrcDoc_InterruptConfig
 * \{ */

/**
 * \name Interrupt priority configuration.
 * The interrupt priority range is [1,255]
 * \{
 */

#define ISR_PRIORITY_ASC_3_RX       4       /**< \brief Define the ASCLIN1 receive interrupt priority.  */
#define ISR_PRIORITY_ASC_3_TX       8       /**< \brief Define the ASCLIN1 transmit interrupt priority.  */
#define ISR_PRIORITY_ASC_3_EX       12      /**< \brief Define the ASCLIN1 error interrupt priority.  */

#define ISR_PRIORITY_STM_STM0SR0    40      /**< \brief Define the System Timer Interrupt priority.  */

#define ISR_PRIORITY_TOM0_CH2       20      /**< \brief Define the GTM TOM Interrupt priority.  */
#define ISR_PRIORITY_TIMER          1       /**< \brief Define the 1ms timer interrupt priority.  */

#define ISR_PRIORITY_INCRENC_ZERO   6       /**< \brief Define the Incremental encoder zero interrupt priority.  */
/** \} */

/**
 * \name Interrupt service provider configuration.
 * \{ */


#define ISR_PROVIDER_ASC_3          IfxSrc_Tos_cpu0         /**< \brief Define the ASCLIN 3 interrupt provider.  */
#define ISR_PROVIDER_STM_STM0SR0    IfxSrc_Tos_cpu0         /**< \brief Define the System Timer interrupt provider.  */

#define ISR_PROVIDER_TIMER          IfxSrc_Tos_cpu0         /**< \brief Define the 1ms timer interrupt provider.  */

#define ISR_PROVIDER_INCRENC_ZERO   IfxSrc_Tos_cpu0          /**< \brief Define the Incremental encoder zero interrupt provider.  */
/** \} */

/**
 * \name Interrupt configuration.
 * \{ */

#define INTERRUPT_ASC_3_RX          ISR_ASSIGN(ISR_PRIORITY_ASC_3_RX, ISR_PROVIDER_ASC_3)               /**< \brief Install the ASCLIN1 receive interrupt handler.  */
#define INTERRUPT_ASC_3_TX          ISR_ASSIGN(ISR_PRIORITY_ASC_3_TX, ISR_PROVIDER_ASC_3)               /**< \brief Install the ASCLIN1 transmit interrupt handler.  */
#define INTERRUPT_ASC_3_EX          ISR_ASSIGN(ISR_PRIORITY_ASC_3_EX, ISR_PROVIDER_ASC_3)               /**< \brief Install the ASCLIN1 error interrupt handler.  */

#define INTERRUPT_STM_STM0SR0       ISR_ASSIGN(ISR_PRIORITY_STM_INT0, ISR_PROVIDER_STM_INT0)            /**< \brief Define the System Timer interrupt priority.  */

#define INTERRUPT_TIMER             ISR_ASSIGN(ISR_PRIORITY_TIMER, ISR_PROVIDER_TIMER)                  /**< \brief Define the 1ms timer interrupt priority.  */

#define INTERRUPT_INCRINC_ZERO      ISR_ASSIGN(ISR_PRIORITY_INCRENC_ZERO, ISR_PROVIDER_INCRENC_ZERO)    /**< \brief Define the  Incremental encoder zero interrupt priority.  */
/** \} */

/** \} */
//------------------------------------------------------------------------------

#endif
