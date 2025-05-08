

#define __KEYMAP_C__

#include <stdint.h>
#include <strings.h>
#include <math.h>
#include <sys/types.h>
#include QMK_KEYBOARD_H
#include "cqa02303v4.h"

// キーマップ定義は.hに移行
#include "keymap.h"



// -------------------------------------------------------------------------------------
// 独自処理だけどweakの関数を上書きする際、keymap.cでは _kb は使わず、 _user を使うようにする
// -------------------------------------------------------------------------------------


// マトリクス位置から現在のキーコードを得る
uint16_t matrix_to_keycode(uint8_t row, uint8_t col){
    uint8_t layer = get_highest_layer(layer_state | default_layer_state);
    keypos_t key = (keypos_t){
        .row = row,
        .col = col
    };
    // キーに登録されたキーコードを取得(透過キーの場合は追跡して確認する)
    uint16_t code = KC_TRNS;
    for(; (layer >=0) && (code == KC_TRNS); layer--){
        // BUG: offになっているレイヤは読み飛ばさないといけない
        code = keymap_key_to_keycode(layer, key);
    }
    return code;
}

// -------------------------------------------------------------------------------------
// RawHID関連
// -------------------------------------------------------------------------------------
char rawbuffer[33] = {0};
void raw_hid_receive_kb(uint8_t *data, uint8_t length){ // _userが存在しないため
    uint8_t *command_id   = &(data[0]);
    
    bzero(rawbuffer, sizeof(rawbuffer));
    bcopy(data, rawbuffer, length);
    
    // viaコマンドの場合は何もしない
    if(*command_id != 0xFF) {   // 0xFF ( id_unhandled ) が処理なしなのでそこに重ねて実装するようにする
        return;
    }
    // 独自処理
    // echo back
    // raw_hid_send(data, length);
    return;
}


extern void pointing_device_driver_set_adjust(void);

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // カスタムキーコードの処理
    switch (keycode) {
        case JOY_CLR:
            if (record->event.pressed) {
                pointing_device_driver_set_adjust();
            }
            return false;
        default:
            break;
    }
    return true;
}
