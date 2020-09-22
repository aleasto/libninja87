#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "libusb_helper.h"
#include "ninja87/lights.h"

#define BULK_EP_OUT 0x03
#define INTERFACE 1

void light_state_init(struct light_state* state, enum LightSource source) {
    memset(state, 0, 32);

    memcpy(state->magic, (uint8_t[2]){ 0x06, 0xBE }, 2);
    state->source = source;
    state->transaction = STATE_OFF;
    state->on_bit = 0x0;
    state->off_bit = 0x01;
    state->effect = 0xAF;
    state->speed = 0x0;
    state->brightness = 0x0;
}

void light_state_off(struct light_state* state) {
    state->transaction = STATE_OFF;
    state->on_bit = 0x0;
    state->off_bit = 0x01;
    state->effect = 0xAF;
    state->speed = 0x0;
    state->brightness = 0x0;
    memcpy(state->color1, (uint8_t[3]){0}, 3);
    memcpy(state->color2, (uint8_t[3]){0}, 3);
}

void light_state_on(struct light_state* state, uint8_t effect) {
    state->transaction = STATE_ON;
    state->on_bit = 0x01;
    state->off_bit = 0x0;
    state->effect = effect;
}

void light_state_set_color(struct light_state* state, enum ColorType type, char* color, enum RainbowMode rainbow) {
    int r, g, b;
    sscanf(color, "#%02x%02x%02x", &r, &g, &b);

    if (type == COLOR_PRIMARY) {
        memcpy(state->color1, (uint8_t[3]){ r, g, b }, 3);
        state->color1_rainbow = rainbow;
    } else if (type == COLOR_SECONDARY) {
        memcpy(state->color2, (uint8_t[3]){ r, g, b }, 3);
        state->color2_rainbow = rainbow;
    } else {
        fprintf(stderr, "Unrecognised color type. Available are: COLOR_PRIMARY, COLOR_SECONDARY\n");
    }
}

void light_state_set_speed(struct light_state* state, uint8_t speed) {
    if (speed > 4)
        speed = 4;
    state->speed = speed;
}

void light_state_set_brightness(struct light_state* state, uint8_t brightness) {
    if (brightness > 6)
        brightness = 6;
    state->brightness = brightness;
}

void light_state_set_direction(struct light_state* state, uint8_t direction) {
    state->direction = direction;
}

void apply_state(libusb_device_handle* handle, struct light_state* state) {
    // TODO: CHECK IF VALID
    libusb_bulk_transfer(handle, BULK_EP_OUT, (uint8_t*)state, 32, NULL, 1000);
}

libusb_device_handle* open_ninja87() {
    libusb_device_handle* handle = usb_open("0416:a0f8", INTERFACE);
    if (handle == NULL)
        fprintf(stderr, "Error opening ninja87\n");
    return handle;
}
