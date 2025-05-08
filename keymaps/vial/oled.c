#include "keymap.h"

// -------------------------------------------------------------------------------------
// OLED 表示処理
// -------------------------------------------------------------------------------------
#ifdef OLED_ENABLE

// OLED起動時の初期表示設定
bool startup = true;
// 基準タイマー値を保持する変数
static uint32_t timer_base = 0;

// OLED初期処理
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    startup = true;
    timer_base = timer_read32();
    // OLEDを180度回転
#if OLED_IC == OLED_IC_SH1107
    return OLED_ROTATION_270;
#else
    return OLED_ROTATION_180;
#endif
}

// OLED起動時の初期表示設定(タイマを確認して表示内容を変えてリターンする)
void oled_startup_sequence(void) {
    int32_t timer = timer_elapsed32(timer_base);
    // 表示更新が頻繁過ぎると間に合わないので少し間隔をあける
    if(timer % 10 != 0) {
        return;
    }

    // 最初はブランク
    if (timer < OLED_STARTUP_DELAY_MS) {
        return;
    }

    // Memoryだけ少し表示
    if (timer < OLED_STARTUP_DELAY_MS + OLED_COUNTUP_DELAY_MS - 10) {
        oled_write_P("Memory ", false);
        return;
    }

    // "Memory xxxxx MB OK" の表示
    // 起動1秒後から2秒間、0.1秒ごとにカウントアップ
    if(timer <= OLED_STARTUP_DELAY_MS + OLED_COUNTUP_DELAY_MS + OLED_COUNTUP_MAX_KB) {
        // 表示する数字
        int size = timer - OLED_STARTUP_DELAY_MS - OLED_COUNTUP_DELAY_MS;
        size = size < 0 ? 0 : size;
        oled_write_P("Memory ", false);
        // xxxx部分をカウントアップ
        char buffer[16];
        snprintf(buffer, sizeof(buffer), "%d", size);
        oled_write_P(buffer, false);
        oled_write_P(" KB OK", false);
        return;
    }

    // カウントアップ終了後は少しそのまま
    if(timer <= OLED_STARTUP_DELAY_MS + OLED_COUNTUP_DELAY_MS + OLED_COUNTUP_MAX_KB + OLED_SCREEN_HOLD_DELAY_MS) {
        oled_write_P("Memory ", false);
        char buffer[16];
        snprintf(buffer, sizeof(buffer), "%d", OLED_COUNTUP_MAX_KB);
        oled_write_P(buffer, false);
        oled_write_P(" KB OK", false);
        return;
    }

    // カウントアップ終了後0.5秒以降は画面を暗転
    if(timer < OLED_STARTUP_DELAY_MS + OLED_COUNTUP_DELAY_MS + OLED_COUNTUP_MAX_KB + OLED_SCREEN_HOLD_DELAY_MS + OLED_SCREEN_OFF_DELAY_MS) {
        oled_clear();
        return;
    }

    // 処理が終わったら通常処理に移行
    startup = false;
}


// 外部の表示データ
extern bool encoder_a_state[NUM_ENCODERS];
extern bool encoder_b_state[NUM_ENCODERS];
extern char rawbuffer[33];
extern int16_t positions[4]; // アナログジョイスティックの状態を追跡する変数
extern int16_t coordinates[4]; // アナログジョイスティックの状態を追跡する変数
extern uint8_t vmatrix;
extern char extstr[10];
extern matrix_row_t raw_matrix[MATRIX_ROWS];

// 表示関数
bool oled_task_user(void) {
    // OLEDの起動時にのみ実行
    if (startup) {
        oled_startup_sequence();
        return false;
    }

    // 表示更新が頻繁過ぎると間に合わないので少し間隔をあける
    if(timer_elapsed32(timer_base) % 100 != 0) {
        return false;
    }

    // 通常処理
    // Host Keyboard Layer Status
    oled_write_P(PSTR("Layer: "), false);

    switch (get_highest_layer(layer_state | default_layer_state)) {
        case _BASE:
            oled_write_P(PSTR("L0 "), false);
            break;
        case _L1:
            oled_write_P(PSTR("L1 "), false);
            break;
        case _L2:
            oled_write_P(PSTR("L2 "), false);
            break;
        default:
            // Or use the write_ln shortcut over adding '\n' to the end of your string
            oled_write_ln_P(PSTR("?? "), false);
    }
    // Host Keyboard LED Status
    led_t led_state = host_keyboard_led_state();
    oled_write_P(led_state.num_lock ? PSTR("NL ") : PSTR("  "), false);
    oled_write_P(led_state.caps_lock ? PSTR("CL ") : PSTR("  "), false);
    oled_write_ln_P(led_state.scroll_lock ? PSTR("SL") : PSTR(" "), false);
    
    // ジョイスティックの値を表示
    /* 
    oled_write_P(PSTR("JOYX:"), false);
    oled_write_P(get_u16_str(coordinates[0], ' '), false);
    oled_write_P(PSTR(" Y:"), false);
    oled_write_P(get_u16_str(coordinates[1], ' '), false);
    oled_write_P(PSTR(" H:"), false);
    oled_write_P(get_u16_str(coordinates[2], ' '), false);
    oled_write_P(PSTR(" V:"), false);
    oled_write_ln(get_u16_str(coordinates[3], ' '), false);
    */
    int num;
    oled_write_P(PSTR("JOYX:"), false);
    num = coordinates[0]; if(num < 0){ oled_write_P(PSTR("-"), false); num =  -num; }
    oled_write_P(get_u8_str(num, ' '), false);
    oled_write_P(PSTR(" Y:"), false);
    num = coordinates[1]; if(num < 0){ oled_write_P(PSTR("-"), false); num =  -num; }
    oled_write_ln(get_u8_str(num, ' '), false);
    oled_write_P(PSTR(" H:"), false);
    num = coordinates[2]; if(num < 0){ oled_write_P(PSTR("-"), false); num =  -num; }
    oled_write_P(get_u8_str(num, ' '), false);
    oled_write_P(PSTR(" V:"), false);
    num = coordinates[3]; if(num < 0){ oled_write_P(PSTR("-"), false); num =  -num; }
    oled_write_ln(get_u8_str(num, ' '), false);
    //
    oled_write_ln(PSTR(extstr), false); // スティックの入力結果表示


    // 仮想ロータリーエンコーダーのステータス表示
    uint8_t encoder_a_rows[NUM_ENCODERS] = HFRE_A_ROWS;
    uint8_t encoder_a_cols[NUM_ENCODERS] = HFRE_A_COLS;
    uint16_t keycode = matrix_to_keycode(encoder_a_rows[0], encoder_a_cols[0]);
    oled_write_P(PSTR("ROL"), false);
    if((keycode >= QK_MOUSE_WHEEL_UP) && (keycode <= QK_MOUSE_WHEEL_RIGHT)){
        oled_write_P(PSTR("[Wheel]:"), false);
    }else {
        oled_write_P(PSTR("[Cursor]:"), false);

    }
    for (uint8_t i = 0; i < NUM_ENCODERS; i++) {
        oled_write_P(encoder_a_state[i] ? PSTR(" A") : PSTR("  "), false);
        oled_write_P(encoder_b_state[i] ? PSTR("B") : PSTR(" "), false);
    }

    /*
    oled_set_cursor(0, 4); // [x,y]文字カーソル位置指定
    oled_write_raw_P((char*)&vmatrix, sizeof(vmatrix));

    oled_set_cursor(1, 4); // [x,y]文字カーソル位置指定
    oled_write_raw_P((char*)raw_matrix, sizeof(raw_matrix));

    
    // Raw HID 受信データを表示
    oled_set_cursor(1, 5); // [x,y]文字カーソル位置指定
    oled_write_raw_P(rawbuffer, sizeof(rawbuffer));
    */

    return false;
}

#endif