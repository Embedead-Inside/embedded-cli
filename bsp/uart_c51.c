/**
 * @file uart_c51.c
 * @brief 8051 アーキテクチャ (STCマイコン / SDCC / Keil C51) 用 低レイヤUART実装
 * @details 8051標準の SCON, SBUF レジスタを制御します。
 *
 * @copyright Copyright (c) 2026 Embedead-Inside
 * @license SPDX-License-Identifier: MIT-0
 *
 * @par 使い方
 * タイマー1等を利用したボーレート設定を事前に記述してください。
 */

#include "bsp_uart.h"

#if defined(__SDCC)
#include <8051.h>
#else
#include <reg51.h>
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
    SBUF = ch;
    while (!TI);
    TI = 0;
}

/**
 * @brief 1バイト受信
 * @param[out] ch 受信文字ポインタ
 * @return int 1:成功, 0:データなし
 */
int uart_getchar(uint8_t *ch)
{
    if (!RI) {
        return 0;
    }
    RI = 0;
    *ch = SBUF;
    return 1;
}

/**
 * @brief Keil C51 / SDCC 標準出力リターゲット関数
 * @param[in] c 出力文字
 * @return char 出力文字
 * @details `printf` 実行時に内部から自動的に呼び出されます。
 */
char putchar(char c)
{
    if (c == '\n') {
        uart_putchar('\r');
    }
    uart_putchar((uint8_t)c);
    return c;
}
