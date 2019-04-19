MEMORY {
       flash(rx) : org = 0x00000000, len = 32k;
       sram(rwx) : org = 0x10000000, len = 4k;      
}

SECTIONS {
         .text : {
                . = 0x00000000;
                *(.vector);
                . = ALIGN(4);
                *(.text);
                . = ALIGN(4);
          } > flash

         .rodata : {
                . = 0x00000000;
                *(.data);
                . = ALIGN(4);
         } > sram AT > flash

         .data : {
               . = 0x00000000;
               *(.data);
               . = ALIGN(4);
         } > sram AT > flash

         .bss : {
               *(.bss);
               . = ALIGN(4);
         } > sram

         __DATA_LMA = LOADADDR(.data);
         __DATA_SIZE = LOADADDR(.data) + SIZEOF(.data)
         __DATA_VMA = ADDR(.data);
         __INITIAL_SP = ORIGIN(sram) + LENGTH(sram);
         __VECTOR_CHECKSUM = 0 - __INITIAL_SP - (reset + 1) - (NMI + 1) - (hardfault + 1);
         GPIO0 = 0x50000000;
         GPIO1 = 0x50010000;
}
                
         
