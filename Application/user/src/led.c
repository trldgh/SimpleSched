#include "stm32f10x.h"
#include "cm_include.h"

/* ===================== 示例初始化任务 ===================== */
static uint32_t led_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    return 0;
}
REGISTER_INIT_TASK(led_init, INIT_PRIO_DRIVER, 1);


static void LED1_Task(void)
{
	static uint8_t state = 0;
	if(state == 0)
	{
		state = 1;
		GPIO_ResetBits(GPIOA, GPIO_Pin_0);
	}
	else if(state == 1)
	{
		state = 0;
		GPIO_SetBits(GPIOA, GPIO_Pin_0);
	}
}
REGISTER_PERIODIC_TASK(LED1_Task, 1000, 1);



static void LED2_Task(void)
{
	static uint8_t state = 0;
	if(state == 0)
	{
		state = 1;
		GPIO_ResetBits(GPIOA, GPIO_Pin_2);
	}
	else if(state == 1)
	{
		state = 0;
		GPIO_SetBits(GPIOA, GPIO_Pin_2);
	}
}
REGISTER_PERIODIC_TASK(LED2_Task, 500, 1);
