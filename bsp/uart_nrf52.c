/**
 * @file uart_nrf52.c
 * @brief Nordic nRF52 / nRF53 シリーズ (nRF Connect SDK / Zephyr) 用 低レイヤUART実装
 * @details Zephyr RTOS の `uart_poll_in` / `uart_poll_out` API を呼び出します。
 *
 * @copyright Copyright (c) 2026 Embedead-Inside
 * @license SPDX-License-Identifier: MIT-0
 *
 * @par 使い方
 * Zephyr の Device Tree で `CONFIG_SERIAL=y` およびノードラベル（`uart0`）を設定してください。
 */

#include "bsp_uart.h"
#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>

static const struct device *uart_dev = DEVICE_DT_GET(DT_NODELABEL(uart0));

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
    if (device_is_ready(uart_dev)) {
        uart_poll_out(uart_dev, ch);
    }
}

/**
 * @brief 1バイト受信
 * @param[out] ch 受信文字ポインタ
 * @return int 1:成功, 0:データなし
 */
int uart_getchar(uint8_t *ch)
{
    if (!device_is_ready(uart_dev)) {
        return 0;
    }
    return (uart_poll_in(uart_dev, ch) == 0) ? 1 : 0;
}
