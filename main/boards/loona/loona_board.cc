#include "wifi_board.h"
#include "codecs/no_audio_codec.h"
#include "display/display.h"
#include "system_reset.h"
#include "application.h"
#include "button.h"
#include "config.h"
#include "mcp_server.h"
#include "led/single_led.h"
#include "assets/lang_config.h"

#include <esp_log.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <string.h>

#define TAG "LoonaBoard"

// Structure matching the ESP32 DevKit receiver
typedef struct __attribute__((packed)) {
    char device[16];
    char action[16];
} LoonaCommand;

class LoonaBoard : public WifiBoard {
private:
    Button boot_button_;
    bool esp_now_initialized_ = false;

    // ESP32 DevKit Broadcast / Peer Address (FF:FF:FF:FF:FF:FF or specific MAC)
    uint8_t broadcast_mac_[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    void InitializeEspNow() {
        if (esp_now_init() != ESP_OK) {
            ESP_LOGE(TAG, "Failed to initialize ESP-NOW");
            return;
        }

        esp_now_peer_info_t peer_info = {};
        memcpy(peer_info.peer_addr, broadcast_mac_, 6);
        peer_info.channel = 0; // Use current Wi-Fi channel
        peer_info.encrypt = false;

        if (esp_now_add_peer(&peer_info) != ESP_OK) {
            ESP_LOGW(TAG, "Failed to add broadcast peer, maybe already exists");
        } else {
            ESP_LOGI(TAG, "ESP-NOW broadcast peer added successfully");
        }
        esp_now_initialized_ = true;
    }

    void SendLightCommand(const char* action) {
        if (!esp_now_initialized_) {
            InitializeEspNow();
        }

        LoonaCommand cmd;
        strncpy(cmd.device, "light", sizeof(cmd.device) - 1);
        cmd.device[sizeof(cmd.device) - 1] = '\0';
        strncpy(cmd.action, action, sizeof(cmd.action) - 1);
        cmd.action[sizeof(cmd.action) - 1] = '\0';

        esp_err_t err = esp_now_send(broadcast_mac_, (const uint8_t*)&cmd, sizeof(cmd));
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "Sent ESP-NOW command: light -> %s", action);
        } else {
            ESP_LOGE(TAG, "ESP-NOW send failed: %d", err);
        }
    }

    void InitializeButtons() {
        boot_button_.OnClick([this]() {
            auto& app = Application::GetInstance();
            if (app.GetDeviceState() == kDeviceStateStarting) {
                EnterWifiConfigMode();
                return;
            }
            app.ToggleChatState();
        });
    }

    void InitializeMcpTools() {
        auto& mcp_server = McpServer::GetInstance();

        // Register Loona Room Light Control tool for AI
        mcp_server.AddTool("self.room_light.turn_on", "Turn on the room light", PropertyList(), [this](const PropertyList& properties) -> ReturnValue {
            SendLightCommand("on");
            return true;
        });

        mcp_server.AddTool("self.room_light.turn_off", "Turn off the room light", PropertyList(), [this](const PropertyList& properties) -> ReturnValue {
            SendLightCommand("off");
            return true;
        });
    }

public:
    LoonaBoard() : boot_button_(BOOT_BUTTON_GPIO) {
        InitializeButtons();
        InitializeMcpTools();
        GetAudioCodec()->SetOutputVolume(100);
    }

    virtual Led* GetLed() override {
        static SingleLed led(BUILTIN_LED_GPIO);
        return &led;
    }

    virtual AudioCodec* GetAudioCodec() override {
        // High-performance Duplex I2S audio codec using your exact pins (BCLK 4, LRC 5, DIN 6, MIC 7)
        static NoAudioCodecDuplex audio_codec(
            AUDIO_INPUT_SAMPLE_RATE,
            AUDIO_OUTPUT_SAMPLE_RATE,
            AUDIO_I2S_GPIO_BCLK,
            AUDIO_I2S_GPIO_WS,
            AUDIO_I2S_GPIO_DOUT,
            AUDIO_I2S_GPIO_DIN
        );
        return &audio_codec;
    }

    virtual Display* GetDisplay() override {
        static NoDisplay no_display;
        return &no_display;
    }
};

DECLARE_BOARD(LoonaBoard);
