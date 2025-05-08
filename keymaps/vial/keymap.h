// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include QMK_KEYBOARD_H

enum keymap_layers {
    _BASE,
    _L1,
    _L2,
    _L3
};

#ifdef __KEYMAP_C__
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(
        KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,     KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS,  KC_EQL,  KC_BSPC,    KC_DEL,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,     KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC,  KC_RBRC, KC_BSLS,
        KC_LCTL, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,     KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,  KC_ENT,  C(KC_ENT),
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,     KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,  KC_GRV,  KC_RSFT,
        KC_LALT, KC_PSCR, TG(_L1), KC_LALT, KC_LCTL, KC_SPACE,KC_ENT,   KC_BSPC, MO(_L2), MO(_L3),          MO(_L3),                        KC_UP,
        KC_TAB, KC_LWIN, KC_LCTL,                                                      KC_UP, KC_DOWN, KC_LEFT, KC_RIGHT,        KC_LEFT, JOY_CLR, KC_RIGHT,
                 KC_LSFT,                                                                                                                 KC_DOWN
    ),
    [_L1] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
          MS_UP, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,                         MS_WHLU,
        MS_LEFT, KC_LWIN, MS_RGHT,                                              MS_WHLU, MS_WHLD, MS_WHLL, MS_WHLR,       MS_WHLR, _______, MS_WHLL, 
                 MS_DOWN,                                                                                                          MS_WHLD
    ),
    [_L2] = LAYOUT(
        _______,   KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,  KC_F10,  KC_F11,  KC_F12, MS_BTN1,  MS_BTN2,
        _______, KC_MPRV, KC_MPLY, KC_MNXT, _______, _______, _______, _______, KC_PSCR, KC_SCRL, KC_PAUS,  KC_F12, _______, _______,
        _______, KC_MUTE, KC_VOLD, KC_VOLU, _______, _______, _______, KC_HOME,  KC_END, MS_WHLD, MS_WHLU, _______, _______, KC_PGUP,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, MS_BTN1, MS_BTN2, KC_PGUP, KC_PGDN,
        KC_VOLU, _______, _______, _______, _______, _______, JOY_CLR, TG(_L1), _______, _______, _______,                         _______,
        KC_MPRV, KC_MPLY, KC_MNXT,                                              _______, _______, _______, _______,       _______, _______, _______, 
                 KC_VOLD,                                                                                                          _______
    ),
   [_L3] = LAYOUT(
       _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
       _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
       _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
       _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
       _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,                         _______,
       _______, _______, _______,                                              _______, _______, _______, _______,       _______, _______, _______, 
                _______,                                                                                                          _______
)
};
#endif

// -------------------------------------------------------------------------------------
// RGB関連
// -------------------------------------------------------------------------------------
// RGBアニメーション開始処理(keypressのon/offはセットじゃなくてもよい)
inline void switch_events(uint8_t row, uint8_t col, bool pressed) {
#if defined(LED_MATRIX_ENABLE)
    led_matrix_handle_key_event(row, col, pressed);
#endif
#if defined(RGB_MATRIX_ENABLE)
    rgb_matrix_handle_key_event(row, col, pressed);
#endif
}

// キーコードを調べる
uint16_t matrix_to_keycode(uint8_t row, uint8_t col);

