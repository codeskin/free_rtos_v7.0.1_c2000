/*
    FreeRTOS V7.0.1 - Copyright (C) 2011 Real Time Engineers Ltd.

	C2000 Port - Copyright (C) 2011 CodeSkin LLC - www.codeskin.com

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS tutorial books are available in pdf and paperback.        *
     *    Complete, revised, and edited pdf reference manuals are also       *
     *    available.                                                         *
     *                                                                       *
     *    Purchasing FreeRTOS documentation will not only help you, by       *
     *    ensuring you get running as quickly as possible and with an        *
     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
     *    the FreeRTOS project to continue with its mission of providing     *
     *    professional grade, cross platform, de facto standard solutions    *
     *    for microcontrollers - completely free of charge!                  *
     *                                                                       *
     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
     *                                                                       *
     *    Thank you for using FreeRTOS, and thank you for your support!      *
     *                                                                       *
    ***************************************************************************


    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    >>>NOTE<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes FreeRTOS without being obliged to
    provide the source code for proprietary components outside of the FreeRTOS
    kernel.  FreeRTOS is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/

#ifndef PORTMACRO_H_
#define PORTMACRO_H_

/* we design for large memory model, where pointers are 22 bit */
#ifndef __LARGE_MODEL__
#error Wrong memory model!
#endif

/* Type definitions. */
#define portCHAR		char
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		long
#define portSHORT		int
#define portBASE_TYPE	portSHORT

#define portSTACK_TYPE unsigned int

#if( configUSE_16_BIT_TICKS == 1 )
	typedef unsigned portSHORT portTickType;
	#define portMAX_DELAY ( portTickType ) 0xffff
#else
	typedef unsigned portLONG portTickType;
	#define portMAX_DELAY ( portTickType ) 0xffffffff
#endif

#define portDISABLE_INTERRUPTS() asm(" setc INTM")	
#define portENABLE_INTERRUPTS()  asm(" clrc INTM")

/* Hardware specifics. */

/* Note the following about the stack:
		* Stack is 16-bit wide
		* The stack grows from low memory to high memory.
		* When 32-bit values are saved to the stack, the least significant 16 bits are
          saved first, and the most significant 16 bits are saved to the next higher
          address (little endian format).
*/
#define portBYTE_ALIGNMENT			( 2 )  // we even-align stack
#define portSTACK_GROWTH			( 1 )
#define portTICK_RATE_MS			( ( portTickType ) 1000 / configTICK_RATE_HZ )	
#define portNOP()					asm(" nop")	

#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )

/*
 * Manual context switch called by portYIELD or taskYIELD.
 */
extern void vPortYield( void );

// not directly used by RTOS, but RTOS uses 
// portYIELD_WITHIN_API & taskYIELD
// which point to portYIELD()
#define portYIELD() vPortYield()

// not used from within RTOS
#define portYIELD_FROM_ISR( x ) if( x ) vPortYield()

/* Critical section control macros. */
#define portNO_CRITICAL_SECTION_NESTING		( ( unsigned portSHORT ) 0 )

#define portENTER_CRITICAL()													\
{																				\
extern volatile unsigned short usCriticalNesting;								\
																				\
	portDISABLE_INTERRUPTS();													\
																				\
	/* Now interrupts are disabled usCriticalNesting can be accessed */			\
	/* directly.  Increment ulCriticalNesting to keep a count of how many */	\
	/* times portENTER_CRITICAL() has been called. */							\
	usCriticalNesting++;														\
}

#define portEXIT_CRITICAL()														\
{																				\
extern volatile unsigned short usCriticalNesting;								\
																				\
	if( usCriticalNesting > portNO_CRITICAL_SECTION_NESTING )					\
	{																			\
		/* Decrement the nesting count as we are leaving a critical section. */	\
		usCriticalNesting--;													\
																				\
		/* If the nesting level has reached zero then interrupts should be */	\
		/* re-enabled. */														\
		if( usCriticalNesting == portNO_CRITICAL_SECTION_NESTING )				\
		{																		\
			portENABLE_INTERRUPTS();											\
		}																		\
	}																			\
}

#endif /*PORTMACRO_H_*/
