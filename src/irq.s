
        /* Generic interrupt handler.  This assumebly function saves the      */
        /* state of the current thread context on that thread's stack, then   */
        /* class the actual interrupt handler.  Upon return from that         */
        /* handler, which may have changed the current thread, this function  */
        /* restores the thread state and returns from the interrupt.          */
        /*
        /* This is writen in assembly because the registers are saved to a    */
        /* different stack from the one pointed to by sp.  This would be very */
        /* awkward from C.                                                    */
        .text
        .align 1
        .global __irq_generic
        .syntax unified
__irq_generic:
        push {lr}

        /* if (LR == 0xFFFFFFFD)                                              */
        mov  r0, lr
        ldr  r1, .L1+8
        cmp  r1, r0
        bne  .L10

        /* { save_thread_context onto psp                                              */
        cpsid i
        mrs  r0,  psp
        mov  r1,  sp
        mov  sp,  r0
        push {r4, r5, r6, r7}
        mov  r4,  r8
        mov  r5,  r9
        mov  r6,  r10
        mov  r7,  r11
        push {r4, r5, r6, r7}
        mov   r0,  sp
        mov   sp,  r1
        cpsie i

       /* grab CURCTX address,
          and set CURCTX->sp to psp */
        ldr  r1, .L1
        ldr  r1, [r1]
        str  r0, [r1,#4]
        /* } */

.L10:   /* __irq_handlers[psr&0x3ff]()                                        */
        mrs  r0, psr
        movs r1, 0x3F
        ands r0, r1
        lsls r0, #2
        ldr  r1, .L1+4
        adds r0, r1
        ldr  r0, [r0]
        tst  r0, r0
        beq  .L0
        
        blx  r0
        
.L0:    /* if (LR == 0xFFFFFFFD)                                              */
        ldr  r0, [sp]
        ldr  r1, .L1+8
        cmp  r1, r0
        bne  .L11

        /* { restore thread context                                           */
        ldr  r1, .L1
        ldr  r1, [r1]
        ldr  r0, [r1,#4]

        ldm  r0!, {r4,r5,r6,r7}
        mov  r8,  r4
        mov  r9,  r5
        mov  r10, r6
        mov  r11, r7
        ldm  r0!, {r4,r5,r6,r7}
        msr  psp, r0
        /* }                                                                  */
.L11:
        pop  {pc}
        .align 4
.L1:
        .word CURCTX
        .word __irq_handlers
        .word 0xFFFFFFFD

        
        /* low-level rest.  This function switch the current stack to the     */
        /* stack associated with the special thread named __main__.  For      */
        /* simplicty, __main__'s stack is pointed to by the symbol __PSP.     */
        /* bit 1 (#2) when set on the control register will set the active stack to PSP */
        /* see http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0497a/CHDBIBGJ.html */
        .align 1
        .global __reset
__reset:
        ldr  r0,  .L3
        ldr  r0,  [r0]
        msr  psp, r0
        mrs  r0,  control
        movs r1,  #2            
        orrs r0,  r1
        msr  control, r0

        bl   reset
        .align 4
.L3:
        .word __PSP             


