/**
 * @file uart_alt.c
 * @brief Intel (Altera) Nios II / Nios V 用 低レイヤUART実装
 * @details Avalon UART IP の制御および HAL `_write` システムコールのフックを行います。
 *
 * @copyright Copyright (c) 2026 Embedead-Inside
 * @license SPDX-License-Identifier: MIT-0
 *
 * @par 使い方
 * Quartus Platform Designer で配置した Avalon UART のベースアドレス（`UART_0_BASE`等）を定義して使用します。
 */

#include "bsp_uart.h"
#include "system.h"
#include "altera_avalon_uart_regs.h"

#ifndef UART_BASE
#define UART_BASE UART_0_BASE
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
    while (!(IORD_ALTERA_AVALON_UART_STATUS(UART_BASE) & ALTERA_AVALON_UART_STATUS_TRDY_MSK));
    IOWR_ALTERA_AVALON_UART_TXDATA(UART_BASE, ch);
}

/**
 * @brief 1バイト受信
 * @param[out] ch 受信文字ポインタ
 * @return int 1:成功, 0:データなし
 */
int uart_getchar(uint8_t *ch)
{
    if (!(IORD_ALTERA_AVALON_UART_STATUS(UART_BASE) & ALTERA_AVALON_UART_STATUS_RRDY_MSK)) {
        return 0;
    }
    *ch = (uint8_t)IORD_ALTERA_AVALON_UART_RXDATA(UART_BASE);
    return 1;
}

/**
 * @brief Altera HAL 低レベルシステムコールオーバーライド関数
 * @param[in] file ディスクリプタ
 * @param[in] ptr ポインタ
 * @param[in] len 出力数
 * @return int 送信文字数
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
