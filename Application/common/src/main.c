#include "cm_include.h"
#include "user_include.h"

/* ===================== main ===================== */
int main(void)
{
    /* 调度器初始化（执行所有 init 任务） */
    sched_init();

    /* 启动调度器（不会返回） */
    sched_start();

    while (1);
}