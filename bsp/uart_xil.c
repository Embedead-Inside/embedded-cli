/**
 * @file uart_xil.c
 * @brief AMD (Xilinx) MicroBlaze / Zynq / Versal 用 低レイヤUART実装
 * @details Xilinx standalone BSP の XUartPs 制御および `outbyte` オーバーライドを処理します。
 *
 * @copyright Copyright (c) 2026 Embedead-Inside
 * @license SPDX-License-Identifier: MIT-0
 *
 * @par 使い方
 * AMD Vitis の Platform BSP 生成設定で stdin/stdout が有効化されているか確認してください。
 */

#include "bsp_uart.h"
#include "xparameters.h"
#include "xuartps_hw.h"

#ifndef UART_BASE
#define UART_BASE XPAR_XUARTPS_0_BASEADDR
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
    while (XUartPs_IsTransmitFull(UART_BASE));
    XUartPs_WriteReg(UART_BASE, XUARTPS_FIFO_OFFSET, ch);
}

/**
 * @brief 1バイト受信
 * @param[out] ch 受信文字ポインタ
 * @return int 1:成功, 0:データなし
 */
int uart_getchar(uint8_t *ch)
{
    if (!XUartPs_IsReceiveData(UART_BASE)) {
        return 0;
    }
    *ch = (uint8_t)XUartPs_ReadReg(UART_BASE, XUARTPS_FIFO_OFFSET);
    return 1;
}

/**
 * @brief Xilinx BSP 標準出力オーバーライド関数
 * @param[in] c 出力文字
 * @details Xilinx Newlib 環境で `printf` から呼び出される1文字出力関数です。
 */
void outbyte(char c)
{
    if (c == '\n') {
        uart_putchar('\r');
    }
    uart_putchar((uint8_t)c);
}
