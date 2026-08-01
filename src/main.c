#include "driver/gpio.h"
#include "esp_timer.h"

#define LED_SEQUENCE_LONGEVITY 3
#define TIME_DIVIDER 1000

#define LED_PIN_RED GPIO_NUM_4
#define LED_PIN_GREEN GPIO_NUM_5
#define LED_PIN_BLUE GPIO_NUM_6

#define LED_FREQ_RED 200
#define LED_FREQ_GREEN 500
#define LED_FREQ_BLUE 1000

typedef struct {
    gpio_config_t* gpio_config;
    uint32_t last_blink_ms;
    uint16_t frequency;
    gpio_num_t pin;
    uint8_t level;
} led_config;

typedef struct {
    uint8_t longevity;
    led_config** leds;
} led_sequence_config;

gpio_config_t led_gpio_config_red = {
    .pin_bit_mask = 1ULL << LED_PIN_RED,
    .mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_ENABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE};

gpio_config_t led_gpio_config_green = {
    .pin_bit_mask = 1ULL << LED_PIN_GREEN,
    .mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_ENABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE}; 
    
gpio_config_t led_gpio_config_blue = {
    .pin_bit_mask = 1ULL << LED_PIN_BLUE,
    .mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_ENABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE};

led_config led_red = {
    .gpio_config = &led_gpio_config_red,
    .frequency = LED_FREQ_RED,
    .pin = LED_PIN_RED,
    .last_blink_ms = 0,
    .level = 0};

led_config led_green = {
    .gpio_config = &led_gpio_config_green,
    .frequency = LED_FREQ_GREEN,
    .pin = LED_PIN_GREEN,
    .last_blink_ms = 0,
    .level = 0};

led_config led_blue = {
    .gpio_config = &led_gpio_config_blue,
    .frequency = LED_FREQ_BLUE,
    .pin = LED_PIN_BLUE,
    .last_blink_ms = 0,
    .level = 0};    

led_config* leds[LED_SEQUENCE_LONGEVITY] = {&led_red, &led_green, &led_blue};

led_sequence_config config = {
    .longevity = LED_SEQUENCE_LONGEVITY,
    .leds = leds
};

void blink_many(led_sequence_config* config)
{
    uint32_t now = esp_timer_get_time() / TIME_DIVIDER;

    for (uint8_t i = 0; i < config->longevity; i++)
    {
        if ((now - config->leds[i]->last_blink_ms) >= config->leds[i]->frequency) 
        {
            uint8_t level = config->leds[i]->level == 0 ? 1 : 0;
            
            config->leds[i]->last_blink_ms = now;
            config->leds[i]->level = level;

            gpio_set_level(config->leds[i]->pin, level);
        }
    }
}

void init(led_sequence_config* config, uint8_t longevity) 
{
    for (uint8_t i = 0; i < longevity; i++)
    {
        gpio_config(config->leds[i]->gpio_config);
    }
}

void app_main() 
{
    init(&config, LED_SEQUENCE_LONGEVITY);
    
    while (1)
    {
        blink_many(&config);
    }
}