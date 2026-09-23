/**
 * @file uart_msp430.c
 * @brief TI MSP430 シリーズ (msp430-gcc) 用 低レイヤUART実装
 * @details eUSCI_A モジュールレジスタを制御し、`putchar` を書き換えます。
 *
 * @copyright Copyright (c) 2026 Embedead-Inside
 * @license SPDX-License-Identifier: MIT-0
 *
 * @par 使い方
 * クロック（DCO）および eUSCI_A0 のボーレートジェネレータ設定を事前に実施してください。
 */

#include "bsp_uart.h"
#include <msp430.h>
#include <stdio.h>

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
    while (!(UCA0IFG & UCTXIFG));
    UCA0TXBUF = ch;
}

/**
 * @brief 1バイト受信
 * @param[out] ch 受信文字ポインタ
 * @return int 1:成功, 0:データなし
 */
int uart_getchar(uint8_t *ch)
{
    if (!(UCA0IFG & UCRXIFG)) {
        return 0;
    }
    *ch = UCA0RXBUF;
    return 1;
}

/**
 * @brief msp430-gcc 用 putchar フック関数
 * @param[in] c 出力文字
 * @return int 出力文字
 */
int putchar(int c)
{
    if (c == '\n') {
        uart_putchar('\r');
    }
    uart_putchar((uint8_t)c);
    return c;
}
