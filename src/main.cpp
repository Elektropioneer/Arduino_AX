#include <Arduino.h>
#include <DynamixelSerial.h>
#include <SoftwareSerial.h>

#define DIRECTION_PIN     2
#define SSerial_RX        10
#define SSerial_TX        11
#define Serial_Baud       9600
#define glavnaSerial_Baud 9600
#define Dynamixel_Baud    1000000

char buffer[8];
bool buffer_read = false;

char tool_id; int item_id; char function_id; unsigned char value1; unsigned char value2;

SoftwareSerial glavna_serial(SSerial_RX, SSerial_TX);

bool debug = false;

void setupz() {

  if(debug) {
    Serial.begin(Serial_Baud);
  } else {
    Dynamixel.begin(Dynamixel_Baud, DIRECTION_PIN);
  }

  glavna_serial.begin(glavnaSerial_Baud);

}

void setup() {
  setupz();
}

void loop() {

  if(glavna_serial.available() >= 8) {
    glavna_serial.readBytes(buffer, 8);
    buffer_read = true;
  }

  if(buffer_read) {
    tool_id     = buffer[0];
    item_id     = buffer[1];
    function_id = buffer[2];
    value1      = buffer[3];
    value2      = buffer[4];

    if(debug) { Serial.println(tool_id); Serial.println(item_id); Serial.println(function_id); Serial.println(value1); Serial.println(value2); Serial.println("-----------------------"); }

    if(!debug) {
      if(tool_id == 'a') {
        if(function_id == 'm') {
          int position = (int) 1024 / (255 / value1);
          int speed    = (int) 1024 / (255 / value2);

          Dynamixel.moveSpeed(item_id, position, speed);

          glavna_serial.write(1);

        } else if(function_id == 'l') {
          if(value1 == 1) {
            Dynamixel.ledStatus(item_id, true);
          } else {
            Dynamixel.ledStatus(item_id, false);
          }

          glavna_serial.write(1);
        } else if(function_id == 'M') {
            int moving;
            moving = Dynamixel.moving(item_id);

            glavna_serial.write(1);

            glavna_serial.write((unsigned char)moving);

        } else if (function_id == 'p') {
          int position, calculated_pos;
          position = Dynamixel.readPosition(item_id);

          glavna_serial.write(1);

          calculated_pos = ((255 / (1024 / position)) * 300) / 255;

          glavna_serial.write((unsigned char)calculated_pos);
        }

      }

    } else {

      int position = (int) 1024 / (255 / value1);
      int speed    = (int) 1024 / (255 / value2);

      Serial.println(position);
      Serial.println(speed);
      Serial.println("-----------------------");

    }

    buffer_read = false;
  }
}
