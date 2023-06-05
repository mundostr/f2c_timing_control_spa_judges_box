/*
https://lastminuteengineers.com/nrf24l01-arduino-wireless-communication/
*/

#pragma once

#include <Arduino.h>
#include <Bounce2.h>
#include <SPI.h>
#include <RF24.h>

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
// #define INFO_LED_PIN 18

// Constantes generales / general constants
#define DEBUG // Si se define se habilita la consola serial / If defined, serial console is enabled
#define SERIAL_MONITOR_BAUDS 115200
#define RADIO_COMMAND_LENGTH 3
#define BUTTONS_READING_FREQ 30
#define NRF_CHANNEL 108

// Objectos para control de radio y pulsadores / Objects for radio and pushbuttons control
RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);
Bounce btn_rtfp, btn_rtfm, btn_gtfp, btn_gtfm, btn_ytfp, btn_ytfm, btn_sr, btn_rr, btn_se, btn_ls = Bounce();

// Dirección de comunicación / NRF comm address
const byte RADIO_ADDRESS[6] = "00001";

// Payload para manejo de datos / Payload for data handling
struct Payload {
    char id[6];
    char data[RADIO_COMMAND_LENGTH + 1] = "000";
};

Payload payload;