# embedded-cli

[![License: MIT-0](https://img.shields.io/badge/License-MIT--0-blue.svg)](https://spdx.org/licenses/MIT-0.html)
[![Language: C99](https://img.shields.io/badge/Language-C99-green.svg)](#)

# ECLI (Embedded Command Line Interface)

軽量・再入可能（スレッドセーフ）・マルチプラットフォーム対応の組み込み向けCLI（コマンドラインインターフェース）ライブラリです。

超小容量の8bitマイコンから、FPGA上のソフトコア、32bit ARM/RISC-Vマイコン、SoC（Cortex-A）、さらにはホストPC（Linux/Windows）上のシミュレーション環境まで、同一のコアロジックで動作します。

---

## 特徴
* **マルチプラットフォーム対応**
  * 主要な全アーキテクチャ・SDK（20種類以上）用の低レイヤUARTドライバを同梱。
* **標準ライブラリ完全非依存**
  * `printf` や標準 `strtok` の静的状態に依存せず、フットプリント（Flash/RAM）を極限まで削減。
* **リエントラント（スレッドセーフ）設計**
  * 自前実装の `ecli_strtok_r` によるスタックベース解析。マルチスレッド（FreeRTOS, Zephyr等）や割り込み環境でも安全。
* **番人方式（Sentinel）コマンドテーブル**
  * テーブルの要素数計算（`sizeof`）が不要で、直感的な配列追加のみでコマンドを拡張可能。
* **CR+LF 自動吸収・ターミナル消去対応**
  * `CR`, `LF`, `CR+LF` の各種改行コードの自動認識、バックスペース操作 (`\b`, `0x7F`) やバッファ溢れ警告 (`\a`) に対応。

---

## ディレクトリ構成
プロジェクトに組み込む際は、コアコード（`ecli.h`, `ecli.c`）および `bsp/` フォルダ内から**お使いの環境に対応する `uart_xxx.c` を1つだけ**選択してビルド対象に含めてください。

```text
.
├── ecli.h             # ECLI コアヘッダ
├── ecli.c             # ECLI コア実装（パーサー・コマンドテーブル）
├── main.c             # アプリケーションエントリポイント（実装例）
├── bsp_uart.h         # UART HAL 共通インターフェースヘッダ
└── bsp/               # 各アーキテクチャ別 低レイヤUART実装
    ├── uart_rp2350.c  # Raspberry Pi Pico 1/2 (RP2040 / RP2350)
    ├── uart_stm32c0.c # STMicroelectronics STM32C0 シリーズ
    ├── uart_lpc800.c  # NXP LPC800 シリーズ
    ├── uart_ra.c      # Renesas RA シリーズ (FSP)
    ├── uart_cm.c      # 汎用 ARM Cortex-M (CMSIS準拠)
    ├── uart_avr.c     # 8bit AVR (ATmega328P等 / avr-gcc)
    ├── uart_pic32.c   # Microchip PIC32MX/MZ (XC32)
    ├── uart_v3s.c     # Allwinner V3 / V3s (ARM Cortex-A7)
    ├── uart_esp32_c.c # ESP32-C3 / C6 / H2 (RISC-Vコア)
    ├── uart_ch32v.c   # WCH CH32V003 / V203 / V307 (RISC-V)
    ├── uart_riscv.c   # 汎用 RISC-V (SiFive等)
    ├── uart_esp32.c   # ESP32 / ESP32-S (Xtensaコア)
    ├── uart_rl78.c    # Renesas RL78 シリーズ (CC-RL)
    ├── uart_rx.c      # Renesas RX651 / RX600 シリーズ (CC-RX)
    ├── uart_xil.c     # AMD (Xilinx) MicroBlaze / Zynq / Versal
    ├── uart_alt.c     # Intel (Altera) Nios II / Nios V
    ├── uart_msp430.c  # TI MSP430 シリーズ
    ├── uart_c2000.c   # TI C2000 シリーズ (C28xコア)
    ├── uart_nrf52.c   # Nordic nRF52 / nRF53 シリーズ (Zephyr RTOS)
    ├── uart_c51.c     # 8051 アーキテクチャ (Keil C51 / SDCC)
    ├── uart_linux.c   # Linux / macOS / WSL 動作確認用
    └── uart_win32.c   # Windows 動作確認用
```

---

# 使い方
1. 初期化とメインループ呼出 (main.c)
```C
#include "ecli.h"

int main(void)
{
    /* 各種ハードウェア初期化（クロック、GPIO等） */
    // Board_Init();

    /* ECLI 初期化 */
    ecli_init();

    /* メインのスーパーループ */
    while (1) {
        /* 非ブロックでシリアル入力を受信・解析 */
        ecli_poll();

        /* その他の非ブロックタスク */
    }

    return 0;
}
```
2. コマンドの追加方法 (ecli.c)
ecli.c 内の cmd_table 配列にハンドラ関数と説明文を追加します。

```C
/* カスタムコマンドのハンドラ関数 */
static void cmd_gpio(int argc, char *argv[])
{
    if (argc < 3) {
        // 引数不足の警告等
        return;
    }
    /* argv[1], argv[2] を使用した処理 */
}

/* コマンドテーブル定義 */
static const ecli_cmd_t cmd_table[] = {
    { "help",    cmd_help,    "Show available commands" },
    { "version", cmd_version, "Show firmware version" },
    { "gpio",    cmd_gpio,    "Control GPIO: gpio <pin> <1|0>" }, /* 追加 */
    { NULL,      NULL,        NULL }                              /* 番人 (Sentinel) */
};
```
# ホストPCでの動作テスト（Linux / macOS / WSL）
実機マイコンを接続せずに、PC上でそのまま動作確認が可能です。

```Bash
# ビルド
gcc -Wall -Wextra -o ecli_demo ecli.c bsp/uart_linux.c main.c
```

# 実行
```Bash
./ecli_demo
```

---

## ライセンス
このプロジェクトはMIT-0ライセンスのもとで公開されています。詳細は[LICENSE](LICENSE)ファイルをご覧ください。
- 商用利用・個人利用を問わず、完全自由に使用できます。
- 著作権表示やライセンス文言の保持・記載義務すらありません。
- ソースコードの改変、流用、再配布、自社製品への組み込み等、制限なくご活用いただけます。

<p align="right"><small><a name="note" class="muted-link">※一部AIによる生成・調整コードが含まれることがあります。</a></small></p>
