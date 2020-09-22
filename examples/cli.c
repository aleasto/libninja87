#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include "ninja87/lights.h"

void print_usage(char*);
int parse_color_string(char*, enum ColorType*, char*, enum RainbowMode*);

static char* short_options = "s:b:d:c:";
static struct option long_options[] = {
    {"speed",      required_argument, 0,  's' },
    {"brightness", required_argument, 0,  'b' },
    {"direction",  required_argument, 0,  'd' },
    {"color",      required_argument, 0,  'c' },
    {0,            0,                 0,  0   }
};

int main(int argc, char* argv[]) {
    libusb_device_handle* handle = open_ninja87();
    if (!handle)
        return 1;

    // At least 2 args, SOURCE and EFFECT
    if (argc < 3 || KEYNOTFOUND == enum_lookup(argv[1], LightSource_dictionary, NSOURCE)) {
        print_usage(argv[0]);
        return 0;
    }

    struct light_state state;
    // First option MUST be SOURCE
    enum LightSource source = enum_lookup(argv[1], LightSource_dictionary, NSOURCE);
    argv[1] = argv[0]; // hack for getopt(argc - 1, &argv[1], ...)
    light_state_init(&state, source);

    // Set some defaults, even if redundant
    light_state_set_color(&state, COLOR_PRIMARY, "#000000", RAINBOW_OFF);
    light_state_set_color(&state, COLOR_SECONDARY, "#000000", RAINBOW_OFF);
    light_state_set_brightness(&state, 6);
    light_state_set_speed(&state, 2);
    light_state_set_direction(&state, DIR_RIGHT);

    // Parse options
    int opt;
    int long_index = 0;
    while((opt = getopt_long(argc - 1, &argv[1],
                 short_options, long_options, &long_index)) != -1) {
        switch(opt) {
            case 's': {
                int speed = atoi(optarg);
                light_state_set_speed(&state, speed);
                break;
            }
            case 'b': {
                int brightness = atoi(optarg);
                light_state_set_brightness(&state, brightness);
                break;
            }
            case 'd': {
                enum LightDirection direction = enum_lookup(optarg, LightDirection_dictionary, NDIRECTION);
                light_state_set_direction(&state, direction);
                break;
            }
            case 'c': {
                enum ColorType color_type;
                char* hex = malloc(7 * sizeof(char));
                enum RainbowMode rainbow;
                if (parse_color_string(optarg, &color_type, hex, &rainbow) == 0) {
                    light_state_set_color(&state, color_type, hex, rainbow);
                    free(hex);
                    break;
                }
                free(hex);
                // fall through
            }
            default:
                print_usage(argv[0]);
                return 0;
        }
    }

    // Extra option is EFFECT (position indipendent)
    optind++; // We're skipping the first argv
    if (optind < argc) {
        uint8_t effect;
        if (state.source == SRC_BACKLIGHT)
            effect = enum_lookup(argv[optind], BacklightEffect_dictionary, NBACKEFFECT);
        else
            effect = enum_lookup(argv[optind], SidelightEffect_dictionary, NSIDEEFFECT);

        if (effect == EFFECT_OFF)
            light_state_off(&state);
        else
            light_state_on(&state, effect);
    } else {
        print_usage(argv[0]);
        return 0;
    }

    apply_state(handle, &state);

    return 0;
}

// Returns 0 if ok
//        -1 if missing type or color
int parse_color_string(char* input, enum ColorType* type, char* color, enum RainbowMode* rainbow) {
    color[0] = '\0';
    *type = KEYNOTFOUND;
    *rainbow = RAINBOW_OFF;

    char* token = strtok(input, ",");
    while (token != NULL) {
        if (!strcmp(token, "rainbow")) {
            *rainbow = RAINBOW_ON;
        } else if (token[0] == '#') {
            strncpy(color, token, 7);
        } else {
            if(*type == KEYNOTFOUND)
                *type = enum_lookup(optarg, ColorType_dictionary, NCOLOR);
        }

        token = strtok(NULL, ",");
    }

    if (color[0] != '\0' && *type != KEYNOTFOUND)
        return 0;
    return -1;
}

void print_usage(char* program_name) {
    printf("\nUsage:\n");
    printf("%s {backlight,sidelight} <effect>\n"
        "   [--color primary,#ffffff[,rainbow]]\n"
        "   [--color secondary,#ffffff[,rainbow]]\n"
        "   [--brightness {0..6}]\n"
        "   [--speed {0..4}]\n"
        "   [--direction {right,left,up,down}]\n"
        , program_name);
}
