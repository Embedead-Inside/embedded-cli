/**
 * @file uart_win32.c
 * @brief Windows (CMD / PowerShell / MSVC / MinGW 用) 低レイヤUART模擬実装
 * @details Win32 Console API を用いて入力エコー・行バッファリングを無効化し、PC上でシリアル操作を模擬します。
 *
 * @copyright Copyright (c) 2026 Embedead-Inside
 * @license SPDX-License-Identifier: MIT-0
 *
 * @par 使い方
 * Visual Studio や MinGW(gcc) を使用してWindows環境上で単体動作テストを行う際に利用できます。
 */

#include "bsp_uart.h"
#include <stdio.h>
#include <windows.h>
#include <conio.h>

static HANDLE h_stdin;
static DWORD orig_mode;

static void restore_console_mode(void)
{
    SetConsoleMode(h_stdin, orig_mode);
}

/**
 * @brief Windowsコンソールモードの初期化
 */
void uart_init(void)
{
    h_stdin = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(h_stdin, &orig_mode);
    atexit(restore_console_mode);

    DWORD raw_mode = orig_mode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT);
    SetConsoleMode(h_stdin, raw_mode);
}

/**
 * @brief 1バイト送信
 * @param[in] ch 送信文字
 */
void uart_putchar(uint8_t ch)
{
    putchar((int)ch);
    fflush(stdout);
}

/**
 * @brief 1バイト受信 (ノンブロッキング)
 * @param[out] ch 受信文字ポインタ
 * @return int 1:成功, 0:データなし
 */
int uart_getchar(uint8_t *ch)
{
    if (_kbhit()) {
        *ch = (uint8_t)_getch();
        return 1;
    }
    return 0;
}
