/**
 * @file uart_pic32.c
 * @brief Microchip PIC32MX/MZ (XC32) 用 低レイヤUART実装
 * @details PIC32 UARTレジスタ制御および XC32の `_mon_putc` のオーバーライドを行います。
 *
 * @copyright Copyright (c) 2026 Embedead-Inside
 * @license SPDX-License-Identifier: MIT-0
 *
 * @par 使い方
 * MPLAB Code Configurator (MCC) や Harmony でUARTモジュールとPPS設定を済ませておきます。
 */

#include "bsp_uart.h"
#include <xc.h>

#ifndef UART_CH
#define UART_CH 1
#endif

#if (UART_CH == 1)
  #define U_STA  U1STA
  #define U_TXR  U1TXREG
  #define U_RXR  U1RXREG
  #define U_STAb U1STAbits
#endif

/**
 * @brief UART初期化
 */
void uart_init(void)
{
}

/**
 * @brief 1バイト送信
 * @param[in] ch 送信データ
 */
void uart_putchar(uint8_t ch)
{
    while (U_STAb.UTXBF == 1);
    U_TXR = ch;
}

/**
 * @brief 1バイト受信
 * @param[out] ch 受信文字ポインタ
 * @return int 1:データあり, 0:データなし
 */
int uart_getchar(uint8_t *ch)
{
    if (U_STAb.OERR == 1) {
        U_STAb.OERR = 0;
        return 0;
    }
    if (U_STAb.URXDA == 0) {
        return 0;
    }
    *ch = (uint8_t)U_RXR;
    return 1;
}

/**
 * @brief XC32ライブラリ固有出力リターゲット関数
 * @param[in] c 出力文字
 * @details XC32 の Cライブラリ（printf）から低レベル1文字出力として直接呼ばれます。
 */
void _mon_putc(char c)
{
    if (c == '\n') {
        uart_putchar('\r');
    }
    uart_putchar((uint8_t)c);
}
