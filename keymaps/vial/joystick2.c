#include "keymap.h"
#include "pointing_device.h"
#include "analog.h"
#include "gpio.h"
#include "wait.h"
#include "timer.h"
#include <stdint.h>
#include <stdlib.h>
#include "pointing_device_internal.h"

uint16_t minAxisValue = ANALOG_JOYSTICK_AXIS_MIN;
uint16_t maxAxisValue = ANALOG_JOYSTICK_AXIS_MAX;
uint8_t maxCursorSpeed = ANALOG_JOYSTICK_SPEED_MAX;
uint8_t maxWheelSpeed  = ANALOG_WHEEL_SPEED_MAX;
uint8_t speedRegulator = ANALOG_JOYSTICK_SPEED_REGULATOR; // Lower Values Create Faster Movement


// アナログピンの値を取得する
int16_t axisCoordinate(int16_t position, uint16_t origin, int8_t axis) {
    int8_t  direction;
    int16_t distanceFromOrigin;
    int16_t range;


    if (origin == position) {
        return 0;
    } else if (origin > position) {
        distanceFromOrigin = origin - position;
#ifdef ANALOG_JOYSTICK_AUTO_AXIS
        if (position < minAxisValues[axis]) {
            minAxisValues[axis] = position;
        }
        range = origin - minAxisValues[axis];
#else
        range = origin - minAxisValue;
#endif
        direction = -1;
    } else {
        distanceFromOrigin = position - origin;
#ifdef ANALOG_JOYSTICK_AUTO_AXIS
        if (position > maxAxisValues[axis]) {
            maxAxisValues[axis] = position;
        }
        range = maxAxisValues[axis] - origin;
#else
        range = maxAxisValue - origin;
#endif
        direction = 1;
    }

    float   percent    = (float)distanceFromOrigin / range;
    int16_t coordinate = (int16_t)(percent * 100);
    if (coordinate < 0) {
        return 0;
    } else if (coordinate > 100) {
        return 100 * direction;
    } else {
        return coordinate * direction;
    }
}

// 取得値からマウスの移動量を計算する
int8_t axisToMouseComponent(int16_t coordinate, int16_t origin, uint8_t maxSpeed, int8_t *weights) {
    int8_t  result = 0;
#ifndef ANALOG_JOYSTICK_WEIGHTS
    if (coordinate != 0) {
        float percent = (float)coordinate / 100;
        result        = percent * maxSpeed * (abs(coordinate) / speedRegulator);
    } else {s
        return 0;
    }
#else
    result = weights[abs(coordinate)] * (coordinate < 0 ? -1 : 1) * maxSpeed / speedRegulator;
#endif

    return result;
}

char extstr[10] = {0};

static int8_t weightsCursor[101] = ANALOG_JOYSTICK_WEIGHTS;
static int8_t weightsWheel[101] = ANALOG_JOYSTICK_WEIGHTS2;

static pin_t adpins[HFJS_STKS] = HFJS_ANALOG_PINS;
int16_t origins[HFJS_STKS] = {0};
uint8_t vmatrix = 0;
int16_t positions[HFJS_STKS]; // アナログジョイスティックの状態を追跡する変数
int16_t coordinates[HFJS_STKS]; // アナログジョイスティックの状態を追跡する変数

// 値を取得する・キー入力の場合はキーイベントを発生させる
report_mouse_t analog_joystick_read(void) {
    static uint8_t rows[HFJS_STKS * 2] = HFJS_ROWS; // {6,7,5,8, 1,2,0,3}
    static uint8_t cols[HFJS_STKS * 2] = HFJS_COLS; // {9,9,9,9, 9,9,9,9}
    static int8_t inverts[HFJS_STKS] = HFJS_INVERTS;

    report_mouse_t report = {0};
    uint16_t keycode;
    int16_t position;
    int8_t coordinate;
    int num;
    int flg;
    int8_t distance;

    for (int i = 0; i < HFJS_STKS; i++){
        position = analogReadPin(adpins[i]);
        coordinate = inverts[i] * axisCoordinate(position, origins[i], 0);
        positions[i] = position;
        coordinates[i] = coordinate;
        for (int n = 0; n < 2; n++){
            distance = (n ? -1 : 1) * coordinate;
            num = i * 2 + n;
            flg = 0x1 << (num);                 // vmatrix のビットマップへの割り当て(BUG: 押している最中のレイヤ切替でリリースしていない)
            keycode = matrix_to_keycode(rows[num], cols[num]);
            switch(keycode){
                case MS_DOWN: // +y
                            extstr[num] = 'm';
                            if(distance > 0){
                                report.y -= axisToMouseComponent(coordinate, origins[i], maxCursorSpeed, weightsCursor); 
                                extstr[num] = 'D';
                            }
                            break;
                case MS_UP:  // -y
                            extstr[num] = 'm';
                            if(distance > 0){
                                report.y -= axisToMouseComponent(coordinate, origins[i], maxCursorSpeed, weightsCursor); 
                                extstr[num] = 'U';
                            }
                            break;
                case MS_RGHT:  // +x
                            extstr[num] = 'm';
                            if(distance > 0){
                                report.x += axisToMouseComponent(coordinate, origins[i], maxCursorSpeed, weightsCursor);
                                extstr[num] = 'R';
                            }
                            break;
                case MS_LEFT:  // -x
                            extstr[num] = 'm';
                            if(distance > 0){
                                report.x += axisToMouseComponent(coordinate, origins[i], maxCursorSpeed, weightsCursor);
                                extstr[num] = 'L';
                            }
                            break;
                case MS_WHLD:
                            extstr[num] = 'w';
                            if(distance > 0){
                                report.v += axisToMouseComponent(coordinate, origins[i], maxWheelSpeed, weightsWheel); 
                                extstr[num] = 'd';
                            }
                            break;
                case MS_WHLU:
                            extstr[num] = 'w';
                            if(distance > 0){
                                report.v += axisToMouseComponent(coordinate, origins[i], maxWheelSpeed, weightsWheel); 
                                extstr[num] = 'u';
                            }
                            break;
                case MS_WHLR:
                            extstr[num] = 'w';
                            if(distance > 0){
                                report.h += axisToMouseComponent(coordinate, origins[i], maxWheelSpeed, weightsWheel);
                                extstr[num] = 'r';
                            }
                            break;
                case MS_WHLL:
                            extstr[num] = 'w';
                            if(distance > 0){
                                report.h += axisToMouseComponent(coordinate, origins[i], maxWheelSpeed, weightsWheel);
                                extstr[num] = 'l';
                            }
                            break;
                default:
                            // キー入力として処理する
                            if(vmatrix & flg){         // 前回押されていた時
                                if(distance < HFJS_RELEASE) {  // 下限以下ならリリース
                                    vmatrix &= ~flg;
                                    action_exec(MAKE_KEYEVENT(rows[num], cols[num], false));
                                    switch_events(rows[num], cols[num], false); // 点灯させる位置を知らせるためにkeyboard.jsonでダミーのLEDを登録する
                                    extstr[num] = 'c';
                                }
                            }else {                     // 未だ押されていない場合
                                if(distance > HFJS_ACTION * (vmatrix ? HFJS_DBLACT : 1) ){      // 上限を超えたらプッシュ (斜め入力の判定を緩和する)
                                    vmatrix |= flg;
                                    action_exec(MAKE_KEYEVENT(rows[num], cols[num], true));
                                    switch_events(rows[num], cols[num], true);
                                    extstr[num] = 'P';
                                }
                            }
            }
        }
    }
    return report;
}

// ポジションリセット
void pointing_device_driver_set_adjust(void){
    for (int i = 0; i < HFJS_STKS; i++){
        origins[i] = analogReadPin(adpins[i]);
    }
    for(int i = 0; i < 8; i++){
        extstr[i] = ' ';
    }
}

// アナログピンの初期化
void pointing_device_driver_init(void) {
    // GPIO初期化
    for (int i = 0; i < HFJS_STKS; i++){
        gpio_set_pin_input_high(adpins[i]);
    }
    // GPIO初期化した後少し待ってから初期値を得るようにする
    wait_us(10);
    pointing_device_driver_set_adjust();
}

uint16_t lastCursor = 0;                    // 最終取得時刻
// ここでポインティングデバイスの状態を取得する
report_mouse_t pointing_device_driver_get_report(report_mouse_t mouse_report) {
    if (timer_elapsed(lastCursor) > ANALOG_JOYSTICK_READ_INTERVAL) {
        lastCursor = timer_read();

        report_mouse_t data = analog_joystick_read();

        pd_dprintf("Raw ] X: %d, Y: %d, H: %d, V: %d\n", data.x, data.y, data.h, data.v);
    
        mouse_report.x = data.x;
        mouse_report.y = data.y;
        mouse_report.h = data.h;
        mouse_report.v = data.v;
        mouse_report.buttons = pointing_device_handle_buttons(mouse_report.buttons, data.buttons, POINTING_DEVICE_BUTTON1);
        }
    return mouse_report;
}

// ここでCPIを取得する
uint16_t pointing_device_driver_get_cpi(void) {
    return 0;
}

// ここでCPIを設定する
void pointing_device_driver_set_cpi(uint16_t cpi) {
}

//---------------------------------------------------------------------------------------------------------------------------------
static report_mouse_t last_mouse_report = {0};
// アナログジョイスティックの値を取得した際の処理
report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    // ドリフト対策
    // アナログジョイスティックの値を取得する際、スティックの状態変化を確認して３秒以上変化が無ければリセットする
    static uint32_t pointing_device_changed = 0;
    int32_t timer = timer_elapsed32(pointing_device_changed);

    // TODO:この判定が結構厳しい
    if(!(mouse_report.x == 0 && mouse_report.y == 0)) { // レポート間隔のはざまは0,0になるので読み飛ばす
        if(memcmp(&mouse_report, &last_mouse_report, sizeof(report_mouse_t)) == 0) { // 変化が無かったら
            if (timer > 3000) { // 3秒越えたら
                pointing_device_driver_set_adjust(); // 基準値をリセット
                pointing_device_changed = timer_read32();
            }
            //if(abs(mouse_report.x) < 5 && abs(mouse_report.y) < 5) { // スティック倒しきる状態を許容するか？→デバイス未接続はリセットしたい
                // → マウス系じゃないキーコードを割り当てて置けばポインタは反応しない
            //}
        }else {
            pointing_device_changed = timer_read32();
        }
        last_mouse_report = mouse_report;
    }

    // 
    return mouse_report;
}

