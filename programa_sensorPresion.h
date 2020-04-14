#pragma once
#include "Arduino.h"
#include "Adafruit_BMP280.h"

class Programa_sensorPresion{

    Adafruit_BMP280 bmp; // I2C
    
    int refresh_rate_bmp = 100;     // tasa de refresco en ms
    unsigned long last_t_bmp = 0;
    bool new_pressure = false;
    float mean_pressure = 0;
    int nMuestras = 0;
    float relative_pressure;
    float last_pressure;
    float last_relat_pressure = 0;
    float d_pressure;
    float last_d_pressure = 0;

    //Respiraciones
    int last_sector = 0;
    bool start_respiracion = false;
    unsigned long t_inicio_resp;
    unsigned long t_ciclo;

public:
    float presion; // Almacena la presion atmosferica (kPa)
    float temperatura; // Almacena la temperatura (oC)
    
    unsigned long t_start_calib;
    int sector = 0;
    int nRespiraciones = 0;

    int begin_();
    void getMuestra();
    int doCalibration();
    void run();
    void printCSV();
};