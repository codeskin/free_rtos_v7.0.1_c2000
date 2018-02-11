C2000 FreeRTOS
==============

   FreeRTOS V7.0.1 - Copyright (C) 2011 Real Time Engineers Ltd.
   
   C2000 Port - Copyright (C) 2011 CodeSkin LLC - www.codeskin.com

   Please see "copyright.txt: for further copyright and licensing information.
   
This folder contains the portable support for TI C2000 MCU that are based on 
the 280 core.

IMPORTANT: 

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE.

Also note that, while this port has been tested, it has not been formally 
validated. 

Implementation Basics:
----------------------
1) the application needs to implement vApplicationSetupTimerInterrupt() which
   is responsible for setting-up a periodical interrupt
2) the interrupt  vector needs to point to either vPortPreemptiveTickISR() or
   vPortCooperativeTickISR(), depending on the desired scheduling type
3) vApplicationTickHook() should be used to service timer/interrupt flags  
4) TRAP #31 must point to vPortYieldTrap()

Known Limitations:
------------------
1) only large memory model supported
2) critical sections disable all interrupts
3) hardware floating point unit not supported (as FPU registers are not saved)

Todo:
-----
1) floating point target needs additional registers saved
2) nuanced interrupt disabling needs to be implemented
3) test portYIELD_FROM_ISR( x )

References:
-----------
1) "TMS320C28x CPU and Instruction Set Reference Guide" - 
   Literature Number: SPRU430E August 2001 - Revised January 2009

2) "TMS320C28x Optimizing C/C++ Compiler v6.0" - Literature Number: 
   SPRU514D - May 2011
