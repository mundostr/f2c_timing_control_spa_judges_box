#pragma once

#include "config.h"

void init_pins() {
    // pinMode(INFO_LED_PIN, OUTPUT);
    // digitalWrite(INFO_LED_PIN, LOW);

    // Config pines para objetos de control de pulsadores, todos con PULLUP habilitado
    btn_rtfp.attach(RED_TEAM_FAULT_PLUS, INPUT_PULLUP);
    btn_rtfm.attach(RED_TEAM_FAULT_MINUS, INPUT_PULLUP);
    btn_gtfp.attach(GREEN_TEAM_FAULT_PLUS, INPUT_PULLUP);
    btn_gtfm.attach(GREEN_TEAM_FAULT_MINUS, INPUT_PULLUP);
    btn_ytfp.attach(YELLOW_TEAM_FAULT_PLUS, INPUT_PULLUP);
    btn_ytfm.attach(YELLOW_TEAM_FAULT_MINUS, INPUT_PULLUP);
    btn_sr.attach(START_RACE_PIN, INPUT_PULLUP);
    btn_rr.attach(RESET_RACE_PIN, INPUT_PULLUP);
    btn_se.attach(START_ENGINES_PIN, INPUT_PULLUP);
    btn_ls.attach(LAPS_SELECTOR_PIN, INPUT_PULLUP);

    // Intervalo de lectura para debouncing
    btn_rtfp.interval(BUTTONS_READING_FREQ);
    btn_rtfm.interval(BUTTONS_READING_FREQ);
    btn_gtfp.interval(BUTTONS_READING_FREQ);
    btn_gtfm.interval(BUTTONS_READING_FREQ);
    btn_ytfp.interval(BUTTONS_READING_FREQ);
    btn_ytfm.interval(BUTTONS_READING_FREQ);
    btn_sr.interval(BUTTONS_READING_FREQ);
    btn_rr.interval(BUTTONS_READING_FREQ);
    btn_se.interval(BUTTONS_READING_FREQ);
    btn_ls.interval(BUTTONS_READING_FREQ);

    randomSeed(analogRead(4));
}

// void blink_info_led() {
//     for(;;) {
//         digitalWrite(INFO_LED_PIN, !digitalRead(INFO_LED_PIN));
//         delay(250);
//     }
// }

void init_radio() {
    if (!radio.begin()) {
        Serial.println(F("NRF24: ERROR"));
        // blink_info_led();
    }

    radio.setPALevel(RF24_PA_MAX); //(RF24_PA_MIN|RF24_PA_LOW|RF24_PA_HIGH|RF24_PA_MAX)
    radio.setDataRate(RF24_250KBPS); //(RF24_250KBPS|RF24_1MBPS|RF24_2MBPS)
    radio.setChannel(NRF_CHANNEL);
    radio.setPayloadSize(sizeof(payload));
    radio.openWritingPipe(RADIO_ADDRESS);
    radio.stopListening(); // Modo TX
    
    Serial.println(F("NRF24: OK modo TX"));
}

void send_radio_payload() {
    // Asigna un id al azar
    ltoa(random(1, 65535), payload.id, 10);
    // ltoa(millis(), payload.id, 10); // chequear para no exceder los 5 dígitos
    radio.write(&payload, sizeof(payload));

    Serial.println(payload.id);
    Serial.println(payload.data);
    Serial.println();
}

void verify_laps_selector_pin() {
    // Chequea simplemente al inicio el estado del selector de vueltas y manda el mensaje
    btn_ls.read() ? strcpy(payload.data, "100"): strcpy(payload.data, "200");
    send_radio_payload();
}

void loop_buttons() {
    static bool sendRadioCommand = false;

    // Rutinas de control de los pulsadores, deben ejecutarse tan seguido como se pueda en el loop
    btn_rtfp.update();
    btn_rtfm.update();
    btn_gtfp.update();
    btn_gtfm.update();
    btn_ytfp.update();
    btn_ytfm.update();
    btn_sr.update();
    btn_rr.update();
    btn_se.update();
    btn_ls.update();

    // Equipo rojo, pulsadores agregado y resta falta
    if (btn_rtfp.fell()) {
        strcpy(payload.data, "RFP");
        sendRadioCommand = true;
    }
    if (btn_rtfm.fell()) {
        strcpy(payload.data, "RFM");
        sendRadioCommand = true;
    }

    // Equipo verde, pulsadores agregado y resta falta
    if (btn_gtfp.fell()) {
        strcpy(payload.data, "GFP");
        sendRadioCommand = true;
    }
    if (btn_gtfm.fell()) {
        strcpy(payload.data, "GFM");
        sendRadioCommand = true;
    }

    // Equipo amarillo, pulsadores agregado y resta falta
    if (btn_ytfp.fell()) {
        strcpy(payload.data, "YFP");
        sendRadioCommand = true;
    }
    if (btn_ytfm.fell()) {
        strcpy(payload.data, "YFM");
        sendRadioCommand = true;
    }

    // Pulsador inicio crono carrera
    if (btn_sr.fell()) {
        strcpy(payload.data, "SRS");
        sendRadioCommand = true;
    }

    // Pulsador reseteo crono carrera
    if (btn_rr.fell()) {
        strcpy(payload.data, "RRS");
        sendRadioCommand = true;
    }

    // Pulsador arrancar motores
    if (btn_se.fell()) {
        strcpy(payload.data, "SES");
        sendRadioCommand = true;
    }

    // Pulsador elección de vueltas
    if (btn_ls.changed()) {
        verify_laps_selector_pin();
    }

    // Envío mensaje radio
    if (sendRadioCommand) {
        send_radio_payload();
        sendRadioCommand = false;
    }
}