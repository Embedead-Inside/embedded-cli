/**
 * @file uart_rx.c
 * @brief Renesas RX651 / RX600 シリーズ (CC-RX) 用 低レイヤUART実装
 * @details CC-RX 標準ランタイムの低レベルI/O機能 `charput` をリターゲットします。
 *
 * @copyright Copyright (c) 2026 Embedead-Inside
 * @license SPDX-License-Identifier: MIT-0
 *
 * @par 使い方
 * スマートコンフィギュレータ等でSCIを初期化した上でご使用ください。
 */

#include "bsp_uart.h"
#include "platform.h"

#if defined(BSP_MCU_RX65N) || defined(BSP_MCU_RX651)
#define SCI_REG SCI1
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
    while (SCI_REG.SSR.BIT.TEND == 0);
    SCI_REG.TDR = ch;
}

/**
 * @brief 1バイト受信
 * @param[out] ch 受信文字ポインタ
 * @return int 1:成功, 0:データなし
 */
int uart_getchar(uint8_t *ch)
{
    if ((SCI_REG.SSR.BYTE & 0x38) != 0) {
        SCI_REG.SSR.BYTE = (uint8_t)(SCI_REG.SSR.BYTE | 0xC0);
        return 0;
    }
    if (SCI_REG.SSR.BIT.RDRF == 0) {
        return 0;
    }
    *ch = SCI_REG.RDR;
    return 1;
}

/**
 * @brief CC-RX 標準ランタイム lowlvl.c 用出力リターゲット関数
 * @param[in] c 出力文字
 * @details CC-RXの `printf` ライブラリの最終段1文字出力インターフェースです。
 */
void charput(unsigned char c)
{
    if (c == '\n') {
        uart_putchar('\r');
    }
    uart_putchar(c);
}
