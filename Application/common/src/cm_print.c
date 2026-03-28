#include <stdio.h>

// #define MYPRINTF(type, fmt, ...) \
//     printf("[%lu][%d] " fmt, system_tick, type, ##__VA_ARGS__)


// int fputc(int ch, FILE *f)
// {
//     USART_SendData(USART1, ch);
//     while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
//     return ch;
// }