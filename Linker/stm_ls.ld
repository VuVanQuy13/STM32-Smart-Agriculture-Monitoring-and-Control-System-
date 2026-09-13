ENTRY(Reset_Handler)

MEMORY
{
    FLASH(rx):ORIGIN =0x08000000, LENGTH = 62K
    CONFIG(r):ORIGIN =0x0800F800, LENGTH = 2K
    SRAM(rwx):ORIGIN =0x20000000, LENGTH = 20K
}

SECTIONS
{
    .text :  
    {      
        *(.isr_vector)
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;
    } > FLASH

    .data : 
    {
        _sdata = .;
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > SRAM AT > FLASH 
    ASSERT(LOADADDR(.data) + SIZEOF(.data) <= ORIGIN(CONFIG),
           "Firmware overlaps configuration pages")

    .bss :
    {
        _sbss = .;
        *(.bss*)
        . = ALIGN(4);
        _ebss = .;
    } > SRAM
}
