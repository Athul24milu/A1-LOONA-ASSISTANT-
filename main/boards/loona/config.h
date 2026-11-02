#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <driver/gpio.h>

#define AUDIO_INPUT_SAMPLE_RATE  16000
#define AUDIO_OUTPUT_SAMPLE_RATE 24000

// Duplex I2S Mode (Sharing BCLK and WS between Amp and Mic)
// Comment out AUDIO_I2S_METHOD_SIMPLEX to enable Duplex
#undef AUDIO_I2S_METHOD_SIMPLEX

#define AUDIO_I2S_GPIO_BCLK GPIO_NUM_4
#define AUDIO_I2S_GPIO_WS   GPIO_NUM_5
#define AUDIO_I2S_GPIO_DOUT GPIO_NUM_6   // Speaker Data Out (Amp DIN)
#define AUDIO_I2S_GPIO_DIN  GPIO_NUM_7   // Microphone Data In (INMP441 SD)

// Onboard WS2812 RGB LED and Boot Button
#define BUILTIN_LED_GPIO        GPIO_NUM_48
#define BOOT_BUTTON_GPIO        GPIO_NUM_0
#define TOUCH_BUTTON_GPIO       GPIO_NUM_NC
#define VOLUME_UP_BUTTON_GPIO   GPIO_NUM_NC
#define VOLUME_DOWN_BUTTON_GPIO GPIO_NUM_NC

// MAX7219 Dot Matrix Eyes
#define EYE_DIN_PIN             GPIO_NUM_1
#define EYE_CS_PIN              GPIO_NUM_2
#define EYE_CLK_PIN             GPIO_NUM_42

// Ear Servos (PWM)
#define SERVO_LEFT_EAR_PIN      GPIO_NUM_15
#define SERVO_RIGHT_EAR_PIN     GPIO_NUM_16

// Display is handled by MAX7219 Eyes
#define DISPLAY_WIDTH           16
#define DISPLAY_HEIGHT          8

#endif // _BOARD_CONFIG_H_
