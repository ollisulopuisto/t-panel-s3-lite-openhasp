// Insert in gpioSetup() after aceButtonSetup().
// Re-applies BOOT/KEY2/KEY1 if NVS has no button config (older plates
// saved before GPIO 48 was in the picker). Not in upstream PR #1037.

#if defined(LILYGO_T_PANEL_LITE)
    {
        const uint8_t lite_btns[] = {0, 47, 48}; // BOOT, KEY2, KEY1
        for(uint8_t b = 0; b < 3; b++) {
            bool found = false;
            for(uint8_t i = 0; i < HASP_NUM_GPIO_CONFIG; i++) {
                if(gpioConfig[i].pin == lite_btns[b] && gpioConfig[i].type == hasp_gpio_type_t::BUTTON_TYPE) {
                    found = true;
                    break;
                }
            }
            if(!found) {
                int8_t id = gpioGetFreeConfigId();
                if(id >= 0) {
                    gpioSavePinConfig(id, lite_btns[b], hasp_gpio_type_t::BUTTON_TYPE, (uint8_t)(b + 1),
                                      hasp_gpio_function_t::INTERNAL_PULLUP, false);
                }
            }
        }
    }
#endif
