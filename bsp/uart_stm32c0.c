/**
 * @file uart_stm32c0.c
 * @brief STMicroelectronics STM32C0 シリーズ (Cortex-M0+) 用 低レイヤUART実装
 * @details STM32C0のUSARTレジスタを直接操作し、標準出力のリターゲットを行います。
 *
 * @copyright Copyright (c) 2026 Embedead-Inside
 * @license SPDX-License-Identifier: MIT-0
 *
 * @par 使い方
 * STM32CubeMX等でUSARTのGPIO(AF)およびクロック設定を済ませてから本モジュールを呼び出してください。
 */

#include "bsp_uart.h"
#include <stdint.h>

#if defined(STM32C0xx)
#include "stm32c0xx.h"
#endif

#ifndef USART_DEV
#define USART_DEV USART1
#endif

#ifndef USART_ISR_TXE_TXFNF
#define USART_ISR_TXE_TXFNF (1U << 7)
#endif
#ifndef USART_ISR_RXNE_RXFNE
#define USART_ISR_RXNE_RXFNE (1U << 5)
#endif

/**
 * @brief UARTペリフェラルの初期化
 */
void uart_init(void)
{
}

/**
 * @brief 1バイト送信（ポーリング）
 * @param[in] ch 送信文字データ
 */
void uart_putchar(uint8_t ch)
{
#if defined(USART_DEV)
    while (!(USART_DEV->ISR & USART_ISR_TXE_TXFNF));
    USART_DEV->TDR = ch;
#endif
}

/**
 * @brief 1バイト受信（ノンブロッキング）
 * @param[out] ch 受信データ格納ポインタ
 * @return int 1:受信成功, 0:データなし
 */
int uart_getchar(uint8_t *ch)
{
#if defined(USART_DEV)
    if (USART_DEV->ISR & (USART_ISR_ORE | USART_ISR_NE | USART_ISR_FE)) {
        USART_DEV->ICR = (USART_ICR_ORECF | USART_ICR_NCF | USART_ICR_FECF);
    }
    if (!(USART_DEV->ISR & USART_ISR_RXNE_RXFNE)) {
        return 0;
    }
    *ch = (uint8_t)(USART_DEV->RDR & 0xFF);
    return 1;
#else
    return 0;
#endif
}

/**
 * @brief Newlib標準出力リターゲット関数
 * @param[in] file ファイルハンドル
 * @param[in] ptr 出力文字バッファ
 * @param[in] len 長さ
 * @return int 出力バイト数
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
