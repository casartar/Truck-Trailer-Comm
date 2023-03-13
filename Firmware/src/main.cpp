#include <Arduino.h>
#include <Wire.h>
#include <TLC59116.h>

TLC59116 board1(0);

uint8_t pinArray[16] = {0, 5, 9, 10, 12, 13, 14, 15, 16, 17, 18, 19, 23, 25, 26, 27};

void setup() {
	board1.begin();
    for (uint8_t i = 0; i < 16; i++)
    {
        pinMode(pinArray[i], INPUT_PULLUP);
    }
}



void loop() {

    for (uint8_t i = 0; i < 16; i++)
    {
        board1.analogWrite(i, digitalRead(pinArray[i]) ? 20 : 0);
    }
	delay(10);
}