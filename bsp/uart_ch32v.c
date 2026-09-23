/**
 * @file uart_ch32v.c
 * @brief WCH CH32V003 / CH32V203 / V307 (RISC-V) 用 低レイヤUART実装
 * @details WCH MounRiver Studio / EVT ライブラリの USART レジスタ構成を使用します。
 *
 * @copyright Copyright (c) 2026 Embedead-Inside
 * @license SPDX-License-Identifier: MIT-0
 *
 * @par 使い方
 * `ch32v00x.h` 等のヘッダをインクルードし、RCCクロック供給およびGPIO設定を実施してください。
 */

#include "bsp_uart.h"
#include <stdint.h>

#if defined(CH32V00X)
#include "ch32v00x.h"
#define USART_DEV USART1
#endif

#ifndef USART_STAT_TXE
#define USART_STAT_TXE (1U << 7)
#endif
#ifndef USART_STAT_RXNE
#define USART_STAT_RXNE (1U << 5)
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
#if defined(USART_DEV)
    while (!(USART_DEV->STATR & USART_STAT_TXE));
    USART_DEV->DATAR = ch;
#endif
}

/**
 * @brief 1バイト受信
 * @param[out] ch 受信文字ポインタ
 * @return int 1:成功, 0:データなし
 */
int uart_getchar(uint8_t *ch)
{
#if defined(USART_DEV)
    if (!(USART_DEV->STATR & USART_STAT_RXNE)) {
        return 0;
    }
    *ch = (uint8_t)(USART_DEV->DATAR & 0xFF);
    return 1;
#else
    return 0;
#endif
}

/**
 * @brief RISC-V GCC (Newlib) 用出力リターゲット関数
 * @param[in] file ファイルディスクリプタ
 * @param[in] ptr データポインタ
 * @param[in] len データ長
 * @return int 送信成功バイト数
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
