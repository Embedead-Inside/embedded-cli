/**
 * @file uart_linux.c
 * @brief Linux / macOS / WSL (POSIX環境動作テスト用) 低レイヤUART模擬実装
 * @details POSIX terminal (termios) API を用いて標準入力を Raw モードに変更し、PC上での実機シミュレーションを行います。
 *
 * @copyright Copyright (c) 2026 Embedead-Inside
 * @license SPDX-License-Identifier: MIT-0
 *
 * @par 使い方
 * PC上で `gcc main.c ecli.c uart_linux.c` のようにビルドして直接テスト実行可能です。
 */

#include "bsp_uart.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

static struct termios orig_termios;

static void disable_raw_mode(void)
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

/**
 * @brief ターミナルのRawモード設定初期化
 */
void uart_init(void)
{
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}

/**
 * @brief 1バイト送信 (標準出力へ出力)
 * @param[in] ch 送信文字
 */
void uart_putchar(uint8_t ch)
{
    putchar((int)ch);
    fflush(stdout);
}

/**
 * @brief 1バイト受信 (標準入力からノンブロッキング取得)
 * @param[out] ch 受信文字ポインタ
 * @return int 1:成功, 0:データなし
 */
int uart_getchar(uint8_t *ch)
{
    unsigned char c;
    ssize_t nread = read(STDIN_FILENO, &c, 1);
    if (nread > 0) {
        *ch = (uint8_t)c;
        return 1;
    }
    return 0;
}
