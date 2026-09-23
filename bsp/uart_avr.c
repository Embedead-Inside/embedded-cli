/**
 * @file uart_avr.c
 * @brief 8bit AVR (ATmega328P等 / avr-gcc) 用 低レイヤUART実装
 * @details USART0 レジスタの制御および avr-libc ストリームのリターゲットを行います。
 *
 * @copyright Copyright (c) 2026 Embedead-Inside
 * @license SPDX-License-Identifier: MIT-0
 *
 * @par 使い方
 * `uart_init()` 内部で `stdout` を独自ストリームへ割り当てます。ボーレート設定は事前に行ってください。
 */

#include "bsp_uart.h"
#include <avr/io.h>
#include <stdio.h>

static int avr_putchar_stream(char c, FILE *stream);
static FILE uart_stdout = FDEV_SETUP_STREAM(avr_putchar_stream, NULL, _FDEV_SETUP_WRITE);

/**
 * @brief UART初期化および標準ストリーム設定
 */
void uart_init(void)
{
    stdout = &uart_stdout;
}

/**
 * @brief 1バイト送信
 * @param[in] ch 送信文字
 */
void uart_putchar(uint8_t ch)
{
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = ch;
}

/**
 * @brief 1バイト受信
 * @param[out] ch 受信文字ポインタ
 * @return int 1:データあり, 0:データなし
 */
int uart_getchar(uint8_t *ch)
{
    if (!(UCSR0A & (1 << RXC0))) {
        return 0;
    }
    *ch = UDR0;
    return 1;
}

/**
 * @brief avr-libc 専用標準出力ストリームフック関数
 * @param[in] c 出力文字
 * @param[in] stream ストリームオブジェクト
 * @return int 常に0
 * @details `printf` 呼び出し時に avr-libc の internals から呼び出されます。
 */
static int avr_putchar_stream(char c, FILE *stream)
{
    (void)stream;
    if (c == '\n') {
        uart_putchar('\r');
    }
    uart_putchar((uint8_t)c);
    return 0;
}
