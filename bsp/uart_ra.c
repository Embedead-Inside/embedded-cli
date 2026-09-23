/**
 * @file uart_ra.c
 * @brief Renesas RA シリーズ (FSP) 用 低レイyaUART実装
 * @details Flexible Software Package (FSP) の R_SCI_UART インターフェースを使用します。
 *
 * @copyright Copyright (c) 2026 Embedead-Inside
 * @license SPDX-License-Identifier: MIT-0
 *
 * @par 使い方
 * FSP Configuratorで `g_uart0` インスタンスを配置し、`R_SCI_UART_Open()` を実行してください。
 */

#include "bsp_uart.h"
#include "hal_data.h"

/**
 * @brief UART初期化
 */
void uart_init(void)
{
}

/**
 * @brief 1バイト送信
 * @param[in] ch 送信データ
 */
void uart_putchar(uint8_t ch)
{
    R_SCI_UART_Write(&g_uart0_ctrl, &ch, 1);
}

/**
 * @brief 1バイト受信
 * @param[out] ch 受信データポインタ
 * @return int 1:成功, 0:データなし
 */
int uart_getchar(uint8_t *ch)
{
    fsp_err_t err = R_SCI_UART_Read(&g_uart0_ctrl, ch, 1);
    return (err == FSP_SUCCESS) ? 1 : 0;
}

/**
 * @brief GCC標準ライブラリ出力リターゲット関数
 * @param[in] file ファイル識別子
 * @param[in] ptr データ配列
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
