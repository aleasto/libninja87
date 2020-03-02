#include <unistd.h>
#include "ninja87/lights.h"

int main(int argc, char* argv[]) {
    struct light_state backlight_state;
    light_state_init(&backlight_state, SRC_SIDELIGHT); // defaults to off

    light_state_on(&backlight_state, BACK_STREAMER);
    light_state_set_color(&backlight_state, COLOR_PRIMARY, "#000000", RAINBOW_OFF);
    light_state_set_color(&backlight_state, COLOR_SECONDARY, "#FFFFFF", RAINBOW_ON);
    light_state_set_speed(&backlight_state, 2);
    light_state_set_brightness(&backlight_state, 4);
    apply_light_state(&backlight_state);

    sleep(2);

    light_state_off(&backlight_state);
    apply_light_state(&backlight_state);

    return 0;
}
