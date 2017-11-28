#include <SPI.h>
#include <EEPROM.h>
#include <nRF24L01.h>
#include "RF24.h"
#include "VescUart.h"

struct vescValues {
  float ampHours;
  float inpVoltage;
  long rpm;
  long tachometerAbs;
};

struct remoteData { //   cmd    | settings
  byte packageType; //    0     |    1
  byte item1;       // throttle | useUart
  byte item2;       // Button   |    -
};

#define CONFIG_VERSION "rx1"
#define CONFIG_START 32

struct settings {
  bool useUart;
  char version_of_program[4];
} rxSettings = {
  true, // set true to use uart for sending throttle
  CONFIG_VERSION
};

RF24 radio(9, 10);
int radioChannel = 108; // Above most WiFi frequencies
const uint64_t pipe = 0xABCDABCD71LL;

bool recievedData = false;
uint32_t lastTimeReceived = 0;

int motorSpeed = 127;
int timeoutMax = 500;
int speedPin = 5;

struct remotePackage remPack;
struct bldcMeasure measuredValues;

struct vescValues data;
unsigned long lastDataCheck;

struct remoteData remData;

void setup() {
  SERIALIO.begin(115200);

  loadEEPROMSettings();

  radio.begin();
  radio.setChannel(radioChannel);
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  radio.openReadingPipe(1, pipe);
  radio.startListening();

  if (rxSettings.useUart) 
  { 
    remPack.valXJoy         = 127; //middle Position 
    remPack.valYJoy         = 127; 
    remPack.valUpperButton  = 0; 
    remPack.valLowerButton  = 0; 
  } 
  else 
  { 
    pinMode(speedPin, OUTPUT); 
    analogWrite(speedPin, motorSpeed); 
  }
}

void loop() {

  getVescData();

  // If transmission is available
  if (radio.available())
  {
    //int payloadSize = radio.getDynamicPayloadSize(); // get PayloadSize

    // Read the actual message
    radio.read(&remData, sizeof(remData));

    recievedData = true;
  }

  if (recievedData == true)
  {
    // A speed is received from the transmitter (remote).

    lastTimeReceived = millis();
    recievedData = false;

    switch (remData.packageType) {
      case 0:
        motorSpeed = remData.item1;
        remPack.valLowerButton = remData.item2;
        radio.writeAckPayload(pipe, &data, sizeof(data));
        break;
      case 1:
        rxSettings.useUart = remData.item1;
        radio.writeAckPayload(pipe, &data, sizeof(data));
        updateEEPROMSettings();
        break;
    }

    if (rxSettings.useUart) {
      remPack.valYJoy = motorSpeed;
      VescUartSetNunchukValues(remPack); 
    } else { 
      // Write the PWM signal to the ESC (0-255). 
      analogWrite(speedPin, motorSpeed); 
    }
  }
  else if ((millis() - lastTimeReceived) > timeoutMax)
  {
    // No speed is received within the timeout limit.
    motorSpeed = 127;
    if (rxSettings.useUart) { 
      remPack.valYJoy = motorSpeed; 
      VescUartSetNunchukValues(remPack); 
    } else { 
      analogWrite(speedPin, motorSpeed); 
    }
  }
}

void getVescData() {

  if (millis() - lastDataCheck >= 250) {

    lastDataCheck = millis();

    // Only transmit what we need
    if (VescUartGetValue(measuredValues)) {
      data.ampHours = measuredValues.ampHours;
      data.inpVoltage = measuredValues.inpVoltage;
      data.rpm = measuredValues.rpm;
      data.tachometerAbs = measuredValues.tachometerAbs;
    } else {
      data.ampHours = 0.0;
      data.inpVoltage = 0.0;
      data.rpm = 0;
      data.tachometerAbs = 0;
    }
  }
}

void setDefaultEEPROMSettings() {
  clearEEPROM();
  updateEEPROMSettings();
}

void loadEEPROMSettings() {
  // Load settings from EEPROM to custom struct
  if (EEPROM.read(CONFIG_START + sizeof(settings) - 2) == rxSettings.version_of_program[2] &&
      EEPROM.read(CONFIG_START + sizeof(settings) - 3) == rxSettings.version_of_program[1] &&
      EEPROM.read(CONFIG_START + sizeof(settings) - 4) == rxSettings.version_of_program[0])
  {
    EEPROM.get(CONFIG_START, rxSettings);
  } else {
    setDefaultEEPROMSettings();
  }
}

// Write settings to the EEPROM then exiting settings menu.
void updateEEPROMSettings() {
  EEPROM.put(CONFIG_START, rxSettings);
}

void clearEEPROM() {
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
}
