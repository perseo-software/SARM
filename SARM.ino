#include "Adafruit_BMP280.h"

#define pinPotenciometro A0
#define pinEnableMotor 5

Adafruit_BMP280 bmp; // I2C

float presion; // Almacena la presion atmosferica (Pa)
float temperatura; // Almacena la temperatura (oC)
int altitud; // Almacena la altitud (m) (se puede usar variable float)

void setup(){
    Serial.begin(9600);
    if (!bmp.begin()) {
        Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
        while (1);
    }
    pinMode(pinEnableMotor, OUTPUT);
    analogWrite(pinEnableMotor, 0);
}

void loop(){
    // Lee valores del sensor:
    presion = bmp.readPressure()/100;
    temperatura = bmp.readTemperature();
    altitud = bmp.readAltitude (1015); // Ajustar con el valor local

    //Serial.print(F("Presion: "));
    Serial.println(presion);
    //Serial.print(" hPa");
    //Serial.print("\t");
    //Serial.print(("Temp: "));
    //Serial.print(temperatura);
    //Serial.print(" *C");
    //Serial.print("\t");
    //Serial.print("Altitud (aprox): ");
    //Serial.print(altitud); 
    //Serial.println(" m");
    delay(1000);
    
    int val = analogRead(pinPotenciometro);
    analogWrite(pinEnableMotor, val/4);
    //Serial.println(val);    
}   