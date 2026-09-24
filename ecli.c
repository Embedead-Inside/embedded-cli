/**
 * @file ecli.c
 * @brief 組み込み向けCLI（Embedded CLI）コアパーサーおよびテーブル駆動型実行エンジンの実装
 * @details ノンブロッキング文字受信、引数解析 (argc/argv)、コマンドテーブルによる検索・実行を行います。
 *
 * @copyright Copyright (c) 2026 Embedead-Inside
 * @license SPDX-License-Identifier: MIT-0
 */

#include "ecli.h"
#include "bsp_uart.h"
#include <string.h>

/** @brief CLIプロンプト文字列の定義 */
#ifndef ECLI_PROMPT
#define ECLI_PROMPT "ECLI> "
#endif

/** @brief コマンド引数切り分け用のデリミタ（区切り文字）の定義 */
#ifndef ECLI_DELIM
#define ECLI_DELIM " \t"
#endif

/** @brief マクロの値を文字列に変換するためのヘルパーマクロ */
#define ECLI_STR_HELPER(x) #x
#define ECLI_STR(x) ECLI_STR_HELPER(x)

/**
 * @brief コマンドハンドラ関数の型定義
 * @param[in] argc 解析された引数の数
 * @param[in] argv 引数文字列配列へのポインタ
 */
typedef void (*ecli_cmd_func_t)(int argc, char *argv[]);

/**
 * @struct ecli_cmd_t
 * @brief コマンドテーブル登録用構造体
 */
typedef struct {
    const char     *name; /**< コマンド名（NULLで番人を示す） */
    ecli_cmd_func_t func; /**< コマンド実行時に呼び出す関数ポインタ */
    const char     *help; /**< 'help' コマンドで表示する説明文 */
} ecli_cmd_t;

/* 内部状態管理変数 */
static char input_buffer[ECLI_LINE_MAX];
static int buffer_idx = 0;
static uint8_t last_ch = 0; /* CR+LF 重複検出用の直前文字保持 */

/* 前方宣言 */
static void cmd_help(int argc, char *argv[]);

/**
 * @brief 文字列をUARTへ直接送信するヘルパー関数
 * @param[in] str 送信するNULL終端文字列
 */
static void uart_puts(const char *str)
{
    while (*str != '\0') {
        if (*str == '\n') {
            uart_putchar('\r');
        }
        uart_putchar((uint8_t)*str);
        str++;
    }
}

/**
 * @brief 'version' コマンドのハンドラ関数
 * @param[in] argc 引数の数
 * @param[in] argv 引数文字列配列
 */
static void cmd_version(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    uart_puts("ECLI Firmware v1.0.0\n");
}

/**
 * @brief ECLIコマンドテーブルの定義（番人方式）
 * @note 配列の最後に name が NULL の要素（番人）を配置します。
 */
static const ecli_cmd_t cmd_table[] = {
    { "help",    cmd_help,    "Show available commands" },
    { "version", cmd_version, "Show firmware version" },
    { NULL,      NULL,        NULL } /* 番人 (Sentinel) */
};

/**
 * @brief 'help' コマンドのハンドラ関数
 * @param[in] argc 引数の数
 * @param[in] argv 引数文字列配列
 * @details コマンドテーブルを番人（name == NULL）まで巡回してヘルプを表示します。
 */
static void cmd_help(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    uart_puts("Available Commands:\n");
    for (int i = 0; cmd_table[i].name != NULL; i++) {
        if (cmd_table[i].help == NULL){ //
            continue;
        }
        uart_puts("  ");
        uart_puts(cmd_table[i].name);
        uart_puts("\t: ");
        uart_puts(cmd_table[i].help);
        uart_puts("\n");
    }
}

/**
 * @brief 文字がデリミタに含まれているか判定するヘルパー関数
 * @param[in] c 判定対象の文字
 * @param[in] delim デリミタ文字列
 * @return int 1:該当する, 0:該当しない
 */
static int is_delimiter(char c, const char *delim)
{
    while (*delim != '\0') {
        if (c == *delim) {
            return 1;
        }
        delim++;
    }
    return 0;
}

/**
 * @brief 完全無依存・スレッドセーフなトークン分離処理 (ecli専用リエントラント実装)
 * @details C標準ライブラリの関数呼出を排除し、名前衝突（strtok_r）を回避。
 * @param[in] str 解析対象の文字列 (初回呼び出し時以外は NULL)
 * @param[in] delim 区切り文字セット
 * @param[in,out] saveptr 状態保存用の内部ポインタ
 * @return char* 分離されたトークンへのポインタ (終端に達した場合は NULL)
 */
static char *ecli_strtok_r(char *str, const char *delim, char **saveptr)
{
    char *token;

    if (str == NULL) {
        str = *saveptr;
    }
    if (str == NULL) {
        return NULL;
    }

    /* 先頭の区切り文字（デリミタ）をスキップ */
    while ((*str != '\0') && is_delimiter(*str, delim)) {
        str++;
    }
    if (*str == '\0') {
        *saveptr = NULL;
        return NULL;
    }

    token = str;

    /* トークンの末尾（次の区切り文字）を検索 */
    while ((*str != '\0') && !is_delimiter(*str, delim)) {
        str++;
    }

    if (*str == '\0') {
        *saveptr = NULL;
    } else {
        *str = '\0';
        *saveptr = str + 1;
    }

    return token;
}

/**
 * @brief 受信文字列を引数（argc/argv）に分解して対応するコマンドを実行
 * @param[in,out] line 入力文字列バッファ（破壊的解析が行われます）
 */
static void execute_command(char *line)
{
    char *argv[ECLI_MAX_ARGS];
    int argc = 0;
    char *saveptr = NULL;

    /* ecli_strtok_r によるトークン分割 */
    char *token = ecli_strtok_r(line, ECLI_DELIM, &saveptr);
    while ((token != NULL) && (argc < ECLI_MAX_ARGS)) {
        argv[argc++] = token;
        token = ecli_strtok_r(NULL, ECLI_DELIM, &saveptr);
    }

    /* 空行の場合は何もしない */
    if (argc == 0) {
        return;
    }

    /* 引数上限チェック：まだトークンが残っている場合は上限超過エラーとする */
    if (token != NULL) {
        uart_puts("Error: Too many arguments. Maximum allowed is " ECLI_STR(ECLI_MAX_ARGS) ".\n");
        return;
    }

	/* 番人（name == NULL）に達するまでテーブルを検索して実行 */
    for (int i = 0; cmd_table[i].name != NULL; i++) {
        if (strcmp(argv[0], cmd_table[i].name) == 0) {
            cmd_table[i].func(argc, argv);
            return;
        }
    }

    /* 一致するコマンドが存在しない場合の警告メッセージ */
    uart_puts("Unknown command: '");
    uart_puts(argv[0]);
    uart_puts("'. Type 'help'.\n");
}

/**
 * @brief ECLIコアパーサーおよびハードウェアインターフェースの初期化
 * @details 内部バッファのクリア、UART HALの初期化、プロンプト文字列の出力を行います。
 */
void ecli_init(void)
{
    uart_init();
    memset(input_buffer, 0, sizeof(input_buffer));
    buffer_idx = 0;
    last_ch = 0;

    uart_puts(ECLI_PROMPT);
}

/**
 * @brief 入力文字列を処理するノンブロッキング・ポーリング関数
 * @details メインのスーパーループ内または低優先度タスクから周期的に呼び出してください。
 *          バックスペース処理、エコーバック、CR+LF重複回避、コマンド実行処理をハンドリングします。
 */
void ecli_poll(void)
{
    uint8_t ch;

    /* UART受信ストリームをノンブロッキングで読み込み */
    while (uart_getchar(&ch)) {

        /* CR+LF の 2バイト目（直前がCRで今回がLF、または直前がLFで今回がCR）をスキップ */
        if ((ch == '\r' && last_ch == '\n') || (ch == '\n' && last_ch == '\r')) {
            last_ch = 0;
            continue;
        }

        /* 改行コード（CRまたはLF）の検出 */
        if (ch == '\r' || ch == '\n') {
            last_ch = ch; /* 改行文字を記憶 */
            input_buffer[buffer_idx] = '\0';

            uart_puts("\n");

            /* コマンド実行（入力がある場合） */
            if (buffer_idx > 0) {
                execute_command(input_buffer);
                buffer_idx = 0;
            }

            /* 常にプロンプトを表示（Enterのみの場合も改行して新プロンプトを表示） */
            uart_puts(ECLI_PROMPT);
        }
        /* バックスペース / デリートキーのハンドリング */
        else if (ch == '\b' || ch == 0x7F) {
            last_ch = ch;
            if (buffer_idx > 0) {
                buffer_idx--;
                /* ターミナル上の表示消去シーケンス */
                uart_putchar('\b');
                uart_putchar(' ');
                uart_putchar('\b');
            }
        }
        /* 通常の入力文字のバッファ格納 */
        else {
            last_ch = ch;
            if (buffer_idx < (ECLI_LINE_MAX - 1)) {
                input_buffer[buffer_idx++] = (char)ch;
                uart_putchar(ch); /* エコーバック */
            } else {
                /* バッファあふれ時の警告音（ベル） */
                uart_putchar('\a');
            }
        }
    }
}
