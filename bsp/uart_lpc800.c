/**
 * @file uart_lpc800.c
 * @brief NXP LPC800 シリーズ (Cortex-M0+) 用 低レイヤUART実装
 * @details LPC800のUSARTペリフェラルレジスタを操作します。
 *
 * @copyright Copyright (c) 2026 Embedead-Inside
 * @license SPDX-License-Identifier: MIT-0
 *
 * @par 使い方
 * Switch Matrix(SWM)でUSARTのRXD/TXDピン割り当てを行ってから初期化関数を呼び出してください。
 */

#include "bsp_uart.h"
#include <stdint.h>

#if defined(__LPC80X__) || defined(__LPC82X__) || defined(__LPC84X__)
#include "LPC8xx.h"
#endif

#ifndef USART_DEV
#define USART_DEV LPC_USART0
#endif

#define STAT_RXRDY (1U << 0)
#define STAT_TXRDY (1U << 2)

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
    while (!(USART_DEV->STAT & STAT_TXRDY));
    USART_DEV->TXDAT = ch;
#endif
}

/**
 * @brief 1バイト受信
 * @param[out] ch 受信文字ポインタ
 * @return int 1:データあり, 0:データなし
 */
int uart_getchar(uint8_t *ch)
{
#if defined(USART_DEV)
    if (!(USART_DEV->STAT & STAT_RXRDY)) {
        return 0;
    }
    *ch = (uint8_t)(USART_DEV->RXDAT & 0xFF);
    return 1;
#else
    return 0;
#endif
}

/**
 * @brief システムコール出力リターゲット関数
 * @param[in] file ディスクリプタ
 * @param[in] ptr 文字配列
 * @param[in] len バッファ長
 * @return int 書き込みバイト数
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
