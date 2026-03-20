# SimpleSched 轻量级任务调度框架V1.0
# 1、项目介绍
SimpleSched是基于 SysTick 的轻量级时间片调度器设计，实现任务自动注册、周期调度及软件定时器机制，适用于裸机嵌入式系统。

其设计并实现了基于 SysTick 的轻量级任务调度框架，支持周期任务、软件定时器及任务控制机制；通过 GCC section 与 linker script 实现任务自动注册，提升系统模块化与可扩展性；采用中断与主循环解耦设计，提高系统稳定性与实时性，并支持低功耗运行模式。

该调度器适用于资源受限场景，是 RTOS 的轻量替代方案，但在复杂系统中可进一步演进为基于优先级和事件驱动的调度模型。  

核心：是时间驱动，但是可以支持事件。 属于时间驱动 + 事件触发（混合模型）  

> SysTick → 驱动调度
>
> 中断 → 产生事件
>
> 任务 → 处理事件
>

总体来说，当前调度框架采用基于 SysTick 的时间驱动模型，通过周期调度实现任务执行，同时结合中断机制产生事件，在任务中进行处理，形成时间驱动与事件触发相结合的调度方式。  

# 2、架构模型：
> SysTick中断（1ms）  
        ↓  
更新时间基（system_tick）  
        ↓  
设置调度标志（g_sched_pending）  
        ↓  
主循环轮询  
        ↓  
scheduler_run()  
        ↓  
按周期执行任务
>

# 3、核心机制
## 3.1、时间驱动（Tick-based）
+ 使用 SysTick 产生 1ms 节拍
+ 所有任务基于 tick 计数执行

## 3.2、周期调度（轮询调度）
+ tick_counter
+ 到0 → 执行任务 → 重装周期

## 3.3、自动注册机制：REGISTER_PERIODIC_TASK(...)
+ 基于 linker section
+ 启动时自动收集任务
+ 无需手动维护任务表

## 3.4、软件定时器
+ 基于 tick 递减
+ 到期触发回调函数

## 3.5、SimpleSched VS FreeRTOS
当前调度框架基于 SysTick 实现时间驱动调度，与 FreeRTOS 类似具备统一时基；其差异在于当前为协作式调度，未实现任务优先级、阻塞机制及上下文切换。通过引入任务状态管理、优先级调度及 PendSV 上下文切换机制，可进一步演进为类 RTOS 内核。  

| 能力 | SimpleSched | FreeRTOS |
| --- | --- | --- |
| 时间驱动 | ✔ | ✔ |
| 事件驱动 | 简单flag | 完整机制（队列/信号量） |
| 优先级 | ❌ | ✔ |
| 抢占 | ❌ | ✔ |
| 上下文切换 | ❌ | ✔ |
| 多任务并发 | ❌（伪并发） | ✔（真并发） |


# 4、使用方法
以STM32F103C8T6为例。目前项目中启动文件和链接文件都基于此芯片。
初始化调度器
在 main.c 中：
#include "sched.h"

int main(void)
{
    sched_init();     // 执行所有初始化任务
    sched_start();    // 启动调度器（不会返回）

    while (1);
}

添加初始化任务
用于系统初始化（GPIO / UART / 外设等）
static uint32_t board_init(void)
{
    // 硬件初始化代码
    return 0;
}

REGISTER_INIT_TASK(board_init, INIT_PRIO_DRIVER, 1);

添加周期任务
static void led_task(void)
{
    // 周期执行代码
}

REGISTER_PERIODIC_TASK(led_task, TASK_PERIOD_100MS, 1);
编译
直接运行build.bat
烧录
直接运行falsh.bat


# 5、源码解析


