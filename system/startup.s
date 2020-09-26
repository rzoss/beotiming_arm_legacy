/**
 *******************************************************************************
 * \file    startup.s
 *******************************************************************************
 * \brief   Startupcode
 			- interrupt vectors
 			- call AT91F_LowLevelInit()
 			- initialize stacks
 			- call main()
 *
 * \version	1.0
 * \date	27.11.06
 * \author	M.Muehlemann
 *
 *
 *******************************************************************************
 */
.global UND_STACK_SIZE
.global ABT_STACK_SIZE
.global FIQ_STACK_SIZE
.global IRQ_STACK_SIZE
.global SVC_STACK_SIZE
.global USR_STACK_SIZE
.set  UND_STACK_SIZE, 0x00000200	/* stack for "undefined instruction"	  */
.set  ABT_STACK_SIZE, 0x00000200	/* stack for "abort" interrupts			  */
.set  FIQ_STACK_SIZE, 0x00000200	/* stack for "FIQ" interrupts			  */
.set  IRQ_STACK_SIZE, 0x00000200	/* stack for "IRQ" normal interrupts	  */
.set  SVC_STACK_SIZE, 0x00000200	/* stack for "SVC" supervisor mode		  */
.set  USR_STACK_SIZE, 0x00001000	/* stack for "SVC" supervisor mode		  */

/* Standard definitions of Mode bits and Interrupt (I & F) flags (ion CPSR)	***/
.set  MODE_USR, 0x10				/* Normal User Mode						  */
.set  MODE_FIQ, 0x11				/* FIQ Processing Fast Interrupts Mode	  */
.set  MODE_IRQ, 0x12				/* IRQ Processing Standard Interrupts Mode*/
.set  MODE_SVC, 0x13				/* Software Interrupts Mode				  */
.set  MODE_ABT, 0x17				/* Abort Processing memory Faults Mode	  */
.set  MODE_UND, 0x1B				/* Undefined Instructions Mode			  */
.set  MODE_SYS, 0x1F 				/* System Priviledged Operating Mode	  */

.set  I_BIT, 0x80					/* when I bit is set, IRQ is disabled	  */
.set  F_BIT, 0x40					/* when F bit is set, FIQ is disabled	  */


/*
;------------------------------------------------------------------------------
;- Area Definition
;------------------------------------------------------------------------------
*/

.text
.arm								/* generate ARM-Code					  */

.global _startup
.section .startup
_startup:

/*;------------------------------------------------------------------------------
;- Exception vectors
;--------------------
;- These vectors can be read at address 0 or at RAM address
;- They ABSOLUTELY requires to be in relative addresssing mode in order to
;- guarantee a valid jump. For the moment, all are just looping.
;- If an exception occurs before remap, this would result in an infinite loop.
;- To ensure if a exeption occurs before start application to infinite loop.
;------------------------------------------------------------------------------
*/
	B			InitReset						/* 0x00 Reset handler			*/
undefvec:
	B			AT91F_Default_UNDEF_handler		/* 0x04 Undefined Instruction	*/
swivec:
	B			AT91F_Default_SWI_handler		/* 0x08 Software Interrupt		*/
pabtvec:
	B			AT91F_Default_Abort_handler		/* 0x0C Prefetch Abort			*/
dabtvec:
	B			AT91F_Default_Abort_handler		/* 0x10 Data Abort				*/
rsvdvec:
	B			rsvdvec							/* 0x14 reserved				*/
irqvec:
	ldr			pc,[pc,#-0xf20]					/* 0x18 IRQ						*/
fiqvec:
    ldr			pc,[pc,#-0xf20]					/* 0x1c FIQ						*/
	
InitReset:
/*
;------------------------------------------------------------------------------
;- Low level Init (PMC, AIC, ? ....) by C function AT91F_LowLevelInit
;------------------------------------------------------------------------------
;- minumum C initialization
;- call  AT91F_LowLevelInit( void)
;------------------------------------------------------------------------------
*/

    ldr       r13, =_und_stack_top_address	/* temporary stack in internal RAM*/
    ldr       r0, =AT91F_LowLevelInit		/* load address 				  */
    mov       lr, pc
    bx        r0							/* call  AT91F_LowLevelInit		  */		

/*
;------------------------------------------------------------------------------
;- Setup the stack for each mode
;-------------------------------
*/
	ldr r0, =_und_stack_top_address
	/* Undefined Instruction Mode 											  */
	msr CPSR_c, #MODE_UND|I_BIT|F_BIT 	/* switch in UndefinedInstruction Mode*/
	mov sp, r0
	sub r0, r0, #UND_STACK_SIZE
	
	/* Abort Mode 															  */
	msr CPSR_c, #MODE_ABT|I_BIT|F_BIT 	/* switch in Abort Mode				  */
	mov sp, r0
	sub r0, r0, #ABT_STACK_SIZE

	/* FIQ Mode 															  */	
	msr CPSR_c, #MODE_FIQ|I_BIT|F_BIT 	/* switch in FIQ Mode				  */
	mov sp, r0	
	sub r0, r0, #FIQ_STACK_SIZE
	
	/* IRQ Mode 															  */	
	msr CPSR_c, #MODE_IRQ|I_BIT|F_BIT 	/* switch in IRQ Mode				  */
	mov sp, r0
	sub r0, r0, #IRQ_STACK_SIZE
	
	/* switch in Supervisor Mode											  */
	msr CPSR_c, #MODE_SVC|I_BIT|F_BIT 	/* switch in Supervisor Mode		  */
	mov sp, r0
	sub r0, r0, #SVC_STACK_SIZE
	
	/* switch in User Mode													  */
	msr CPSR_c, #MODE_SYS|I_BIT|F_BIT 	/* switch in User Mode				  */
	mov sp, r0


/* copy .data section initialize ininialized variables	***********************/
    ldr     R1, =_etext
    ldr     R2, =_data
    ldr     R3, =_bss_start
cpydata_loop:
  	cmp     R2, R3
    ldrlo   R0, [R1], #4
	strlo   R0, [R2], #4
	blo     cpydata_loop

/* Clear .bss section (uninitialized data, stack) *****************************/
    mov     R0, #0
    ldr     R1, =_bss_start
    ldr     R2, =_bss_end
zerobss_loop:
	cmp     R1, R2
    strlo   R0, [R1], #4
    blo     zerobss_loop

    b main
.end

