/*
  CRONOMETRAJE F2C TEAM RACING / F2C TEAM RACING TIMING
  CAJA MESA CONTROL módulo 2.4 gHz / CONTROL BOX 2.4 gHz module
  v2.1 actualizado 04jun2023 / updated jun042023
*/

#include "config.h"
#include "main.h"

void setup() {
    Serial.begin(SERIAL_MONITOR_BAUDS);
    init_pins();
    init_radio();
    verify_laps_selector_pin();

    #ifdef DEBUG
    Serial.println("CAJA MESA CONTROL INICIALIZADA / CONTROL BOX STARTED");
    #endif
}

void loop() {
    loop_buttons();
}