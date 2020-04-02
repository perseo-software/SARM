#include "Adafruit_BMP280.h"

#define pinPotenciometro A0
#define pinEnableMotor 5
#define pinPizzometro1 9
#define pinPizzometro2 10

Adafruit_BMP280 bmp; // I2C

float presion; // Almacena la presion atmosferica (Pa)
float temperatura; // Almacena la temperatura (oC)
int altitud; // Almacena la altitud (m) (se puede usar variable float)

bool pizzometro1_state;
bool pizzometro2_state;

void setup(){
    Serial.begin(9600);
    if (!bmp.begin()) {
        Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
        while (1);
    }
    pinMode(pinEnableMotor, OUTPUT);
    analogWrite(pinEnableMotor, 0);

    pinMode(pinPizzometro1, INPUT);
    pinMode(pinPizzometro2,INPUT);
}

void loop(){
    // Lee valores del sensor:
    presion = bmp.readPressure()/100;
    temperatura = bmp.readTemperature();
    altitud = bmp.readAltitude (1015); // Ajustar con el valor local

    pizzometro1_state = digitalRead(pinPizzometro1);
    pizzometro2_state = digitalRead(pinPizzometro2);

    Serial.print(pizzometro1_state);
    Serial.print(',');
    Serial.println(pizzometro2_state);

    Serial.print(F("Presion: "));
    Serial.print(presion);
    Serial.print(" hPa");
    Serial.print("\t");
    Serial.print(("Temp: "));
    Serial.print(temperatura);
    Serial.println(" *C");
    delay(100);
    
    int val = analogRead(pinPotenciometro);
    analogWrite(pinEnableMotor, val/4);
    //Serial.println(val);

}   