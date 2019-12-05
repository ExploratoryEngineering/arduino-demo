/***********************************************************************
  Sample code for a minimalist sensor setup with a microswitch, a 
  potmeter and a led.
***********************************************************************/

#include <Udp.h>
#include <TelenorNBIoT.h>
#include <SoftwareSerial.h>
SoftwareSerial ublox(10, 11);

TelenorNBIoT nbiot;

// The remote IP address to send data packets to
// u-blox SARA N2 does not support DNS
IPAddress remoteIP(172, 16, 15, 14);
int REMOTE_PORT = 1234;

// Pins for LED, button and potmeter
int LED = 7;
int BUTTON = 6;
int POT = 0;

// Flash the lead once
void blinkLED() {
  digitalWrite(LED, HIGH);
  delay(500);
  digitalWrite(LED, LOW);
}

void setup() {
  // LED setup
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  // Open a serial connection over USB for Serial Monitor
  Serial.begin(9600);

  while (!Serial);
  ublox.begin(9600);
  
  // Try to initialize the NB-IoT module until it succeeds
  Serial.print("Connecting to NB-IoT module...\n");
  while (!nbiot.begin(ublox)) {
    Serial.println("Begin failed. Retrying...");
    delay(1000);
  }
  
  // Try to create a socket until it succeeds
  while (!nbiot.createSocket()) {
    Serial.print("Error creating socket. Error code: ");
    Serial.println(nbiot.errorCode(), DEC);
    delay(100);
  }
  // Blink the LED twice when ready
  blinkLED();
  delay(500);
  blinkLED();
  Serial.println("Connected!");
}

// Handle button presses. Prints the analog reading to the serial console
// and sends it to the Horde backend.
void handleButtonPress() {
    int val = analogRead(POT);
    Serial.print("Analog = ");
    Serial.print(val);
    Serial.println();
    if (nbiot.isConnected()) {
      char buf[2];
      buf[0] = (char)(val & 0xFF);
      buf[1] = (char)((val & 0xFF00) >> 8);
      if (nbiot.sendBytes(remoteIP, REMOTE_PORT, buf, 2)) {
        blinkLED();        
      } else {
        Serial.println("Error sending message");
      }
    } else {
      Serial.println("Not connected yet");
    }
}

// Main loop. Checks the button every 100ms; if it is pressed it will send
// the potmeter reading as a NB-IoT message.
void loop() {
  if (digitalRead(BUTTON) == LOW) {
    handleButtonPress();
    delay(200);
  }
  delay(100);
}
