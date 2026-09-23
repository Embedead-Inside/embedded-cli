/**
 * @file bsp_uart.h
 * @brief 組み込みCLI用 汎用低レイヤUARTハードウェア抽象化層（HAL）インターフェース
 * @details 各種マイコン・FPGA環境で共通利用する非ブロッキング型UART制御の統一インターフェースを提供します。
 *
 * @copyright Copyright (c) 2026 Embedead-Inside
 * @license SPDX-License-Identifier: MIT-0
 */

#ifndef BSP_UART_H
#define BSP_UART_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ハードウェアUARTの低レイヤ初期化
 * @details ターゲットペリフェラルの初期化を行います。
 *          BSPやブートローダ側で事前初期化される環境では空関数とすることも可能です。
 */
void uart_init(void);

/**
 * @brief 1バイト送信（ポーリング・ブロッキング）
 * @param[in] ch 送信するデータバイト
 * @details 送信バッファが空くまで待機した後、1バイトを出力します。
 */
void uart_putchar(uint8_t ch);

/**
 * @brief 1バイト受信（ノンブロッキング）
 * @param[out] ch 受信データを格納する変数へのポインタ
 * @return int データを受信した場合は1、データが存在しない場合は0を返します。
 * @details CPU処理をブロックせずに受信バッファの状態を確認して1バイト読み込みます。
 */
int uart_getchar(uint8_t *ch);

#ifdef __cplusplus
}
#endif

#endif /* BSP_UART_H */
