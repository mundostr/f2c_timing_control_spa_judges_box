/*
  CRONOMETRAJE F2C TEAM RACING / F2C TEAM RACING TIMING
  CAJA MESA CONTROL módulo 2.4 gHz / CONTROL BOX 2.4 gHz module
  v3.0 actualizado 15sep2023 / updated sep152023
*/

#include "config.h"
#include "main.h"

void setup() {
    Serial.begin(SERIAL_MONITOR_BAUDS);
    init_pins();
    verify_config_mode();
    
    init_radio();
    #ifdef AUDIO_ENABLED
    init_player();
    #endif
    verify_laps_selector_pin(true);

    #ifdef DEBUG
    Serial.println("CAJA MESA CONTROL INICIALIZADA / CONTROL BOX STARTED");
    #endif
}

void loop() {
    loop_buttons();
}