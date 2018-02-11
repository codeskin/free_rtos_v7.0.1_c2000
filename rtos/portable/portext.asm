;/*
;   FreeRTOS V7.0.1 - Copyright (C) 2011 Real Time Engineers Ltd.
;	
;   C2000 Port - Copyright (C) 2011 CodeSkin LLC - www.codeskin.com
;
;    ***************************************************************************
;    *                                                                       *
;    *    FreeRTOS tutorial books are available in pdf and paperback.        *
;    *    Complete, revised, and edited pdf reference manuals are also       *
;    *    available.                                                         *
;    *                                                                       *
;    *    Purchasing FreeRTOS documentation will not only help you, by       *
;    *    ensuring you get running as quickly as possible and with an        *
;    *    in-depth knowledge of how to use FreeRTOS, it will also help       *
;    *    the FreeRTOS project to continue with its mission of providing     *
;    *    professional grade, cross platform, de facto standard solutions    *
;    *    for microcontrollers - completely free of charge!                  *
;    *                                                                       *
;    *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
;    *                                                                       *
;    *    Thank you for using FreeRTOS, and thank you for your support!      *
;    *                                                                       *
;    ***************************************************************************
;
;
;   This file is part of the FreeRTOS distribution.
;
;   FreeRTOS is free software; you can redistribute it and/or modify it under
;   the terms of the GNU General Public License (version 2) as published by the
;   Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
;   >>>NOTE<<< The modification to the GPL is included to allow you to
;   distribute a combined work that includes FreeRTOS without being obliged to
;   provide the source code for proprietary components outside of the FreeRTOS
;   kernel.  FreeRTOS is distributed in the hope that it will be useful, but
;   WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
;   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
;   more details. You should have received a copy of the GNU General Public
;   License and the FreeRTOS license exception along with FreeRTOS; if not it
;   can be viewed here: http://www.freertos.org/a00114.html and also obtained
;   by writing to Richard Barry, contact details for whom are available on the
;   FreeRTOS WEB site.
;
;   1 tab == 4 spaces!
;
;   http://www.FreeRTOS.org - Documentation, latest information, license and
;   contact details.
;
;   http://www.SafeRTOS.com - A version that is certified for use in safety
;   critical systems.
;
;   http://www.OpenRTOS.com - Commercial support, development, porting,
;   licensing and training services.
;/
	.global _vTaskIncrementTick
	.global _vTaskSwitchContext
	.global _vPortSetupTimerInterrupt
	.global _pxCurrentTCB
	.global _usCriticalNesting
	
	.def _vPortPreemptiveTickISR
	.def _vPortCooperativeTickISR
	.def _vPortYieldTrap
	.def _vPortYield
	.def _xPortStartScheduler
;-----------------------------------------------------------

portSAVE_CONTEXT .macro
    MOVW DP,#_usCriticalNesting ; restore usCriticalNesting
    PUSH @_usCriticalNesting
	ASP ; even align stack - needed by C environment
    PUSH DP:ST1 ; so to remember SPA
    PUSH RPC
    PUSH AR1H:AR0H
    SPM 0
    PUSH XAR4
    PUSH XAR5
    PUSH XAR6
    PUSH XAR7
    PUSH XT 
   	MOV AL, @SP
	; save current SP to entry in TCB
    MOVW  DP,#_pxCurrentTCB
    MOVL  XAR4,@_pxCurrentTCB 
    MOV  *+XAR4[0],AL    	
	.endm
;-----------------------------------------------------------
		
portRESTORE_CONTEXT .macro
	; restore SP from current TCB	
	MOVW      DP,#_pxCurrentTCB
    MOVL      XAR4,@_pxCurrentTCB
    MOVL      ACC,*+XAR4[0]
	MOV @SP,AL 
	
    POP XT
    POP XAR7
    POP XAR6
    POP XAR5
    POP XAR4
    POP AR1H:AR0H
    POP RPC  
    POP DP:ST1 ; restore SPA
    NASP ; revert stack alignment that occured during context save
    MOVW DP,#_usCriticalNesting ; restore usCriticalNesting
    POP @_usCriticalNesting
    IRET
	.endm
;-----------------------------------------------------------

;*
;* The RTOS tick ISR.
;*
;* If the cooperative scheduler is in use this simply increments the tick
;* count.
;*
;* If the preemptive scheduler is in use a context switch can also occur.
;*/
	
	.text
	.align 2
	
_vPortPreemptiveTickISR: .asmfunc
	portSAVE_CONTEXT
	
    CLRC PAGE0,OVM
    CLRC AMODE	
				
	LCR #_vTaskIncrementTick
	LCR #_vTaskSwitchContext
		
	portRESTORE_CONTEXT
	.endasmfunc
		
;-----------------------------------------------------------

	.align 2
	
_vPortCooperativeTickISR: .asmfunc
	portSAVE_CONTEXT
	
	CLRC PAGE0,OVM
    CLRC AMODE
					
	LCR #_vTaskIncrementTick

	portRESTORE_CONTEXT
	.endasmfunc
		
;-----------------------------------------------------------

_vPortYieldTrap: .asmfunc
	; Save the context of the current task.
	portSAVE_CONTEXT
	
	CLRC PAGE0,OVM
    CLRC AMODE			

	; Select the next task to run.
	LCR #_vTaskSwitchContext	

	; Restore the context of the new task.
	portRESTORE_CONTEXT
	.endasmfunc

;-----------------------------------------------------------
; Manual context switch called by the portYIELD() macro.

	.align 2

_vPortYield: .asmfunc
	TRAP #31 ; drap transfers program control to ISR (takes care of pop. stack)
	         ; see "TMS320C28x CPU and Instruction Set Reference Guide" - p. 6-363
	LRETR ; makes assumption that yield was called by LCR	
	.endasmfunc
	
;-----------------------------------------------------------
;
; Start off the scheduler by initialising the RTOS tick timer, then restoring
; the context of the first task.
;

	.align 2
	
_xPortStartScheduler: .asmfunc
	; Setup the hardware to generate the tick.  Interrupts are disabled
	; when this function is called.
	LCR	#_vPortSetupTimerInterrupt
	; Restore the context of the first task that is going to run.
	portRESTORE_CONTEXT
	.endasmfunc
;-----------------------------------------------------------
      		
	.end