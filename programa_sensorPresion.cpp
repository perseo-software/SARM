#include "programa_sensorPresion.h"

int Programa_sensorPresion::begin_(){
    
    if (!bmp.begin()) {
        Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
        return -1;
    }
    return 1;
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

void Programa_sensorPresion::doCalibration(){
    
    if (millis() - t_start_calib < 2000 && new_pressure){
        mean_pressure += presion;
        nMuestras++;
        new_pressure = false;
    }
    else if (millis() - t_start_calib > 2000){
        mean_pressure = mean_pressure / nMuestras;
        Serial.print("Calibracion realizada\r\nPresion atm: ");
        Serial.println(mean_pressure);
        return 1;
    }
    return 0;
}