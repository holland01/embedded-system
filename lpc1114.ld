MEMORY {
       flash(rx) : org = 0x00000000, len = 32k
       sram(rwx) : org = 0x10000000, len = 4k      
}

ENTRY(vector);

SECTIONS {
       . = 0x00000000;
       .text : {
              *(.vector);
              . = 0x000000c0;
              . = ALIGN(4);
              *(.text);
              . = ALIGN(4);
       } > flash

       .rodata : {
              . = ALIGN(4);
              *(.rodata);
       } > flash

       .data : {
             . = 0x00000000;
             . = ALIGN(4);
             *(.data);
       } > sram AT > flash

       .bss : {
             . = ALIGN(4);
             *(.bss);
       } > sram

       __DATA_LMA = LOADADDR(.data);
       __DATA_END = LOADADDR(.data) + SIZEOF(.data);
       __DATA_VMA = ADDR(.data);

       __BSS_VMA = ADDR(.bss);
       __BSS_END = ADDR(.bss) + SIZEOF(.bss);
}

__INITIAL_SP = ORIGIN(sram) + LENGTH(sram);
__VECTOR_CHECKSUM = 0 - __INITIAL_SP - (reset + 1) - (NMI + 1) - (hardfault + 1);

ISER = 0xE000E100;
SYSCON = 0x40048000;
GPIO0 = 0x50000000;
GPIO1 = 0x50010000;
IOCON_PIO_8 = 0x40044060;
TMR16B0 = 0x4000C000;
IOCON_R_PIO_11 = 0x40044074;
