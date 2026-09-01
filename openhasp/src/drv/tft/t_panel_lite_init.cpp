// Drop-in Arduino_GFX init for LilyGo T-Panel S3 Lite (openHASP).
// Arduino_GFX 1.4.7 little-endian maps constructor b* -> data_gpio[0].
// LilyGO's esp_lcd config puts R0 there, so pass B,G,R into the RGB constructor.

#elif(TFT_WIDTH == 480) && (TFT_HEIGHT == 480) && defined(LILYGO_T_PANEL_LITE)
    Arduino_DataBus* bus = new Arduino_SWSPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, TFT_MISO);
    Arduino_ESP32RGBPanel* rgbpanel = new Arduino_ESP32RGBPanel(
        TFT_DE, TFT_VSYNC, TFT_HSYNC, TFT_PCLK, TFT_B0, TFT_B1, TFT_B2, TFT_B3, TFT_B4, TFT_G0, TFT_G1, TFT_G2,
        TFT_G3, TFT_G4, TFT_G5, TFT_R0, TFT_R1, TFT_R2, TFT_R3, TFT_R4, TFT_HSYNC_POLARITY, TFT_HSYNC_FRONT_PORCH,
        TFT_HSYNC_PULSE_WIDTH, TFT_HSYNC_BACK_PORCH, TFT_VSYNC_POLARITY, TFT_VSYNC_FRONT_PORCH, TFT_VSYNC_PULSE_WIDTH,
        TFT_VSYNC_BACK_PORCH, TFT_PCLK_ACTIVE_NEG, TFT_PREFER_SPEED, false /* useBigEndian */, 0 /* de_idle_high */,
        0 /* pclk_idle_high */);

    tft = new Arduino_RGB_Display(TFT_WIDTH, TFT_HEIGHT, rgbpanel, 0 /* rotation */, true /* auto_flush */, bus,
                                  TFT_RST, st7701_t_panel_init_operations, sizeof(st7701_t_panel_init_operations));
    tft->begin(GFX_NOT_DEFINED);
