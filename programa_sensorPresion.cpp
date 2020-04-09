#include "programa_sensorPresion.h"

int Programa_sensorPresion::begin_(){
    
    if (!bmp.begin()) {
        Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
        return 1;
    }
    return 0;
}

void Programa_sensorPresion::getMuestra(){
    
    unsigned long now = millis();
    if (now - last_t_bmp >= refresh_rate_bmp){
        presion = bmp.readPressure()/1000;  //En kPa
        temperatura = bmp.readTemperature();
        new_pressure = true;
        last_t_bmp = now;
    }
}