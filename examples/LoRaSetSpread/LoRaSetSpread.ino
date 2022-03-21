/**
 **************************************************
 *
 * @file        LoRaSetSpread.ino
 * @brief       Sends a message every half second, and polls continually
 *				for new incoming messages. Sets the LoRa radio's spreading factor.
 *
 *              Spreading factor affects how far apart the radio's transmissions
 *              are, across the available bandwidth. Radios with different spreading
 *              factors will not receive each other's transmissions. This is one way you
 *              can filter out radios you want to ignore, without making an addressing scheme.
 *
 *              Spreading factor affects reliability of transmission at high rates, however,
 *              so avoid a hugh spreading factor when you're sending continually.
 *              
 *
 *
 *
 * @authors    	Tom Igoe
 *
 * Modified by: Soldered for use with www.soldered.com/333157 , www.soldered.com/333158
 *
 ***************************************************/

///                 Arduino      RFM95/96/97/98
///                 GND----------GND   (ground in)
///                 3V3----------3.3V  (3.3V in)
/// interrupt 0 pin D2-----------DIO0  (interrupt request out)
///          SS pin D10----------NSS   (CS chip select in)
///         SCK pin D13----------SCK   (SPI clock in)
///        MOSI pin D11----------MOSI  (SPI Data in)
///        MISO pin D12----------MISO  (SPI Data out)
/// This is pinout for Arduino Uno, if you are using other MCU, use SPI pins
///and Interrupt pin 0, if Dasduino ConnectPlus is used
/// (or any other ESP32 board) use pins(SS=27, RST=2, DIO0=32, MISO=33, MOSI=25,SCK=26)

#define LORA		//Specify that module will be used for LoRa to LoRa communication
#include <SPI.h>              // include libraries
#include "LoRa-SOLDERED.h"

const int csPin = 7;          // LoRa radio chip select
const int resetPin = 6;       // LoRa radio reset
const int irqPin = 1;         // Change for your board; must be a hardware interrupt pin

byte msgCount = 0;            // Count of outgoing messages
int interval = 2000;          // Interval between sends
long lastSendTime = 0;        // Time of last packet send

void setup() {
  Serial.begin(9600);                   // Initialize serial
  while (!Serial);

  Serial.println("LoRa Duplex - Set spreading factor");

  // Override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(csPin, resetPin, irqPin); // set CS, reset, IRQ pin

  if (!LoRa.begin(868E6)) {             // Initialize ratio at 868 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // If failed, do nothing
  }

  LoRa.setSpreadingFactor(8);           // Ranges from 6-12,default 7 see API docs
  Serial.println("LoRa init succeeded.");
}

void loop() {
  if (millis() - lastSendTime > interval) {
    String message = "HeLoRa World! ";   // Send a message
    message += msgCount;
    sendMessage(message);
    Serial.println("Sending " + message);
    lastSendTime = millis();            // Timestamp the message
    interval = random(2000) + 1000;    // 2-3 seconds
    msgCount++;
  }

  // Parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
}

void sendMessage(String outgoing) {
  LoRa.beginPacket();                   // Start packet
  LoRa.print(outgoing);                 // Add payload
  LoRa.endPacket();                     // Finish packet and send it
  msgCount++;                           // Increment message ID
}

void onReceive(int packetSize)
{
  if (packetSize == 0) return;          // If there's no packet, return

  // Read packet header bytes:
  String incoming = "";

  while (LoRa.available()) //Read from incoming buffer
  {
    incoming += (char)LoRa.read();
  }

  Serial.println("Message: " + incoming);
  Serial.println("RSSI: " + String(LoRa.packetRssi()));
  Serial.println("Snr: " + String(LoRa.packetSnr()));
  Serial.println();
}

