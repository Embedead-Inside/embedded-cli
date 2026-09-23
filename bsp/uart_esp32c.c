/**
 * @file uart_esp32_c.c
 * @brief ESP32-C3 / C6 / H2 (ESP-IDF RISC-Vコア) 用 低レイヤUART実装
 * @details ESP-IDFの `driver/uart.h` を用いてドライバAPIレベルでアクセスします。
 *
 * @copyright Copyright (c) 2026 Embedead-Inside
 * @license SPDX-License-Identifier: MIT-0
 *
 * @par 使い方
 * ESP-IDF標準の `app_main` 前後でUARTドライバ設定を行った後にご使用ください。
 */

#include "bsp_uart.h"
#include "driver/uart.h"

#ifndef UART_PORT
#define UART_PORT UART_NUM_0
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
    uart_write_bytes(UART_PORT, (const char *)&ch, 1);
}

/**
 * @brief 1バイト受信
 * @param[out] ch 受信文字ポインタ
 * @return int 1:成功, 0:データなし
 */
int uart_getchar(uint8_t *ch)
{
    int len = uart_read_bytes(UART_PORT, ch, 1, 0);
    return (len > 0) ? 1 : 0;
}
