#include "Adafruit_BMP280.h"
//#include <Wire.h>   //Ya incluida en Adafruit_BMP280.h
#include <LiquidCrystal_I2C.h>

#define pinPotenciometro A0
#define pinEnableMotor 5

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
Adafruit_BMP280 bmp; // I2C

float presion; // Almacena la presion atmosferica (Pa)
float temperatura; // Almacena la temperatura (oC)
int altitud; // Almacena la altitud (m) (se puede usar variable float)

unsigned long now;
unsigned long last_lcd = 0;
#define refresh_lcd_rate 500    // tasa de refresco en ms

void setup(){
    lcd.begin(16,2);
    lcd.backlight();
    lcd.setCursor(2,0);
    lcd.print("*** SARM ****");
    lcd.setCursor(0,1); 
    lcd.print("-booting-");

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

    Serial.print(F("Presion: "));
    Serial.print(presion);
    Serial.print(" hPa");
    Serial.print("\t");
    Serial.print(("Temp: "));
    Serial.print(temperatura);
    Serial.println(" *C");
    
    int val = analogRead(pinPotenciometro);
    analogWrite(pinEnableMotor, val/4);

    //LCD update
    now = millis();
    if (now - last_lcd >= refresh_lcd_rate){
        last_lcd = now;

        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Pres[hPa]:");
        lcd.setCursor(0,1);
        lcd.print("Vel[RPM]:");

        lcd.setCursor(10,0);
        lcd.print(presion);
        lcd.setCursor(10,1);
        lcd.print(val/4);
    }
    
}