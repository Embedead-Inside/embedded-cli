/**
 * @file uart_cm.c
 * @brief 汎用 ARM Cortex-M (CMSIS準拠) 用 低レイヤUART実装
 * @details CMSIS定義のレジスタ（USART1/2想定）を利用した汎用構成です。
 *
 * @copyright Copyright (c) 2026 Embedead-Inside
 * @license SPDX-License-Identifier: MIT-0
 *
 * @par 使い方
 * 使用するマイコンのヘッダファイル（例: `stm32f4xx.h`）を適切に定義してご使用ください。
 */

#include "bsp_uart.h"
#include <stdint.h>

#if defined(STM32F4xx) || defined(STM32F1xx)
#include "stm32f4xx.h"
#define USART_DEV USART2
#endif

/**
 * @brief UART初期化
 */
void uart_init(void)
{
}

/**
 * @brief 1バイト送信
 * @param[in] ch 送信文字
 */
void uart_putchar(uint8_t ch)
{
#if defined(USART_DEV)
    while (!(USART_DEV->SR & USART_SR_TXE));
    USART_DEV->DR = ch;
#endif
}

/**
 * @brief 1バイト受信
 * @param[out] ch 受信データポインタ
 * @return int 1:データあり, 0:データなし
 */
int uart_getchar(uint8_t *ch)
{
#if defined(USART_DEV)
    if (!(USART_DEV->SR & USART_SR_RXNE)) {
        return 0;
    }
    *ch = (uint8_t)(USART_DEV->DR & 0xFF);
    return 1;
#else
    return 0;
#endif
}

/**
 * @brief Newlib出力オーバーライド関数
 * @param[in] file ディスクリプタ
 * @param[in] ptr バッファ
 * @param[in] len 文字列長
 * @return int 送信数
 */
int _write(int file, char *ptr, int len)
{
    (void)file;
    for (int i = 0; i < len; i++) {
        if (ptr[i] == '\n') {
            uart_putchar('\r');
        }
        uart_putchar((uint8_t)ptr[i]);
    }
    return len;
}
