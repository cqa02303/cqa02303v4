// -------------------------------------------------------------------------------------
// エンコーダー関連
// -------------------------------------------------------------------------------------
// encoder_quadrature_handle_read(i, encoder_quadrature_read_pin(i, false), encoder_quadrature_read_pin(i, true));
// -> encoder_quadrature_handle_read()
//  -> encoder_handle_state_change()
//   -> encoder_queue_event() でキューに積まれる [quantum/encoder.c]


#include "keymap.h"

// クリック感と動作を合わせるとき(1クリック4ステップで B相が安定したものでないとダメ see. ENCODER_RESOLUTION)
#define ENCODER_DEFAULT_POS 0x0 // 現物の仕様に合わせること


// エンコーダーのキー位置を配列で定義
static const uint8_t encoder_a_rows[NUM_ENCODERS] = HFRE_A_ROWS;
static const uint8_t encoder_a_cols[NUM_ENCODERS] = HFRE_A_COLS;
static const uint8_t encoder_b_rows[NUM_ENCODERS] = HFRE_B_ROWS;
static const uint8_t encoder_b_cols[NUM_ENCODERS] = HFRE_B_COLS;

bool encoder_a_state[NUM_ENCODERS] = {0};
bool encoder_b_state[NUM_ENCODERS] = {0};

// スキャン生データを取得
extern matrix_row_t raw_matrix[MATRIX_ROWS];
inline bool raw_matrix_is_on(uint8_t row, uint8_t col) {
    return (raw_matrix[row] & ((matrix_row_t)1 << col));
}


// キーマトリクスのスキャン結果から必要分を退避
void matrix_scan_user(void) {
    // エンコーダーのピンの状態を記録
    for (uint8_t i = 0; i < NUM_ENCODERS; i++) {
        encoder_a_state[i] = raw_matrix_is_on(encoder_a_rows[i], encoder_a_cols[i]);
        encoder_b_state[i] = raw_matrix_is_on(encoder_b_rows[i], encoder_b_cols[i]);
    }
}

// もしキースキャンの中でコンデンサチャージのwaitをするなら、すべての行で実行してしまうが下記関数でできる
__attribute__((weak)) void matrix_output_select_delay(void) {
    // ここに追加
    //waitInputPinDelay();
}

// drivers/encoder/encoder_quadrature.c の中で ENCODER_A_PINS, ENCODER_B_PINS が未定義だとデフォルトの取得関数が無効(unbdef ENCODER_DEFAULT_PIN_API_IMPL)になることを活用されている
// ここから ENCODER_DEFAULT_PIN_API_IMPL 代替
// お勧めのコンデンサチャージ時間を待機してくれる便利関数 // とりあえずつぶしておく
void encoder_wait_pullup_charge(void){
    //     wait_us(100);
}

// 固定ピンのためにinput highにしてしまうからつぶしておく
void encoder_quadrature_init_pin(uint8_t index, bool pad_b){
}

// エンコーダーのピン状態を記録したテーブルを読み込んで 0 か 1 を返す
uint8_t encoder_quadrature_read_pin(uint8_t index, bool pad_b){
    uint8_t ret;
    if(pad_b){
        ret = encoder_b_state[index];
    } else {
        ret = encoder_a_state[index];
    }
    return ret;
}
// ここまで

// エンコーダーのイベント処理(デフォルトを有効にする場合 encoder_update_userを実装してindexを読む)
bool encoder_update_user(uint8_t index, bool clockwise) {
    int8_t row = clockwise ? encoder_a_rows[index] : encoder_b_rows[index];
    int8_t col = clockwise ? encoder_a_cols[index] : encoder_b_cols[index];
    tap_code_delay(matrix_to_keycode(row, col), 10);
    // RGBアニメーション開始(offで点灯する場合もあるので両方書いている)
    switch_events(row, col, true);  // 点灯させる位置を知らせるためにkeyboard.jsonでダミーのLEDを登録する
    switch_events(row, col, false);

    return false;
}