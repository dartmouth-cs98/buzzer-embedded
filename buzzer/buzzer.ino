#include "BuzzerFSMCallbacks.h"
#include "Globals.h"
#include "Pins.h"
#include "EEPROMReadWrite.h"
#include <EEPROM.h>

BuzzerFSM buzzer_fsm({INIT_FONA, INIT, INIT, InitFunc}, INIT);
SoftwareSerial fona_serial = SoftwareSerial(FONA_TX_PIN, FONA_RX_PIN);
FonaShield fona_shield(&fona_serial, FONA_RST_PIN);
SSD1306AsciiAvrI2c oled;
char buzzer_name_global[30];

void ClearEEPROM() {
  for (int i=0; i<EEPROM.length(); i++) {
    EEPROM.write(i, 0);
  }
}

void setup() {
  Serial.begin(115200);
  // ClearEEPROM();
  pinMode(FONA_RST_PIN, OUTPUT);
  DEBUG_PRINTLN_FLASH("Attempting to init display");
  oled.begin(&Adafruit128x32, I2C_ADDRESS);
  oled.setFont(Adafruit5x7);
  DEBUG_PRINTLN_FLASH("Display successfully initialized");
  pinMode(BUZZER_PIN, OUTPUT);
  EEPROMRead(buzzer_name_global, sizeof(buzzer_name_global));
  Serial.print("Stored in eeprom: ");
  Serial.println(buzzer_name_global);
  //TODO: there needs to be a "catastrophic" error state to go into
  buzzer_fsm.AddState({INIT_GPRS, INIT, INIT, InitFonaShieldFunc}, INIT_FONA);
  int init_gprs_next_state = IDLE;
  if (strlen(buzzer_name_global) == 0) init_gprs_next_state = GET_BUZZER_NAME;
  buzzer_fsm.AddState({init_gprs_next_state, INIT, INIT, InitGPRSFunc}, INIT_GPRS);
  buzzer_fsm.AddState({IDLE, INIT, INIT, GetBuzzerNameFunc}, GET_BUZZER_NAME);
  buzzer_fsm.AddState({INIT, INIT, INIT, IdleFunc}, IDLE);
}

void loop() {
  buzzer_fsm.ProcessState();
}
