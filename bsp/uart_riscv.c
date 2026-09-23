/**
 * @file uart_riscv.c
 * @brief 汎用 RISC-V (SiFive FE310等) 用 低レイヤUART実装
 * @details SiFive型UARTレジスタ構造をメモリマップド直接制御します。
 *
 * @copyright Copyright (c) 2026 Embedead-Inside
 * @license SPDX-License-Identifier: MIT-0
 *
 * @par 使い方
 * 対象ターゲットのUARTベースアドレス（例: 0x10013000）をマクロで指定してください。
 */

#include "bsp_uart.h"
#include <stdint.h>

#ifndef UART_BASE
#define UART_BASE (0x10013000UL)
#endif

#define UART_TXDATA (*(volatile uint32_t *)(UART_BASE + 0x00))
#define UART_RXDATA (*(volatile uint32_t *)(UART_BASE + 0x04))

#define TXFULL_MASK (1U << 31)
#define RXEMPTY_MASK (1U << 31)

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
    while (UART_TXDATA & TXFULL_MASK);
    UART_TXDATA = ch;
}

/**
 * @brief 1バイト受信
 * @param[out] ch 受信文字ポインタ
 * @return int 1:成功, 0:データなし
 */
int uart_getchar(uint8_t *ch)
{
    uint32_t val = UART_RXDATA;
    if (val & RXEMPTY_MASK) {
        return 0;
    }
    *ch = (uint8_t)(val & 0xFF);
    return 1;
}

/**
 * @brief Newlib出力オーバーライド関数
 * @param[in] file ファイル
 * @param[in] ptr 文字配列
 * @param[in] len バッファサイズ
 * @return int 出力長
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
