#pragma once

#include "config.h"

void init_pins() {
    // pinMode(INFO_LED_PIN, OUTPUT);
    // digitalWrite(INFO_LED_PIN, LOW);

    // Config pines para objetos de control de pulsadores, todos con PULLUP habilitado
    // Pins config for pushbuttons control objects, all with enabled PULLUP
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

    // Intervalo de lectura para debouncing / debouncing interval
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
        #ifdef DEBUG
        Serial.println(F("NRF24: ERROR"));
        #endif
        // blink_info_led();
    }

    radio.setPALevel(RF24_PA_MAX); //(RF24_PA_MIN|RF24_PA_LOW|RF24_PA_HIGH|RF24_PA_MAX)
    radio.setDataRate(RF24_250KBPS); //(RF24_250KBPS|RF24_1MBPS|RF24_2MBPS)
    radio.setChannel(NRF_CHANNEL);
    radio.setPayloadSize(sizeof(payload));
    radio.openWritingPipe(RADIO_ADDRESSES[0]);
    radio.stopListening(); // Modo TX / TX Mode
    
    #ifdef DEBUG
    Serial.println(F("NRF24: OK modo TX / TX mode OK"));
    #endif
}

void send_radio_payload(int node) {
    if (node == -1) {
        for (int i = 0; i < 3; i++) {
            radio.openWritingPipe(RADIO_ADDRESSES[i]);
            radio.stopListening();
            // Asigna un id al azar / chooses a random id for the message
            ltoa(random(1, 65535), payload.id, 10);
            radio.write(&payload, sizeof(payload));
            delayMicroseconds(500);
        }
    } else {
        radio.openWritingPipe(RADIO_ADDRESSES[node]);
        radio.stopListening();
        // Asigna un id al azar / chooses a random id for the message
        ltoa(random(1, 65535), payload.id, 10);
        radio.write(&payload, sizeof(payload));

        /* #ifdef DEBUG
        Serial.println(payload.id);
        Serial.println(payload.data);
        Serial.println();
        #endif */
    }
}

void verify_laps_selector_pin() {
    // Chequea el estado del selector de vueltas y manda el mensaje
    // Check status for the laps selector switch and sends the message
    btn_ls.read() ? strcpy(payload.data, "100"): strcpy(payload.data, "200");
    send_radio_payload(-1);
}

void loop_buttons() {
    int destination_node = -1;
    static bool sendRadioCommand = false;

    // Rutinas de control de los pulsadores, deben ejecutarse tan seguido como se pueda en el loop
    // Pushbuttons control routines, must be executed as fast as possible in the main loop
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

    // Equipo rojo, pulsadores agregado y resta falta / Red team, add / delete warning pushbuttons
    if (btn_rtfp.fell()) {
        strcpy(payload.data, "RFP");
        destination_node = 0;
        sendRadioCommand = true;
    }
    if (btn_rtfm.fell()) {
        strcpy(payload.data, "RFM");
        destination_node = 0;
        sendRadioCommand = true;
    }

    // Equipo verde, pulsadores agregado y resta falta / Green team, add / delete warning pushbuttons
    if (btn_gtfp.fell()) {
        strcpy(payload.data, "GFP");
        destination_node = 1;
        sendRadioCommand = true;
    }
    if (btn_gtfm.fell()) {
        strcpy(payload.data, "GFM");
        destination_node = 1;
        sendRadioCommand = true;
    }

    // Equipo amarillo, pulsadores agregado y resta falta / Yellow team, add / delete warning pushbuttons
    if (btn_ytfp.fell()) {
        strcpy(payload.data, "YFP");
        destination_node = 2;
        sendRadioCommand = true;
    }
    if (btn_ytfm.fell()) {
        strcpy(payload.data, "YFM");
        destination_node = 2;
        sendRadioCommand = true;
    }

    // Pulsador inicio crono carrera / Start racing pushbutton
    if (btn_sr.fell()) {
        strcpy(payload.data, "SRS");
        sendRadioCommand = true;
    }

    // Pulsador reseteo crono carrera / Reset racing pushbutton
    if (btn_rr.fell()) {
        strcpy(payload.data, "RRS");
        sendRadioCommand = true;
    }

    // Pulsador arrancar motores / Start engines pushbutton
    if (btn_se.fell()) {
        strcpy(payload.data, "SES");
        sendRadioCommand = true;
    }

    // Switch elección de vueltas / Laps selector switch
    if (btn_ls.changed()) {
        verify_laps_selector_pin();
    }

    // Envío mensaje radio / Radio message delivery
    if (sendRadioCommand) {
        send_radio_payload(destination_node);
        sendRadioCommand = false;
    }
}