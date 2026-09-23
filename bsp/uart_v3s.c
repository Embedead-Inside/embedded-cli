/**
 * @file uart_v3s.c
 * @brief Allwinner V3 / V3s (ARM Cortex-A7) 用 低レイヤUART実装
 * @details 16550互換UARTレジスタを直接操作し、ベアメタル環境等で動作します。
 *
 * @copyright Copyright (c) 2026 Embedead-Inside
 * @license SPDX-License-Identifier: MIT-0
 *
 * @par 使い方
 * クロック（CCU）およびGPIO設定が完了しているメモリマップドレジスタのアドレスを設定してください。
 */

#include "bsp_uart.h"
#include <stdint.h>

#ifndef UART_BASE
#define UART_BASE (0x01C28000UL)
#endif

#define UART_RBR (*(volatile uint32_t *)(UART_BASE + 0x00))
#define UART_THR (*(volatile uint32_t *)(UART_BASE + 0x00))
#define UART_LSR (*(volatile uint32_t *)(UART_BASE + 0x14))

#define LSR_DR   (1 << 0)
#define LSR_THRE (1 << 5)

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
    while ((UART_LSR & LSR_THRE) == 0);
    UART_THR = ch;
}

/**
 * @brief 1バイト受信
 * @param[out] ch 受信文字ポインタ
 * @return int 1:データあり, 0:データなし
 */
int uart_getchar(uint8_t *ch)
{
    if ((UART_LSR & LSR_DR) == 0) {
        return 0;
    }
    *ch = (uint8_t)(UART_RBR & 0xFF);
    return 1;
}

/**
 * @brief Newlib出力オーバーライド関数
 * @param[in] file ファイルディスクリプタ
 * @param[in] ptr 文字配列
 * @param[in] len 長さ
 * @return int 書き込み数
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
