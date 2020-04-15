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

int Programa_sensorPresion::doCalibration(){
    
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

void Programa_sensorPresion::run(){
    if (new_pressure){
        new_pressure = false;
        last_sector = sector;

        // Calculo de la presion relativa
        relative_pressure = presion - mean_pressure;

        //Calculo del diferencial de presion
        float alpha = 0.5;
        d_pressure = last_d_pressure * (1-alpha) + (relative_pressure - last_relat_pressure) * alpha;
        last_relat_pressure = relative_pressure;
        last_d_pressure = d_pressure;

        // Deteccion de tramos
        float umbral = 0.01;
        if (relative_pressure < umbral && relative_pressure > -umbral){
            if (d_pressure < umbral && d_pressure > -umbral){
                sector = 0;
            }
        }
        else if (relative_pressure < 0 && d_pressure < 0){
            sector = -1;
        }
        else if (relative_pressure < 0 && d_pressure > 0){
            sector = -2;
        }
        else if (relative_pressure > 0 && d_pressure > 0){
            sector = 1;
        }
        else if (relative_pressure > 0 && d_pressure < 0){
            sector = 2;
        }
        else{
            sector = 0;
        }

        // Conteo respiraciones y tiempo
        unsigned long now = millis();
        if (!start_respiracion){
            if (sector == -1 && last_sector == 0){
                t_inicio_resp = now;
                start_respiracion = true;
            }
        }
        else {
            if (sector == -1 && (last_sector == 0 || last_sector == 2)){
                t_ciclo = now - t_inicio_resp;
                t_inicio_resp = now;
                if (t_ciclo < 1000){
                    Serial.print("Alarma: t_ciclo: ");
                    Serial.println(t_ciclo);
                }
                else {
                    nRespiraciones++;
                    Serial.print("Respiracion ");
                    Serial.print(nRespiraciones);
                    Serial.print(", t_ciclo: ");
                    Serial.println(t_ciclo/1000.0,3);
                }
            }
        }
    }
}

void Programa_sensorPresion::printCSV(){
    Serial.print(presion,4);
    Serial.print(",");
    Serial.print(relative_pressure,4);
    Serial.print(",");
    Serial.print(d_pressure,4);
    Serial.print(",");
    Serial.print(sector);
    Serial.print(",");
    Serial.print(nRespiraciones);
}