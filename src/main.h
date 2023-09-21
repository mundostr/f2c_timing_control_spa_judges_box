#pragma once

#include "config.h"

void init_pins() {
    pinMode(DF_BUSY_PIN, INPUT_PULLUP);
    
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

void init_radio() {
    if (!radio.begin()) {
        #ifdef DEBUG
        Serial.println(F("NRF24: ERROR"));
        #endif
        for(;;);
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

void init_player() {
    #ifdef AUDIO_ENABLED
    dfSerial.begin(9600);

    if (!dfPlayer.begin(dfSerial)) {
        #ifdef DEBUG
        Serial.println(F("MP3: ERROR"));
        #endif
        for(;;);
    }
    dfPlayer.volume(AUDIO_VOLUME);
    
    #ifdef DEBUG
    Serial.println(F("MP3: OK!"));
    #endif
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
    }
        
    #ifdef DEBUG
    Serial.println(payload.data);
    #endif
}

void verify_laps_selector_pin(boolean onBoot = false) {
    // Chequea el estado del selector de vueltas y manda el mensaje
    // Se aprovecha también para cambiar entre audio español o inglés al iniciar
    // Check status for the laps selector switch and sends the message
    // Is also used to switch between spanish and english audio when booting
    if (btn_ls.read()) {
        strcpy(payload.data, "100");
        
        #ifdef AUDIO_ENABLED
        if (onBoot) { current_audio_ptr = 1; }
        #endif
    } else {
        strcpy(payload.data, "200");
        
        #ifdef AUDIO_ENABLED
        if (onBoot) { current_audio_ptr = 2; }
        #endif
    }

    send_radio_payload(-1);
}

void verify_config_mode() {
    mode = digitalRead(START_RACE_PIN) ? NORMAL: CONFIG;
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
        if (mode == NORMAL) {
            strcpy(payload.data, "RFP");
            destination_node = 0;
        } else {
            strcpy(payload.data, "BRP");
        }
        sendRadioCommand = true;
    }
    if (btn_rtfm.fell()) {
        if (mode == NORMAL) {
            strcpy(payload.data, "RFM");
            destination_node = 0;
        } else {
            strcpy(payload.data, "BRM");
        }
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
        #ifdef AUDIO_ENABLED
        dfPlayer.stop();
        #endif
        strcpy(payload.data, "SRS");
        sendRadioCommand = true;
    }

    // Pulsador reseteo crono carrera / Reset racing pushbutton
    if (btn_rr.fell()) {
        #ifdef AUDIO_ENABLED
        dfPlayer.stop();
        #endif
        strcpy(payload.data, "RRS");
        sendRadioCommand = true;
    }

    // Pulsador arrancar motores / Start engines pushbutton
    if (btn_se.fell()) {
        // Se reproduce el audio de inicio si está habilitado / Start engines audio is played if enabled
        #ifdef AUDIO_ENABLED
        dfPlayer.play(current_audio_ptr);
        start_signal_just_received = true;
        start_delay_timer = millis();

        #else
        strcpy(payload.data, "SES");
        sendRadioCommand = true;
        #endif
    }

    // Switch elección de vueltas / Laps selector switch
    if (btn_ls.changed()) {
        verify_laps_selector_pin();
    }

    #ifdef AUDIO_ENABLED
    if (start_signal_just_received && millis() - start_delay_timer >= START_SIGNAL_DELAY) {
        start_signal_just_received = false;
        start_delay_timer = millis();
        player_status_timer = millis();
        strcpy(payload.data, "SES");
        sendRadioCommand = true;
        check_player_status = true;
    }

    /* if (check_player_status && millis() - player_status_timer >= 10) {
        if (digitalRead(DF_BUSY_PIN)) {
            strcpy(payload.data, "SRS");
            sendRadioCommand = true;
            check_player_status = false;
        }

        player_status_timer = millis();
    } */
    #endif
    
    // Envío mensaje radio / Radio message delivery
    if (sendRadioCommand) {
        send_radio_payload(destination_node);
        sendRadioCommand = false;
    }
}