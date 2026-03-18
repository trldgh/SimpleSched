ENTRY(Reset_Handler)

/* 内存布局 */
MEMORY
{
FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 64K
RAM  (rwx)  : ORIGIN = 0x20000000, LENGTH = 20K
}

/* 栈顶 */
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* 段布局 */
SECTIONS
{

/* 中断向量表 */
.isr_vector :
{
    KEEP(*(.isr_vector))
} > FLASH


/* 代码 */
.text :
{
    *(.text*)
    *(.rodata*)
    *(.glue_7)
    *(.glue_7t)
    *(.eh_frame)
} > FLASH

/* ================== init task section ================== */
.init_task :
{
    _init_task_start = .;
    KEEP(*(.init_task))
    _init_task_end = .;
} >FLASH


/* ================== periodic task section ================== */
.periodic_task :
{
    _periodic_task_start = .;
    KEEP(*(.periodic_task))
    _periodic_task_end = .;
} >FLASH

_sidata = LOADADDR(.data);

/* 已初始化变量 */
.data :
{
    _sdata = .;
    *(.data*)
    _edata = .;
} > RAM AT > FLASH


/* 未初始化变量 */
.bss :
{
    _sbss = .;
    *(.bss*)
    *(COMMON)
    _ebss = .;
} > RAM


/* stack */
._user_heap_stack :
{
    . = ALIGN(8);
    . += 0x400;   /* stack */
} > RAM

}