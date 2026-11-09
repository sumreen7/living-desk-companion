#include "display.h"

#include <Arduino.h>

#include "color.h"
#include "config.h"
#include "log.h"
#include "pins.h"

static_assert(LPC_ROTATION == 0 || LPC_ROTATION == 2, "Use rotation 0 or 2");

class LovyanPanel : public lgfx::LGFX_Device {
public:
    LovyanPanel() {
        {
            auto cfg = bus.config();
            cfg.spi_host = SPI2_HOST;
            cfg.spi_mode = 0;
            cfg.freq_write = LPC_SPI_WRITE_HZ;
            cfg.freq_read = LPC_SPI_READ_HZ;
            cfg.spi_3wire = false;
            cfg.use_lock = true;
            cfg.dma_channel = SPI_DMA_CH_AUTO;
            cfg.pin_sclk = PIN_TFT_SCK;
            cfg.pin_mosi = PIN_TFT_MOSI;
            cfg.pin_miso = PIN_TFT_MISO;
            cfg.pin_dc = PIN_TFT_DC;
            bus.config(cfg);
            panel.setBus(&bus);
        }
        {
            auto cfg = panel.config();
            cfg.pin_cs = PIN_TFT_CS;
            cfg.pin_rst = PIN_TFT_RST;
            cfg.pin_busy = -1;
            cfg.panel_width = LPC_SCREEN_WIDTH;
            cfg.panel_height = LPC_SCREEN_HEIGHT;
            cfg.offset_x = 0;
            cfg.offset_y = 0;
            cfg.offset_rotation = 0;
            cfg.dummy_read_pixel = 8;
            cfg.dummy_read_bits = 1;
            cfg.readable = true;
            cfg.invert = LPC_INVERT_COLORS != 0;
            cfg.rgb_order = LPC_BGR_PANEL != 0;
            cfg.dlen_16bit = false;
            cfg.bus_shared = true;
            panel.config(cfg);
        }
        {
            auto cfg = touch.config();
            cfg.x_min = TOUCH_X_MIN;
            cfg.x_max = TOUCH_X_MAX;
            cfg.y_min = TOUCH_Y_MIN;
            cfg.y_max = TOUCH_Y_MAX;
            cfg.pin_int = PIN_TOUCH_IRQ;
            cfg.bus_shared = true;
            cfg.offset_rotation = TOUCH_ROTATION;
            cfg.spi_host = SPI2_HOST;
            cfg.freq = LPC_TOUCH_HZ;
            cfg.pin_sclk = PIN_TFT_SCK;
            cfg.pin_mosi = PIN_TFT_MOSI;
            cfg.pin_miso = PIN_TFT_MISO;
            cfg.pin_cs = PIN_TOUCH_CS;
            touch.config(cfg);
            panel.setTouch(&touch);
        }
        setPanel(&panel);
    }

    lgfx::Panel_ILI9341 panel;
    lgfx::Bus_SPI bus;
    lgfx::Touch_XPT2046 touch;
};

static LovyanPanel* gPanel = nullptr;
static LGFX_Sprite* gFrame = nullptr;

lgfx::LGFX_Device& displayDevice() {
    return *gPanel;
}

bool displayPanelReady() {
    return gPanel != nullptr;
}

bool displayStart() {
    if (!gPanel) {
        gPanel = new LovyanPanel();
    }
    bool ok = gPanel->init();
    gPanel->setRotation(LPC_ROTATION);
    gPanel->setColorDepth(16);
    logf("display: init %s", ok ? "ok" : "returned false");
    return true;
}

bool displayMakeSprite() {
    if (!gPanel) {
        return false;
    }
    if (!gFrame) {
        gFrame = new LGFX_Sprite(gPanel);
    }
    gFrame->setColorDepth(16);
    gFrame->setPsram(true);
    void* buffer = gFrame->createSprite(LPC_SCREEN_WIDTH, LPC_SCREEN_HEIGHT);
    if (!buffer) {
        logLine("display: picture buffer could not be created");
        return false;
    }
    logf("display: picture buffer %u bytes", (unsigned)(LPC_SCREEN_WIDTH * LPC_SCREEN_HEIGHT * 2));
    return true;
}

void displayColorTest() {
    if (!gPanel) {
        return;
    }
    struct Bar {
        uint16_t color;
        const char* name;
        uint16_t ink;
    };
    const Bar bars[] = {
        {rgb565(214, 64, 72), "red", rgb565(255, 248, 240)},
        {rgb565(64, 168, 96), "green", rgb565(255, 248, 240)},
        {rgb565(58, 112, 214), "blue", rgb565(255, 248, 240)},
        {rgb565(245, 240, 230), "white", rgb565(36, 48, 68)},
    };
    for (const Bar& bar : bars) {
        logf("screen should be %s", bar.name);
        gPanel->fillScreen(bar.color);
        gPanel->setFont(&fonts::Font4);
        gPanel->setTextSize(1);
        gPanel->setTextDatum(textdatum_t::middle_center);
        gPanel->setTextColor(bar.ink);
        gPanel->drawString(bar.name, gPanel->width() / 2, gPanel->height() / 2);
        delay(550);
    }
}

Canvas displayCanvas(int ox) {
    return Canvas{gFrame, ox};
}

void displayPushFrame() {
    if (gFrame) {
        gFrame->pushSprite(0, 0);
    }
}
