/**
 * @file uart_rp2350.c
 * @brief Raspberry Pi RP2040 / RP2350 (Pico SDK) 用 低レイヤUART実装
 * @details Pico SDKの `hardware_uart` を使用したUART制御および標準出力のリターゲットを行います。
 *
 * @copyright Copyright (c) 2026 Embedead-Inside
 * @license SPDX-License-Identifier: MIT-0
 *
 * @par 使い方
 * 1. CMakeLists.txt に `hardware_uart` および `pico_stdlib` を追加します。
 * 2. 呼び出し前に `stdio_init_all()` または `uart_init()` でハードウェア初期化を行ってください。
 */

#include "bsp_uart.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"

#ifndef UART_ID
#define UART_ID uart0
#endif

/**
 * @brief UARTペリフェラルの初期化
 * @details Pico SDK依存のハードウェア初期化を実施します。
 */
void uart_init(void)
{
    /* SDKの初期化依存 */
}

/**
 * @brief 1バイト送信（ポーリング）
 * @param[in] ch 送信文字データ
 */
void uart_putchar(uint8_t ch)
{
    uart_putc_raw(UART_ID, (char)ch);
}

/**
 * @brief 1バイト受信（ノンブロッキング）
 * @param[out] ch 受信データ格納ポインタ
 * @return int 1:受信成功, 0:データなし
 */
int uart_getchar(uint8_t *ch)
{
    if (!uart_is_readable(UART_ID)) {
        return 0;
    }
    *ch = (uint8_t)uart_getc(UART_ID);
    return 1;
}

/**
 * @brief Newlib標準出力リターゲット用関数
 * @param[in] file ファイル識別子
 * @param[in] ptr 送信文字列バッファ
 * @param[in] len 送信文字数
 * @return int 送信成功バイト数
 * @details printf等の改行コード変換(\n -> \r\n)を行ってUARTへ出力します。
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
