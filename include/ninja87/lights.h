#include <stdint.h>
#include <string.h>
#include <libusb-1.0/libusb.h>

typedef struct { char* key; uint8_t value; } kvp_t;

enum LightSource { SRC_BACKLIGHT = 0x01, SRC_SIDELIGHT = 0x02 };
kvp_t LightSource_dictionary[] = {
    { "backlight", SRC_BACKLIGHT }, { "sidelight", SRC_SIDELIGHT },
};
#define NSOURCE (sizeof(LightSource_dictionary)/sizeof(kvp_t))

enum Transaction { STATE_ON = 0x15, STATE_OFF = 0x17 };
kvp_t Transaction_dictionary[] = {
    { "on", STATE_ON }, { "off", STATE_OFF },
};
#define NSTATE (sizeof(LightState_dictionary)/sizeof(kvp_t))

enum ColorType { COLOR_PRIMARY = 0, COLOR_SECONDARY = 1 };
kvp_t ColorType_dictionary[] = {
    { "primary", COLOR_PRIMARY }, { "secondary", COLOR_SECONDARY },
};
#define NCOLOR (sizeof(ColorType_dictionary)/sizeof(kvp_t))

enum RainbowMode { RAINBOW_OFF = 0x0, RAINBOW_ON = 0x01 };
kvp_t RainbowMode_dictionary[] = {
    { "off", RAINBOW_OFF }, { "on", RAINBOW_ON },
};
#define NRAINBOW (sizeof(RainbowMode_dictionary)/sizeof(kvp_t))

enum BacklightEffect {
    BACK_STREAMER = 0x01, BACK_REACTIVE = 0x02, BACK_WATERDROP = 0x03,
    BACK_CROSS = 0x04, BACK_AURORA = 0x05, BACK_HEATMAP = 0x06,
    BACK_RIPPLE = 0x07, BACK_BREATHING = 0x08, BACK_HORSE_RACE_LAMP = 0x09,
    BACK_TWINKLING_STARS = 0x0A, BACK_STATIC = 0x0B,
    BACK_RADAR = 0x0C, BACK_RAIN = 0x0D, EFFECT_OFF = 0xAF, /* same for sidelight */
};
kvp_t BacklightEffect_dictionary[] = {
    { "streamer", BACK_STREAMER }, { "reactive", BACK_REACTIVE },
    { "waterdrop", BACK_WATERDROP }, { "cross", BACK_CROSS },
    { "aurora", BACK_AURORA }, { "heatmap", BACK_HEATMAP },
    { "ripple", BACK_RIPPLE }, { "breathing", BACK_BREATHING },
    { "horse_race_lamp", BACK_HORSE_RACE_LAMP }, { "twinkling_stars", BACK_TWINKLING_STARS },
    { "static", BACK_STATIC }, { "radar", BACK_RADAR },
    { "rain", BACK_RAIN }, { "off", EFFECT_OFF },
};
#define NBACKEFFECT (sizeof(BacklightEffect_dictionary)/sizeof(kvp_t))

enum SidelightEffect {
    SIDE_STREAMER = 0x01, SIDE_STATIC = 0x02, SIDE_BREATHING = 0x03,
    SIDE_FULL_COLOR = 0x04, /* EFFECT_OFF = 0xAF */
};
kvp_t SidelightEffect_dictionary[] = {
    { "streamer", SIDE_STREAMER }, { "static", SIDE_STATIC },
    { "breathing", SIDE_BREATHING}, { "full_color", SIDE_FULL_COLOR },
    { "off", EFFECT_OFF },
};
#define NSIDEEFFECT (sizeof(SidelightEffect_dictionary)/sizeof(kvp_t))

enum LightDirection { DIR_RIGHT = 0x0, DIR_LEFT = 0x1 };
kvp_t LightDirection_dictionary[] = {
    { "right", DIR_RIGHT }, { "down", DIR_RIGHT },
    { "left", DIR_LEFT }, { "up", DIR_LEFT },
};
#define NDIRECTION (sizeof(LightDirection_dictionary)/sizeof(kvp_t))

#define KEYNOTFOUND UINT8_MAX
uint8_t enum_lookup(char* key, kvp_t dictionary[], size_t len) {
    for (int i = 0; i < len; i++) {
        kvp_t* kvp = &dictionary[i];
        if (!strcmp(kvp->key, key))
            return kvp->value;
    }
    return KEYNOTFOUND;
}

struct light_state {
    uint8_t magic[2];
    uint8_t transaction;
    uint8_t pad;
    uint8_t source;
    uint8_t on_bit;
    uint8_t effect;
    uint8_t speed;
    uint8_t brightness;
    uint8_t direction;
    uint8_t padd[2];
    uint8_t color1[3];
    uint8_t color1_rainbow;
    uint8_t color2[3];
    uint8_t color2_rainbow;
    uint8_t padddd[8];
    uint8_t off_bit;
    uint8_t paddddd[3];
} __attribute__((packed));

void light_state_init(struct light_state*, enum LightSource);
void light_state_off(struct light_state*);
void light_state_on(struct light_state*, uint8_t effect /* backlight or sidelight effect enum */);
void light_state_set_color(struct light_state*, enum ColorType, char* color, enum RainbowMode);
void light_state_set_speed(struct light_state*, uint8_t speed);
void light_state_set_brightness(struct light_state*, uint8_t brightness);
void light_state_set_direction(struct light_state*, uint8_t direction);
void apply_state(libusb_device_handle*, struct light_state*);
libusb_device_handle* open_ninja87();