/**
 * @file ecli.h
 * @brief 組み込み向けCLI（Embedded CLI）コアエンジン・インターフェース
 * @details メインループで呼び出すポーリング関数、各種設定マクロ、初期化APIを提供します。
 *
 * @copyright Copyright (c) 2026 Embedead-Inside
 * @license SPDX-License-Identifier: MIT-0
 */

#ifndef ECLI_H
#define ECLI_H

#ifdef __cplusplus
extern "C" {
#endif

/** @brief 1行あたりの最大入力バッファサイズ（終端文字含む） */
#define ECLI_LINE_MAX    64

/** @brief 1つのコマンドで受け入れ可能な最大引数数 */
#define ECLI_MAX_ARGS    8

/**
 * @brief ECLIコアパーサーおよびハードウェアインターフェースの初期化
 * @details 内部バッファのクリア、UART HALの初期化、プロンプト文字列の出力を行います。
 */
void ecli_init(void);

/**
 * @brief 入力文字列を処理するノンブロッキング・ポーリング関数
 * @details メインのスーパーループ内または低優先度タスクから周期的に呼び出してください。
 *          バックスペース処理、エコーバック、改行検知、コマンド実行処理をハンドリングします。
 */
void ecli_poll(void);

#ifdef __cplusplus
}
#endif

#endif /* ECLI_H */
