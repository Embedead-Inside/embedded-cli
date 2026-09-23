/**
 * @file uart_c2000.c
 * @brief TI C2000 シリーズ (C28xコア / TI-Clang) 用 低レイヤUART実装
 * @details SCIA ペリフェラルレジスタを操作します。
 *
 * @copyright Copyright (c) 2026 Embedead-Inside
 * @license SPDX-License-Identifier: MIT-0
 *
 * @par 使い方
 * C2000Ware SysConfig 等で GPIO ピン設定および SCIA のクロック有効化を行ってください。
 */

#include "bsp_uart.h"
#include <stdint.h>

#ifndef SCIA_BASE
#define SCIA_BASE (0x00007050UL)
#endif

#define SCIRXST (*(volatile uint16_t *)(SCIA_BASE + 0x04))
#define SCIRXBUF (*(volatile uint16_t *)(SCIA_BASE + 0x07))
#define SCITXBUF (*(volatile uint16_t *)(SCIA_BASE + 0x09))
#define SCIFFTX  (*(volatile uint16_t *)(SCIA_BASE + 0x0A))

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
    while ((SCIFFTX & 0x1F00) >= 0x1000);
    SCITXBUF = ch;
}

/**
 * @brief 1バイト受信
 * @param[out] ch 受信文字ポインタ
 * @return int 1:成功, 0:データなし
 */
int uart_getchar(uint8_t *ch)
{
    if (!(SCIRXST & 0x0002)) {
        return 0;
    }
    *ch = (uint8_t)(SCIRXBUF & 0xFF);
    return 1;
}
