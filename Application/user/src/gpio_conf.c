#include <stddef.h>
#include "cm_include.h"
#include "user_include.h"


typedef struct
{
    GPIO_TypeDef* port;
    uint16_t      pin;
    GPIOMode_TypeDef mode;
    GPIOSpeed_TypeDef speed;
    const char*   name;
} board_gpio_cfg_t;

const board_gpio_cfg_t board_gpio_table[] =
{
    {GPIOA, GPIO_Pin_0, GPIO_Mode_Out_PP, GPIO_Speed_50MHz, "LED1"},
    {GPIOA, GPIO_Pin_2, GPIO_Mode_Out_PP, GPIO_Speed_50MHz, "LED2"},
};

#define BOARD_GPIO_COUNT \
    (sizeof(board_gpio_table)/sizeof(board_gpio_cfg_t))


static uint32_t board_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    for (uint32_t i = 0; i < BOARD_GPIO_COUNT; i++)
    {
        GPIO_TypeDef* port = board_gpio_table[i].port;

        /*开时钟*/
        if (port == GPIOA) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        else if (port == GPIOB) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
        else if (port == GPIOC) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
        // 可继续扩展

        /*配置 GPIO */
        GPIO_InitStructure.GPIO_Pin   = board_gpio_table[i].pin;
        GPIO_InitStructure.GPIO_Mode  = board_gpio_table[i].mode;
        GPIO_InitStructure.GPIO_Speed = board_gpio_table[i].speed;

        GPIO_Init(port, &GPIO_InitStructure);
    }

    return 0;
}
REGISTER_INIT_TASK(board_gpio_init, INIT_PRIO_DRIVER, 1);
