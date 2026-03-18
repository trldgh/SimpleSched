#include <stddef.h>
#include "cm_include.h"
#include "user_include.h"


/* 外部符号（链接脚本提供） */
extern init_task_t _init_task_start[];
extern init_task_t _init_task_end[];
extern periodic_task_t _periodic_task_start[];
extern periodic_task_t _periodic_task_end[];

/* 宏定义 */
#define TICK_PER_SEC 1000

/* 全局变量 */
volatile uint32_t system_tick = 0;
sched_task_t g_task_table[MAX_TASKS] = {0};
soft_timer_t g_timer_pool[MAX_TIMERS] = {0};
static uint8_t g_task_count = 0;
static volatile uint8_t g_sched_pending = 0;

/* 断言 */
void sched_assert(const char *file, uint32_t line)
{
    __disable_irq();
    while (1);
}

/* ===================== SysTick 初始化（StdPeriph版本） ===================== */
static uint32_t systick_init(void)
{
    /* SystemCoreClock 在 system_stm32f10x.c 中定义 */
    if (SysTick_Config(SystemCoreClock / TICK_PER_SEC)) {
        return 1;
    }

    /* 设置优先级 */
    NVIC_SetPriority(SysTick_IRQn, 0);

    return 0;
}
REGISTER_INIT_TASK(systick_init, INIT_PRIO_CORE, 1);

/* ===================== 初始化任务 ===================== */
void sched_init(void)
{
    init_task_t *init;
    int prio;

    for (prio = INIT_PRIO_HIGHEST; prio >= INIT_PRIO_LOWEST; prio--) {
        for (init = _init_task_start; init < _init_task_end; init++) {
            if (init->priority == prio && init->enable) {
                uint32_t ret = init->init_func();
                SCHED_ASSERT(ret == 0);
            }
        }
    }
}

/* ===================== 注册周期任务 ===================== */
static void register_periodic_tasks(void)
{
    periodic_task_t *task;

    for (task = _periodic_task_start;
         task < _periodic_task_end && g_task_count < MAX_TASKS;
         task++) {

        if (task->enable && task->task_func) {
            g_task_table[g_task_count].enable = 1;
            g_task_table[g_task_count].period_ms = task->period_ms;
            g_task_table[g_task_count].tick_counter = task->period_ms;
            g_task_table[g_task_count].task_func = task->task_func;
            g_task_count++;
        }
    }
}

/* ===================== 调度器 ===================== */
static void scheduler_run(void)
{
    uint8_t i;

    for (i = 0; i < g_task_count; i++) {

        if (!g_task_table[i].enable || !g_task_table[i].task_func)
            continue;

        if (g_task_table[i].tick_counter == 0) {
            g_task_table[i].tick_counter = g_task_table[i].period_ms;
            g_task_table[i].task_func();
        } else {
            g_task_table[i].tick_counter--;
        }
    }
}

/* ===================== 软定时器 ===================== */
static void timer_pool_handler(void)
{
    uint8_t i;

    for (i = 0; i < MAX_TIMERS; i++) {
        if (g_timer_pool[i].active && g_timer_pool[i].delay_ms > 0) {

            g_timer_pool[i].delay_ms--;

            if (g_timer_pool[i].delay_ms == 0) {
                if (g_timer_pool[i].callback)
                    g_timer_pool[i].callback();

                g_timer_pool[i].active = 0;
            }
        }
    }
}
void sched_tick_handler(void)
{
    system_tick++;
    g_sched_pending = 1;

    timer_pool_handler();
}

/* ===================== 启动 ===================== */
void sched_start(void)
{
    register_periodic_tasks();

    while (1) {
        if (g_sched_pending) {
            g_sched_pending = 0;
            scheduler_run();
        }

        __WFI();
    }
}

/* ===================== 任务控制 ===================== */
void sched_task_suspend(task_func_t task)
{
    for (uint8_t i = 0; i < g_task_count; i++) {
        if (g_task_table[i].task_func == task) {
            g_task_table[i].enable = 0;
            break;
        }
    }
}

void sched_task_resume(task_func_t task)
{
    for (uint8_t i = 0; i < g_task_count; i++) {
        if (g_task_table[i].task_func == task) {
            g_task_table[i].enable = 1;
            g_task_table[i].tick_counter = g_task_table[i].period_ms;
            break;
        }
    }
}

uint8_t sched_task_get_status(task_func_t task)
{
    for (uint8_t i = 0; i < g_task_count; i++) {
        if (g_task_table[i].task_func == task) {
            return g_task_table[i].enable;
        }
    }
    return 0;
}

/* ===================== 定时器 ===================== */
uint8_t sched_timer_start(uint32_t ms, task_func_t callback)
{
    SCHED_ASSERT(ms > 0);
    SCHED_ASSERT(callback != NULL);

    for (uint8_t i = 0; i < MAX_TIMERS; i++) {
        if (!g_timer_pool[i].active) {
            g_timer_pool[i].active = 1;
            g_timer_pool[i].delay_ms = ms;
            g_timer_pool[i].callback = callback;
            return i;
        }
    }
    return 0xFF;
}

void sched_timer_stop(uint8_t timer_id)
{
    if (timer_id < MAX_TIMERS)
        g_timer_pool[timer_id].active = 0;
}

uint8_t sched_timer_is_expired(uint8_t timer_id)
{
    if (timer_id < MAX_TIMERS)
        return (g_timer_pool[timer_id].delay_ms == 0);
    return 0;
}

/* ===================== 延时 ===================== */
void sched_delay_ms(uint32_t ms)
{
    uint32_t start = system_tick;
    while ((system_tick - start) < ms) {
        __WFI();
    }
}

void sched_delay_us(uint32_t us)
{
    SCHED_ASSERT(us < 10000);

    for (uint32_t i = 0; i < us * (SystemCoreClock / 1000000 / 5); i++) {
        __NOP();
    }
}

uint32_t sched_get_tick(void)
{
    return system_tick;
}