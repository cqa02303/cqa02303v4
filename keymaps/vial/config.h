#pragma once

// https://www.keyboard-layout-editor.com/##@@=Esc&=!%0A1&=%2F@%0A2&=%23%0A3&=$%0A4&=%25%0A5&=%5E%0A6&=%2F&%0A7&=*%0A8&=(%0A9&=)%0A0&=%2F_%0A-&=+%0A%2F=&=BS&=Delete%3B&@_c=%239ef590&w:1.5%3B&=Tab&_c=%23cccccc%3B&=Q&=W&=E&=R&=T&=Y&=U&=I&=O&=P&=%7B%0A%5B&=%7D%0A%5D&_c=%239ef590&w:1.5%3B&=%7C%0A%5C%3B&@_c=%23bdc0ff&w:1.75%3B&=Ctrl&_c=%23cccccc%3B&=A&=S&=D&=F&=G&=H&=J&=K&=L&=%2F:%0A%2F%3B&=%22%0A&=ENTER&_c=%23ffa1a1&w:1.25%3B&=Ctrl+Enter%3B&@_c=%23c8b5ff&w:2.25%3B&=SHIFT&_c=%23cccccc%3B&=Z&=X&=C&=V&=B&=N&=M&=%3C%0A,&=%3E%0A.&=%3F%0A%2F%2F&=~%0A%60&_c=%23bdc0ff&w:1.75%3B&=SHIFT%3B&@_x:1.25&c=%23cccccc%3B&=Alt&_c=%23ffa1a1&w:1.25%3B&=PrSc%0A%0A%0A%0A%0A%0AOLED&_w:1.25%3B&=Fn2%0A%0A%0A%0A%0A%0AOLED&_c=%23cccccc%3B&=Ctrl&_c=%23f3f590&a:7&w:2%3B&=&_c=%23ffa1a1&a:4&w:1.25%3B&=Enter&_c=%23cccccc%3B&=BS&=Fn%3B&@_x:1%3B&=5,9&_x:9%3B&=0,4&=0,5&_x:1%3B&=0,9%3B&@=6,9&=5,6&=7,9&_x:8&w:0.5%3B&=1,6&_w:0.5%3B&=0,6&_w:0.5%3B&=1,7&_w:0.5%3B&=0,7&=1,9&=0,8&=2,9%3B&@_x:1%3B&=8,9&_x:12%3B&=3,9


// キーマップレイヤの最大数を増やす
#define DYNAMIC_KEYMAP_LAYER_COUNT 8

// VIAL設定
#define VIAL_UNLOCK_COMBO_ROWS {9, 9}
#define VIAL_UNLOCK_COMBO_COLS {7, 6}
#define VIAL_KEYBOARD_UID {0x23, 0x76, 0xE7, 0x0B, 0xCC, 0xC8, 0x64, 0x36}

// LED最大輝度を制限（0〜255の範囲で設定）
//#define RGBLIGHT_LIMIT_VAL 120

// エンコーダーの総数を定義
#define NUM_ENCODERS 2
// 仮想エンコーダー用に調整
#define ENCODER_RESOLUTION 4
//#define ENCODER_RESOLUTION 2
//#define MAX_QUEUED_ENCODER_EVENTS 4
// 独自実装のエンコーダーマトリクス定義
#define HFRE_A_ROWS {1,1}
#define HFRE_A_COLS {7,6}
#define HFRE_B_ROWS {0,0}
#define HFRE_B_COLS {7,6}

// 独自実装のジョイスティックキー入力化定義
#define HFJS_STKS 4 // ジョイスティック1本でアナログ抵抗（x,y）２本必要
#define HFJS_ANALOG_PINS {ANALOG_JOYSTICK_X_AXIS_PIN, ANALOG_JOYSTICK_Y_AXIS_PIN, ANALOG_JOYSTICK_H_AXIS_PIN, ANALOG_JOYSTICK_V_AXIS_PIN}
#define HFJS_INVERTS {-1, 1, -1, 1} // アナログ抵抗ごとに正値、反転値の設定
#define HFJS_ROWS {7,6,5,8, 2,1,0,3} // 右左上下の順でマトリクスを登録する
#define HFJS_COLS {9,9,9,9, 9,9,9,9} // 右左上下の順でマトリクスを登録する
#define HFJS_ACTION     65  // スイッチをONにする境界値
#define HFJS_RELEASE    30  // スイッチをOFFにする境界値
#define HFJS_DBLACT     0.6 // 斜めにスイッチを入れるとき２個目の境界値を下げる 0.7のとき65-> 45.5 ( 1/sqr(2) )
#define ANALOG_WHEEL_SPEED_MAX 2 // ホイールの最大値

// OLED向けのハードウェア設定
#define HAL_USE_I2C TRUE
#define I2C1_SCL_PIN GP15
#define I2C1_SDA_PIN GP14

// OLEDデバイス設定
#define OLED_IC OLED_IC_SH1107
#define OLED_DISPLAY_64X128

// OLED起動時の初期表示設定
#define OLED_STARTUP_DELAY_MS 1000
#define OLED_COUNTUP_DELAY_MS 500
#define OLED_COUNTUP_MAX_KB 2000
#define OLED_SCREEN_HOLD_DELAY_MS 3000
#define OLED_SCREEN_OFF_DELAY_MS 500

