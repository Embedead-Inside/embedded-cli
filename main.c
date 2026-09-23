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
