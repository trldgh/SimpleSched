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

总体来说，当前调度框架采用基于 SysTick 的时间驱动模型，通过周期调度实现任务执行，同时结合中断机制产生事件，在任务中进行处理，形成时间驱动与事件触发相结合的调度方式。  

# 2、架构模型：

> SysTick中断（1ms）  
>    ↓  
> 更新时间基（system_tick）  
>    ↓  
> 设置调度标志（g_sched_pending）  
>    ↓  
> 主循环轮询  
>    ↓  
> scheduler_run()  
>    ↓  
> 按周期执行任务

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

| 能力       | SimpleSched | FreeRTOS                |
| ---------- | ----------- | ----------------------- |
| 时间驱动   | ✔           | ✔                       |
| 事件驱动   | 简单flag    | 完整机制（队列/信号量） |
| 优先级     | ❌           | ✔                       |
| 抢占       | ❌           | ✔                       |
| 上下文切换 | ❌           | ✔                       |
| 多任务并发 | ❌（伪并发） | ✔（真并发）             |


# 4、使用方法

以STM32F103C8T6为例。目前项目中启动文件和链接文件都基于此芯片。

## 4.1、初始化调度器

在 `main.c` 中：

```plain
#include "sched.h"

int main(void)
{
    sched_init();     // 执行所有初始化任务
    sched_start();    // 启动调度器（不会返回）

    while (1);
}
```

---

## 4.2、添加初始化任务

用于系统初始化（GPIO / UART / 外设等）

```plain
static uint32_t board_init(void)
{
    // 硬件初始化代码
    return 0;
}

REGISTER_INIT_TASK(board_init, INIT_PRIO_DRIVER, 1);
```

---

## 4.3、添加周期任务

```plain
static void led_task(void)
{
    // 周期执行代码
}

REGISTER_PERIODIC_TASK(led_task, TASK_PERIOD_100MS, 1);
```

## 4.4、定时器

 软件定时器 = 用“计数”模拟出来的定时器（不占用硬件）  

定义回调函数

```plain
void led_off_callback(void)
{
    // LED_OFF();
}
```

启动定时器

1000ms后 → 自动调用 led_off_callback()

```plain
sched_timer_start(1000, led_off_callback);
```

停止定时器

```plain
sched_timer_stop(timer_id);
```

查询状态

```plain
if (sched_timer_is_expired(id))
{
    // 已执行完成
}
```

## 4.5、定时器作用

1. 延时执行（替代 delay）

```plain
// 按键按下 → 1秒后关灯
sched_timer_start(1000, led_off);
```

👉 非阻塞  
👉 不影响其他任务

---

2. 超时机制（非常常见）

```plain
// 等待串口数据
sched_timer_start(500, uart_timeout);
```

👉 如果500ms没收到 → 超时处理

---

3. 按键消抖

```plain
key_irq():
    sched_timer_start(20, key_confirm);
```

👉 20ms后确认按键状态

---

4. 状态机延时

```plain
state = WAIT;
sched_timer_start(100, next_state);
```

👉 100ms后自动跳状态

---

5. 通信协议（核心场景）

比如：

+ Modbus
+ 蓝牙
+ UART帧

```plain
// 收到数据 → 启动帧间超时
sched_timer_start(5, frame_end);
```

---

6. 看门狗辅助

```plain
sched_timer_start(1000, system_reset);
```

👉 超时没喂狗 → 重启



当前实现是：

```plain
一次性定时器（one-shot）
```

👉 如果要周期：

```plain
void cb(void)
{
    // do something
    sched_timer_start(1000, cb);  // 重新启动
}
```

定时器VS任务调度

| 项目     | 周期任务 | 软件定时器 |
| -------- | -------- | ---------- |
| 触发方式 | 固定周期 | 一次性     |
| 用途     | 常规任务 | 延时/超时  |
| 运行位置 | 主循环   | 中断       |
| 灵活性   | 一般     | 很高       |


## 4.6、编译

直接运行build.bat

## 4.7、烧录

直接运行falsh.bat

# 5、源码解析


