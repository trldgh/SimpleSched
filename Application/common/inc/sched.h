#ifndef SCHED_H
#define SCHED_H

#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 *                              类型定义
 ******************************************************************************/
typedef void (*task_func_t)(void);
typedef uint32_t (*init_func_t)(void);

/* 初始化任务结构 */
typedef struct {
    init_func_t init_func;
    uint8_t priority;
    uint8_t enable;
} init_task_t;

/* 周期任务结构 */
typedef struct {
    task_func_t task_func;
    uint32_t period_ms;
    uint8_t enable;
} periodic_task_t;

/* 调度任务结构 */
typedef struct {
    uint8_t enable;
    uint32_t period_ms;
    uint32_t tick_counter;
    task_func_t task_func;
} sched_task_t;

/* 软定时器结构 */
typedef struct {
    uint8_t active;
    uint32_t delay_ms;
    task_func_t callback;
} soft_timer_t;

/*******************************************************************************
 *                              枚举定义
 ******************************************************************************/
enum init_priority {
    INIT_PRIO_LOWEST    = 0,
    INIT_PRIO_DEFAULT   = 10,
    INIT_PRIO_HIGHEST   = 20,
    
    INIT_PRIO_CORE      = 20,      // 核心初始化（如SysTick）
    INIT_PRIO_DRIVER    = 15,      // 驱动初始化
    INIT_PRIO_APP       = 10,      // 应用初始化
};

enum task_period {
    TASK_PERIOD_1MS   = 1,
    TASK_PERIOD_5MS   = 5,
    TASK_PERIOD_10MS  = 10,
    TASK_PERIOD_20MS  = 20,
    TASK_PERIOD_50MS  = 50,
    TASK_PERIOD_100MS = 100,
    TASK_PERIOD_200MS = 200,
    TASK_PERIOD_500MS = 500,
    TASK_PERIOD_1S    = 1000,
};

/*******************************************************************************
 *                              宏定义
 ******************************************************************************/
#define MAX_TASKS           20      // 最大任务数
#define MAX_TIMERS          10      // 最大软定时器数

/* 断言宏 */
#define SCHED_ASSERT_ENABLE     1

#if SCHED_ASSERT_ENABLE
void sched_assert(const char *file, uint32_t line);
#define SCHED_ASSERT(expr) \
    do { \
        if (!(expr)) \
            sched_assert(__FILE__, __LINE__); \
    } while(0)
#else
#define SCHED_ASSERT(expr) ((void)0)
#endif

/* 链接段定义 - 用于自动注册 */
#define __INIT_TASK __attribute__((unused, section(".init_task")))
#define __PERIODIC_TASK __attribute__((unused, section(".periodic_task")))

/* 注册宏 */
#define REGISTER_INIT_TASK(func, prio, en) \
    init_task_t _init_##func __INIT_TASK = {func, prio, en}

#define REGISTER_PERIODIC_TASK(func, period, en) \
    periodic_task_t _task_##func __PERIODIC_TASK = {func, period, en}

/*******************************************************************************
 *                              全局变量声明
 ******************************************************************************/
extern volatile uint32_t system_tick;
extern sched_task_t g_task_table[MAX_TASKS];
extern soft_timer_t g_timer_pool[MAX_TIMERS];

/*******************************************************************************
 *                              函数声明
 ******************************************************************************/
/* 初始化与调度 */
void sched_init(void);
void sched_start(void);
void sched_tick_handler(void);

/* 任务控制 */
void sched_task_suspend(task_func_t task);
void sched_task_resume(task_func_t task);
uint8_t sched_task_get_status(task_func_t task);

/* 软定时器 */
uint8_t sched_timer_start(uint32_t ms, task_func_t callback);
void sched_timer_stop(uint8_t timer_id);
uint8_t sched_timer_is_expired(uint8_t timer_id);

/* 系统工具 */
void sched_delay_ms(uint32_t ms);
void sched_delay_us(uint32_t us);
uint32_t sched_get_tick(void);

#endif


