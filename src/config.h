/*
https://lastminuteengineers.com/nrf24l01-arduino-wireless-communication/
*/

#pragma once

#define DEBUG // Si se define se habilita la consola serial / If defined, serial console is enabled
#define AUDIO_ENABLED // Si se define, se activa el uso de audio / If defined, audio module is enabled

#include <Arduino.h>
#include <Bounce2.h>
#include <SPI.h>
#include <RF24.h>
#ifdef AUDIO_ENABLED
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#endif

// Constantes pines / pins definition
#define RED_TEAM_FAULT_PLUS 3
#define RED_TEAM_FAULT_MINUS 5
#define GREEN_TEAM_FAULT_PLUS 6
#define GREEN_TEAM_FAULT_MINUS 7
#define YELLOW_TEAM_FAULT_PLUS 8
#define YELLOW_TEAM_FAULT_MINUS 9
#define RESET_RACE_PIN 16
#define START_RACE_PIN 17
#define START_ENGINES_PIN 4
#define LAPS_SELECTOR_PIN 2
#define NRF_CE_PIN 15
#define NRF_CSN_PIN 10
#define DF_TX_PIN 19
#define DF_RX_PIN 18

// Constantes generales / general constants
#define SERIAL_MONITOR_BAUDS 115200
#define RADIO_COMMAND_LENGTH 3
#define BUTTONS_READING_FREQ 30
#define NRF_CHANNEL 108
#define AUDIO_VOLUME 15 // 0 a 30
#define START_SIGNAL_DELAY 5500

// Objectos para control de radio y pulsadores / Objects for radio and pushbuttons control
RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);
Bounce btn_rtfp, btn_rtfm, btn_gtfp, btn_gtfm, btn_ytfp, btn_ytfm, btn_sr, btn_rr, btn_se, btn_ls = Bounce();
#ifdef AUDIO_ENABLED
SoftwareSerial dfSerial(DF_RX_PIN, DF_TX_PIN);
DFRobotDFPlayerMini dfPlayer;
#endif

// Dirección de comunicación / NRF comm address
const byte RADIO_ADDRESSES[3][6] = {"00001", "00002", "00003"};

// Payload para manejo de datos / Payload for data handling
struct Payload {
    char id[6];
    char data[RADIO_COMMAND_LENGTH + 1] = "000";
};

Payload payload;

// Puntero archivos de audio (0- español, 1- inglés) / Audios pointer (0- spanish, 1- english)
#ifdef AUDIO_ENABLED
int current_audio_ptr = 1;
uint32_t start_delay_timer = 0;
bool start_signal_just_received = false;
#endif