/*
  CRONOMETRAJE F2C TEAM RACING, CAJA MESA CONTROL
  F2C TEAM RACING TIMING, CHAIRMAN CONTROL BOX
  Código v2.0 actualizado 22may2023
  Code v2.0 updated may222023
*/

#include "config.h"
#include "main.h"

void setup() {
    Serial.begin(SERIAL_MONITOR_BAUDS);
    init_pins();
    init_radio();
    verify_laps_selector_pin();
    Serial.println("CAJA MESA CONTROL INICIALIZADA");
}

void loop() {
    loop_buttons();
}