/**
 * @file uart_rl78.c
 * @brief Renesas RL78 マイコン (CC-RL) 用 低レイヤUART実装
 * @details ルネサス CC-RL コンパイラ標準の `putchar` をフックして動作します。
 *
 * @copyright Copyright (c) 2026 Embedead-Inside
 * @license SPDX-License-Identifier: MIT-0
 */

#include "bsp_uart.h"
#include "iodefine.h"

#define UART_TX_STATUS_REG      SSR00   /* 送信ステータスレジスタ */
#define UART_TX_DATA_REG        SDR00L  /* 送信データレジスタ（下位8bit） */

#define UART_RX_STATUS_REG      SSR01   /* 受信ステータスレジスタ */
#define UART_RX_DATA_REG        SDR01L  /* 受信データレジスタ（下位8bit） */

/* SAU（シリアルアレイユニット）共通のビットマスク定義 */
#define SAU_BUFF_EMPTY_MASK     (0x0020U) /* バッファ空きフラグ (BFFxx) */
#define SAU_UNDER_EXECUTE_MASK  (0x0040U) /* 転送中フラグ (SExx/データ存在確認) */


/**
 * @brief UART初期化
 */
void uart_init(void)
{
    /* Smart Configurator等で初期化するため、ここでは空 */
}

/**
 * @brief 1バイト送信
 * @param[in] ch 送信文字
 */
void uart_putchar(uint8_t ch)
{
    /* 送信バッファが満杯の間は待機 */
    while (UART_TX_STATUS_REG & SAU_BUFF_EMPTY_MASK);
    UART_TX_DATA_REG = ch;
}

/**
 * @brief 1バイト受信
 * @param[out] ch 受信文字ポインタ
 * @return int 1:成功, 0:データなし
 */
int uart_getchar(uint8_t *ch)
{
    /* 受信完了（データあり）かチェック */
    if (!(UART_RX_STATUS_REG & SAU_UNDER_EXECUTE_MASK)) {
        return 0;
    }
    *ch = UART_RX_DATA_REG;
    return 1;
}

/**
 * @brief CC-RL 標準ライブラリ用1文字出力関数
 * @param[in] c 出力コード
 * @return int 出力コード
 * @details `printf` 呼び出し時に CC-RL ランタイムライブラリより内部的に直接コールされます。
 */
int putchar(int c)
{
    if (c == '\n') {
        uart_putchar('\r');
    }
    uart_putchar((uint8_t)c);
    return c;
}
