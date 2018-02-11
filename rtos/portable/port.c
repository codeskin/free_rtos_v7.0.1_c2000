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

/*
 * Initialise the stack of a task to look exactly as if a call to
 * portSAVE_CONTEXT had been called.
 *
 * See the header file portable.h.
 */
#include "includes.h" 

/*
	If you perform a task-switch operation (stack changes), the RPC register must
	be manually saved. You are not to save the RPC register if the stack is not
	changed.
	
	
	When a call operation is performed using the LCR instruction, the return address
	is saved in the RPC register and the old value in the RPC is saved on
	the stack (in two 16-bit operations). When a return operation is performed using
	the LRETR instruction, the return address is read from the RPC register
	and the value on the stack is written into the RPC register (in two 16-bit operations).
	Other call instructions do not use the RPC register. For more information,
	see the instructions in Chapter 6.
	
	Return program counter. The RPC register is new on the C28x.
	When a call operation is performed, the return address is saved in
	the RPC register and the old value in the RPC is saved on the stack.
	When a return operation is performed, the return address is read
	from the RPC register and the value on the stack is written into the
	RPC register. The net result is that return operations are faster (4
	instead of 8 cycles). This register is only used when certain call and
	return instructions are used. Normal call and return instructions bypass
	this register.
*/

/* We require the address of the pxCurrentTCB variable, but don't want to know
any details of its type. */
typedef void tskTCB;
extern volatile tskTCB * volatile pxCurrentTCB;

#define portINITIAL_CRITICAL_NESTING	( ( unsigned short ) 10 )
volatile unsigned short usCriticalNesting = portINITIAL_CRITICAL_NESTING;
 
portSTACK_TYPE *pxPortInitialiseStack( portSTACK_TYPE *pxTopOfStack, pdTASK_CODE pxCode, void *pvParameters ){

    /* FIRST we need to emulate automatic context save assumed by the IRET function */
    /* ============================================================================ */

	/* see "TMS320C28x CPU and Instruction Set Reference Guide", page 3-14
	 * on how stack is populated by automatic context save */
	
	// we even-align stack (details of automatic context save depend on alignment
	if(((unsigned long)pxTopOfStack & 1) != 0){
		pxTopOfStack++;
	} 
	
	// ST0: for C runtime environment, it is presumed that OVM=0, PM=0
	*pxTopOfStack = 0x0000; 	
	pxTopOfStack++;
	// T - upper 16-bit of Multiplicand Register, nothing presumed
	*pxTopOfStack = ( portSTACK_TYPE ) 0x0000; 
	pxTopOfStack++;
	// AL - Accumulator, nothing presumed
	*pxTopOfStack = ( portSTACK_TYPE ) 0x0000; 
	pxTopOfStack++;
	// AH - Accumulator, nothing presumed
	*pxTopOfStack = ( portSTACK_TYPE ) 0x0000; 
	pxTopOfStack++;
	// PL - Product Register, nothing presumed
	*pxTopOfStack = ( portSTACK_TYPE ) 0x0000; 
	pxTopOfStack++;
	// PH - Product Register, nothing presumed
	*pxTopOfStack = ( portSTACK_TYPE ) 0x0000; 
	pxTopOfStack++;
	// AR0 - Aux Register 0, nothing presumed
	*pxTopOfStack = ( portSTACK_TYPE ) 0x0000; 
	pxTopOfStack++;
	// AR1 - Aux Register 0, nothing presumed
	*pxTopOfStack = ( portSTACK_TYPE ) 0x0000; 
	pxTopOfStack++;		
		
	// ST1: ARP(15-13) XF(12) M0M1MAP(11) Reserved(10) OBJMODE(9) AMODE(8) IDLESTAT(7)
	//      EALLOW(6) LOOP(5) SPA(4) VMAP(3) PAGE0(2) DBGM(1) INTM(0)
	//      presumed is PAGE0=0	
	// 1: M0 and M1 mapping mode bit. The M0M1MAP bit should always remain set to 1 in the C28x object mode.
	// 0: Reserved
	// 1: C28x object mode (OBJMODE == 1)
	// 0: AMODE This bit is set to 0 on reset.
	// 0: IDLESTAT is not restored from the stack.
	// 0: When the CPU services an interrupt, the current value of EALLOW is saved on the
    //   stack (when ST1 is saved on the stack), and then EALLOW is cleared. Therefore, at the
    //   start of an interrupt service routine (ISR), access to protected registers is disabled. If the
    //   ISR must access protected registers, it must include an EALLOW instruction. At the end
    //   of the ISR, EALLOW can be restored by the IRET instruction.
    // 0: Upon return from the interrupt, LOOP is not restored from the stack.
    // 0: SPA = 0 The stack pointer has not been aligned to an even address.
    // 1: VMAP = For normal operation leave this bit set.
    // 0: PAGE0 = presumed at 0 by C compiler
    // 0: DBGM: 0 = Debug events are enabled.
    // 0: INTM: 0 Maskable interrupts are globally enabled. 
    // ST1 B: 0000 1010 0000 1000
	*pxTopOfStack = ( portSTACK_TYPE ) 0x8A08; 
	pxTopOfStack++;
	// DP - no assumptions made
	*pxTopOfStack = ( portSTACK_TYPE ) 0x0000; 
	pxTopOfStack++;	
	// IER - interrupt enable register - probably needs to be passed as a parameter!
	*pxTopOfStack = ( portSTACK_TYPE ) configTICK_IER_MASK; 
	pxTopOfStack++;
	// DBGSTAT, emulation info - NOT SURE
	*pxTopOfStack = ( portSTACK_TYPE ) 0x0000; 
	pxTopOfStack++;
	// PCL
	*pxTopOfStack = ( portSTACK_TYPE ) ((unsigned long)pxCode & 0xFFFF); 
	pxTopOfStack++;
	// PCH
	*pxTopOfStack = ( portSTACK_TYPE ) (((unsigned long)pxCode >> 16) & 0xFFFF); 
	pxTopOfStack++;
	// unused location (per automatic context save)
	*pxTopOfStack = ( portSTACK_TYPE )0xbea1;
	pxTopOfStack++;		
	
	/* SECOND, we need to emulate portSAVE_CONTEXT in portext.asm */
	/* ========================================================== */	
	/* A variable is used to keep track of the critical section nesting.
	This variable has to be stored as part of the task context and is
	initially set to zero. */
	*pxTopOfStack = ( portSTACK_TYPE ) portNO_CRITICAL_SECTION_NESTING;	
	pxTopOfStack++;	

	/* alignment of stack (required by C environment) */
	if(((unsigned long)pxTopOfStack & 1) == 0){
		// stack already aligned, save SPA = 0
		*pxTopOfStack = ( portSTACK_TYPE ) 0x8A08; // ST1 B: 0000 1010 0000 1000
		pxTopOfStack++;
	} else {
		// align stack, save SPA = 1
		pxTopOfStack++;		
		*pxTopOfStack = ( portSTACK_TYPE ) 0x8A18; // ST1 B: 0000 1010 0001 1000
		pxTopOfStack++;		
	}
	// now DP again, for emulating complete PUSH DP:ST1
	*pxTopOfStack = ( portSTACK_TYPE ) 0x0000; // DP - no assumptions made
	pxTopOfStack++;	
	
	// RPC needs to be saved when stack switching (value does not matter initially)
	*pxTopOfStack = ( portSTACK_TYPE ) 0x0000;  // RPC L
	pxTopOfStack++;		
	*pxTopOfStack = ( portSTACK_TYPE ) 0x0000;  // RPC H
	pxTopOfStack++;	
	// PUSH AR1H:AR0H
	*pxTopOfStack = ( portSTACK_TYPE ) 0x0000;  
	pxTopOfStack++;		
	*pxTopOfStack = ( portSTACK_TYPE ) 0x0000; 
	pxTopOfStack++;	
	// PUSH *SP++,XAR4
	*pxTopOfStack = ( portSTACK_TYPE ) 0x0000;  
	pxTopOfStack++;				
	*pxTopOfStack = ( portSTACK_TYPE ) 0x0000; 
	pxTopOfStack++;	
	// PUSH *SP++,XAR5 
	*pxTopOfStack = ( portSTACK_TYPE ) 0x0000;  
	pxTopOfStack++;		
	*pxTopOfStack = ( portSTACK_TYPE ) 0x0000; 
	pxTopOfStack++;	
	 // PUSH *SP++,XAR6 
	*pxTopOfStack = ( portSTACK_TYPE ) 0x0000; 
	pxTopOfStack++;		
	*pxTopOfStack = ( portSTACK_TYPE ) 0x0000; 
	pxTopOfStack++;	
	 //PUSH *SP++,XAR7 
	*pxTopOfStack = ( portSTACK_TYPE ) 0x0000; 
	pxTopOfStack++;	
    *pxTopOfStack = ( portSTACK_TYPE ) 0x0000; 
	pxTopOfStack++;	
	// PUSH XT	
	*pxTopOfStack = ( portSTACK_TYPE ) 0x0000;  
	pxTopOfStack++;	
    *pxTopOfStack = ( portSTACK_TYPE ) 0x0000; 
	pxTopOfStack++;	
		
	/* Return a pointer to the top of the stack we have generated so this can
	be stored in the task control block for the task. */
	return pxTopOfStack;
}

void vPortEndScheduler( void ){
	/* It is unlikely that this port will get stopped.  If required simply
	disable the tick interrupt here. */
	portDISABLE_INTERRUPTS();
}

/*
 * Hardware initialisation to generate the RTOS tick.
 */
extern void vApplicationSetupTimerInterrupt(void);
void vPortSetupTimerInterrupt( void ){
	vApplicationSetupTimerInterrupt();
}

