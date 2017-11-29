#include <Arduino.h>
#include <DynamixelSerial.h>
#include <SoftwareSerial.h>

#define DIRECTION_PIN     2
#define SSerial_RX        10
#define SSerial_TX        11
#define Serial_Baud       9600
#define glavnaSerial_Baud 9600
#define Dynamixel_Baud    1000000

char buffer[8];                   // buffer length
bool buffer_read = false;

char tool_id; int item_id; char function_id; unsigned char value1; unsigned char value2;

SoftwareSerial glavna_serial(SSerial_RX, SSerial_TX);

bool debug = false;

void setupz() {

/*Initializing Serial or Dynamixel comunication*/
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
// If connection whit "glavna" is established, get data (bytes)
  if(glavna_serial.available() >= 8) {
    glavna_serial.readBytes(buffer, 8);
    buffer_read = true;
  }
// Assinging values to the buffers
  if(buffer_read) {
    tool_id     = buffer[0];
    item_id     = buffer[1];
    function_id = buffer[2];
    value1      = buffer[3];
    value2      = buffer[4];
// Print the gotten buffers if debuging
    if(debug) { Serial.println(tool_id); Serial.println(item_id); Serial.println(function_id); Serial.println(value1); Serial.println(value2); Serial.println("-----------------------"); }
// If not run servos
    if(!debug) {
      if(tool_id == 'a') {                                //If Dynamixel is Initialized
        if(function_id == 'm') {                          // start "move" protocol
          int position = (int) 1024 / (255 / value1);     //   Coverting into position
          int speed    = (int) 1024 / (255 / value2);     //   and speed

          Dynamixel.moveSpeed(item_id, position, speed);  //Initialize Servo

          glavna_serial.write(1);                         //return to "glavna"

        } else if(function_id == 'l') {                   // running led-status
          if(value1 == 1) {
            Dynamixel.ledStatus(item_id, true);
          } else {
            Dynamixel.ledStatus(item_id, false);
          }

          glavna_serial.write(1);                        // return to "glavna"
        } else if(function_id == 'M') {                  //running "movement" check
            int moving;
            moving = Dynamixel.moving(item_id);          //getting status of "movement"

            glavna_serial.write(1);                      //retrun to "glavna"

            glavna_serial.write((unsigned char)moving);  // return movement status to "glavna"


        }

      }

    } else {
/* print position and speed goten from "glavna" if debug is Initialized */

      int position = (int) 1024 / (255 / value1);
      int speed    = (int) 1024 / (255 / value2);

      Serial.println(position);
      Serial.println(speed);
      Serial.println("-----------------------");

    }

    buffer_read = false;                               //closeing down buffer handleing
  }
}
