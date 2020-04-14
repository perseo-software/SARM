#pragma once
#include <Wire.h>

#define FS2012_ADDRESS 0x07

float getFlowRate(){
    Wire.requestFrom(FS2012_ADDRESS,2);
    if (Wire.available() >= 2){
        byte MSB = Wire.read();
        byte LSB = Wire.read();
        //Serial.print("Bytes:");
        //Serial.print(MSB,BIN);
        //Serial.print("|");
        //Serial.println(LSB,BIN);
        float SLPM = ((MSB << 8) + LSB) / 1000.0;
        return(SLPM);
    }
    return(-1);
}