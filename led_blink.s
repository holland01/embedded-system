
led_blink:     file format elf32-littlearm


Disassembly of section .text:

00000000 <vector>:
   0:   fc 0f 00 10 c1 00 00 00 7d 01 00 00 7d 01 00 00     ........}...}...
  10:   7d 01 00 00 7d 01 00 00 7d 01 00 00 d2 e7 ff ef     }...}...}.......
  20:   7d 01 00 00 7d 01 00 00 7d 01 00 00 7d 01 00 00     }...}...}...}...
  30:   7d 01 00 00 7d 01 00 00 7d 01 00 00 7d 01 00 00     }...}...}...}...
  40:   7d 01 00 00 7d 01 00 00 7d 01 00 00 7d 01 00 00     }...}...}...}...
  50:   7d 01 00 00 7d 01 00 00 7d 01 00 00 7d 01 00 00     }...}...}...}...
  60:   7d 01 00 00 7d 01 00 00 7d 01 00 00 7d 01 00 00     }...}...}...}...
  70:   7d 01 00 00 7d 01 00 00 7d 01 00 00 7d 01 00 00     }...}...}...}...
  80:   7d 01 00 00 7d 01 00 00 7d 01 00 00 7d 01 00 00     }...}...}...}...
  90:   7d 01 00 00 7d 01 00 00 7d 01 00 00 7d 01 00 00     }...}...}...}...
  a0:   7d 01 00 00 7d 01 00 00 7d 01 00 00 7d 01 00 00     }...}...}...}...
  b0:   7d 01 00 00 7d 01 00 00 7d 01 00 00 7d 01 00 00     }...}...}...}...

 16c:   000f4240        .word   0x000f4240 ;
 170:   40048000        .word   0x40048000 ; system control
 174:   4003c010        .word   0x4003c010 ; flash controller
 178:   50010000        .word   0x50010000 ; GPIO port 1

;;; psuedo code
;;; #define CLOCK_SRC_IRC_OSCILLATOR 0
;;; word_t* SYSCONTROL = 0x40048000;
;;; offsets:
;;; word_t SYS_PLL_CLOCK_SRC_SEL = 0x40 >> 2;
;;; word_t SYS_PLL_CTRL = 0x8 >> 2;
;;; 
000000c0 <Reset_handler>:
  c0:   b580            push    {r7, lr}
  c2:   b082            sub     sp, #8
  c4:   af00            add     r7, sp, #0

  c6:   4b29            ldr     r3, [pc, #164]  ; reads in 0x000f4240 (from .data I guess) (16c <Reset_handler+0xac>) 
  c8:   603b            str     r3, [r7, #0]    ; store 0x000f4240 on the stack

;;; =====
;;; system control block
;;; =====
    
;;; set IRC oscillator using system PLL
;;; clock source select register:
;;; *(SYSCONTROL + SYS_PLL_CLOCK_SRC_SEL) = CSS_IRC_OSCILLATOR  
  ca:   4b29            ldr     r3, [pc, #164]  ; reads in 0x40048000 (this is a pointer)  (170 <Reset_handler+0xb0>)
  cc:   2200            movs    r2, #0 
  ce:   641a            str     r2, [r3, #64]   ; write 0 to *(0x40048000 + 64). This is a word write

;;;  set feedback divider value of System PLL control register to
;;;  4 [4:0],
;;;  and set post divider ratio of system pll control register to
;;;  2 [6:5]
  d0:   4b27            ldr     r3, [pc, #156]  ; reads in 0x40048000 (this is a pointer (170 <Reset_handler+0xb0>)
  d2:   2223            movs    r2, #35 ; 0x23
  d4:   609a            str     r2, [r3, #8]    ; write 0x23 to *(0x40048000 + 8)

;;; get sys control power down register, throw in r3
  d6:   4a26            ldr     r2, [pc, #152]  ; reads in 0x40048000 (this is a pointer)  (170 <Reset_handler+0xb0>)
  d8:   238e            movs    r3, #142        ; r3 = 142 
  da:   009b            lsls    r3, r3, #2      ; r3 <<= 2 (= 0x238 = 568)
  dc:   58d3            ldr     r3, [r2, r3]    ;
;;; 
;;; BIC: performs an AND operation on the BITS in r3
;;; with the logical negation of the corresponding bits in the value of R2
;;; so...I think this means
;;; r3 = r3 & (~r2)
;;; which means that
;;; the power down register's bit 7
;;; is set to 0, while the rest is preserved.
;;; bit 7 is SYPLL_PD, and a value of 0 means "powered".
;;; store the register value in r2.
;;; in a nutshell, the PLL is turned on here.
  de:   4924            ldr     r1, [pc, #144]  ; (170 <Reset_handler+0xb0>)
  e0:   2280            movs    r2, #128        ; 0x80
  e2:   4393            bics    r3, r2
  e4:   001a            movs    r2, r3

;;; write back new sys control power down register value
  e6:   238e            movs    r3, #142        ; 0x8e
  e8:   009b            lsls    r3, r3, #2
  ea:   50ca            str     r2, [r1, r3]
;;; enable system pll clock source updating 
  ec:   4b20            ldr     r3, [pc, #128]  ; (170 <Reset_handler+0xb0>)
  ee:   2201            movs    r2, #1
  f0:   645a            str     r2, [r3, #68]   ; 0x44

;;; ======
;;; flash controller
;;; ======


;;; fetch flash control block configuration
;;; store into r2
  f2:   4b20            ldr     r3, [pc, #128]  ; (174 <Reset_handler+0xb4>)
  f4:   681a            ldr     r2, [r3, #0]
  f6:   4b1f            ldr     r3, [pc, #124]  ; (174 <Reset_handler+0xb4>)
;;; bits 0:1 represent the flash memory access time.
;;; this code block sets the access time to 2 system clocks.
;;; the 1 which is set at bit 4 (17 is only two bits, the 1st and 4th bit)
;;; I'm not sure about. It's weird, because according to the data sheet
;;; this bit should never explicitly be written to, as it falls into the
;;; reserved category.
  f8:   2111            movs    r1, #17
  fa:   430a            orrs    r2, r1
  fc:   601a            str     r2, [r3, #0]
  fe:   46c0            nop                     ; (mov r8, r8)
;;; ---
;;; BEGIN PLL check
;;; ---
;;; main gist of the following is to ensure that
;;; the phase lock loop is actually locked.
;;; if it isn't, then fall back to an infinite loop
    
    ;; store the system phase lock loop status in r3
 100:   4b1b            ldr     r3, [pc, #108]  ; (170 <Reset_handler+0xb0>)
 102:   68db            ldr     r3, [r3, #12]

 104:   2b00            cmp     r3, #0
 106:   d0fb            beq.n   100 <Reset_handler+0x40>
;;; ----
;;; END PLL CHECK
;;; ----
;;; set the main system clock to
;;; the output from the system phase locked loop
 108:   4b19            ldr     r3, [pc, #100]  ; (170 <Reset_handler+0xb0>)
 10a:   2203            movs    r2, #3
 10c:   671a            str     r2, [r3, #112]  ; 0x70
;;; update the clock src, since we've just changed
;;; the source we wish for it to receive input from.
;;; odd: data sheet (3.5.12) says that a zero should be written
;;; to the register first, before a one is.
 10e:   4b18            ldr     r3, [pc, #96]   ; (170 <Reset_handler+0xb0>)
 110:   2201            movs    r2, #1
 112:   675a            str     r2, [r3, #116]  ; 0x74
;;; ========
;;; GPIO1
;;; ========
;;; fetch data directionr reg from GPIO1 
 114:   4a18            ldr     r2, [pc, #96]   ; GPIO1 base address (178 <Reset_handler+0xb8>)
 116:   2380            movs    r3, #128        ; 0x80
 118:   021b            lsls    r3, r3, #8      ; 128 x 2^8 = 0x8000 <--- data direction register
 11a:   58d3            ldr     r3, [r2, r3]    ; read word at (0x50010000 + 0x8000)


;;; load GPIO1 into r1
;;; set pin 9 as an output
;;;
;;; *(0x50018000) = *(0x50018000) | (128 << 2)
;;; 
;;;
;;; 
 11c:   4916            ldr     r1, [pc, #88]   ; (178 <Reset_handler+0xb8>)
 11e:   2280            movs    r2, #128        ; 0x80
 120:   0092            lsls    r2, r2, #2      ; 128 x 4 = 512 (bit 9)
 122:   431a            orrs    r2, r3          ; or word read into r3 with bit 9
 124:   2380            movs    r3, #128        ; 0x80
 126:   021b            lsls    r3, r3, #8      ; fetch GPIO1 data dir offset
 128:   50ca            str     r2, [r1, r3]    ; write back

;;; 
;;; 
;;; zero out second local
;;;      
 12a:   2300            movs    r3, #0
 12c:   607b            str     r3, [r7, #4]
 12e:   e002            b.n     136 <Reset_handler+0x76> ; enter loop?

;;; loop entry
;;; (iterates 1_000_000 times - throughout this period, current is traveling through
;;; to pin 9
;;;)
;;; 
 130:   687b            ldr     r3, [r7, #4] ; 
 132:   3301            adds    r3, #1       ; increment loop counter
 134:   607b            str     r3, [r7, #4] ; write back
;;; from b.n above
 136:   687a            ldr     r2, [r7, #4] ; should be 0 when loop first entered
 138:   683b            ldr     r3, [r7, #0] ; should be 0x000f4240 when loop first entered
    
;;; continue looping while r2 < r3?
;;; cmp r2, r3 -> r2 - r3
;;; carry flag is set to 0 if
;;; subtraction produces unsigned underflow
;;; http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0801b/CIADCDHH.html
;;; 
 13a:   429a            cmp     r2, r3 
 13c:   d3f8            bcc.n   130 <Reset_handler+0x70>
;;; loop end
;;;
    
;;; GPIO1 base address
 13e:   4a0e            ldr     r2, [pc, #56]   ; (178 <Reset_handler+0xb8>)
 140:   2380            movs    r3, #128        ; 0x80
 142:   011b            lsls    r3, r3, #4      ; 128 << 4 = 2048 (bit 11)

;;;  *((unsigned char*)GPIO1 + 2048) = 0, or data[1 << 9] = 0 (since data is unsigned int)
 144:   2100            movs    r1, #0
 146:   50d1            str     r1, [r2, r3]
;;; reset loop counter
 148:   2300            movs    r3, #0
 14a:   607b            str     r3, [r7, #4]
 14c:   e002            b.n     154 <Reset_handler+0x94>

;;; loop begin (same loop as before)
;;; total is approximately 10 cycles
;;; 5 cycles here
 14e:   687b            ldr     r3, [r7, #4] ; 2 cycles
 150:   3301            adds    r3, #1       ; 1 cycle
 152:   607b            str     r3, [r7, #4] ; 2 cycles

;;; branched here from 14c - loop entry
;;; pipelined memory access, so these two loads
;;; should take 3 cycles
 154:   687a            ldr     r2, [r7, #4] ; read loop counter (will be 0 on first branch)
 156:   683b            ldr     r3, [r7, #0] ; read in max count (will be 1_000_000 on first branch)

;;;
;;; 2 cycles here
 158:   429a            cmp     r2, r3 ; 1 cycle
 15a:   d3f8            bcc.n   14e <Reset_handler+0x8e> ; 1 cycle
;;; loop end
;;;
;;; 
 15c:   4906            ldr     r1, [pc, #24]   ; (178 <Reset_handler+0xb8>)
 15e:   2380            movs    r3, #128        ; 0x80
 160:   011b            lsls    r3, r3, #4      ; 128 << 4 (= bit 11) (div by 4 gives 1 << 9) 
 162:   2201            movs    r2, #1          ; 
 164:   4252            negs    r2, r2          ; r2 = 0xFFFFFFFF
 166:   50ca            str     r2, [r1, r3]    ; set gpio1 pin 9 data to all F's
;;;
;;;
;;; 
 168:   e7df            b.n     12a <Reset_handler+0x6a> ; restart sequence?
 16a:   46c0            nop                     ; (mov r8, r8)
;;;
;;;
;;; 
 16c:   000f4240        .word   0x000f4240 
 170:   40048000        .word   0x40048000 ; system control
 174:   4003c010        .word   0x4003c010 ; flash controller
 178:   50010000        .word   0x50010000 ; GPIO port 1

0000017c <Default_Handler>:
 17c:   b580            push    {r7, lr}
 17e:   af00            add     r7, sp, #0
 180:   e7fe            b.n     180 <Default_Handler+0x4>
        ...
