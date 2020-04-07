#include "Adafruit_BMP280.h"
//#include <Wire.h>   //Ya incluida en Adafruit_BMP280.h
#include <LiquidCrystal_I2C.h>

#define pinPotenciometro A1
#define pinEnableMotor 3
#define pinPizzometro1 5
#define pinPizzometro2 2

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
Adafruit_BMP280 bmp; // I2C

float presion; // Almacena la presion atmosferica (Pa)
float temperatura; // Almacena la temperatura (oC)
int altitud; // Almacena la altitud (m) (se puede usar variable float)
unsigned long last_t_bmp = 0;
#define refresh_rate_bmp 100    // tasa de refresco en ms
bool new_pressure;
float mean_pressure = 0;
int nMuestras = 0;
float relative_pressure;
float last_pressure;
float last_relat_pressure = 0;
float d_pressure;
float last_d_pressure = 0;

//Respiraciones
int sector = 0;
int last_sector = 0;
bool start_respiracion = false;
int nRespiraciones = 0;
unsigned long t_inicio_resp;
unsigned long t_ciclo;


unsigned long now;
unsigned long last_lcd = 0;
#define refresh_lcd_rate 500    // tasa de refresco en ms

bool pizzometro1_state;
bool pizzometro2_state;

enum State{
   booting,
   calibration,
   active,
   idle
};

State program_state;
unsigned long t_start_calib;

void setup(){
    program_state = booting;

    lcd.begin(16,2);
    lcd.backlight();
    lcd.setCursor(2,0);
    lcd.print("*** SARM ****");
    lcd.setCursor(0,1); 
    lcd.print("-booting-");

    Serial.begin(115200);
    if (!bmp.begin()) {
        Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
        while (1);
    }
    
    pinMode(pinEnableMotor, OUTPUT);
    analogWrite(pinEnableMotor, 0);

    pinMode(pinPizzometro1, INPUT);
    pinMode(pinPizzometro2,INPUT);

    program_state = calibration;
    t_start_calib = millis();
    
}

void loop(){
    
    now = millis();
    // Lee valores del sensor a 10Hz
    if (now - last_t_bmp >= refresh_rate_bmp){
        presion = bmp.readPressure()/1000;  //En kPa
        temperatura = bmp.readTemperature();
        new_pressure = true;
        last_t_bmp = now;
    }
    
    pizzometro1_state = digitalRead(pinPizzometro1);
    pizzometro2_state = digitalRead(pinPizzometro2);

    int val = analogRead(pinPotenciometro);

    // Program State
    if (program_state == calibration){
        if (millis() - t_start_calib < 2000 && newPresure){
            mean_pressure += presion;
            nMuestras++;
            new_pressure = false;
        }
        else if (millis() - t_start_calib > 2000){
            mean_pressure = mean_pressure / nMuestras;
            Serial.println("Calibración realizada\r\nPresion atm: ");
            Serial.print(mean_pressure);
            program_state = active;
        }
    }

    else if (program_state == active){
        if (new_pressure){
            // Calculo de la presion relativa
            relative_pressure = presion - mean_pressure;

            //Calculo del diferencial de presion
            float alpha = 0.5;
            d_pressure = last_d_pressure * (1-alpha) + (relative_pressure - last_relat_pressure) * alpha;
            last_relat_pressure = relative_pressure;

            // Deteccion de tramos
            float umbral = 0.0015;
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
            
            // Conteo respiraciones y tiempo promedio
            if (!start_respiracion){
                if (sector == -1 && last_sector == 0){
                    t_inicio_resp = now;
                    start_respiracion = true;
                }
            }
            else {
                if (sector == -1 && last_sector == 0){
                    t_ciclo = now - t_inicio_resp;
                    t_inicio_resp = now;
                    if (t_ciclo < 0.5){
                        Serial.print("Alarma: t_ciclo: ");
                        Serial.println(t_ciclo);
                    }
                    else {
                        nRespiraciones++;
                        Serial.print("Respiracion ");
                        Serial.print(nRespiraciones);
                        Serial.print(", t_ciclo: ");
                        Serial.println(t_ciclo);
                    }
                }
            }
        }
    }

    //Serial.print(pizzometro1_state);
    //Serial.print(',');
    //Serial.println(pizzometro2_state);

    //Serial.print(F("Presion: "));
    Serial.print(millis()/1000.0,3);
    Serial.print(",");
    Serial.println(presion);
    //Serial.print(" kPa");
    //Serial.print("\t");
    //Serial.print(("Temp: "));
    //Serial.print(temperatura);
    //Serial.println(" *C");
    
    // Motor control
    analogWrite(pinEnableMotor, val/4);

    //LCD update
    now = millis();
    if (now - last_lcd >= refresh_lcd_rate){
        last_lcd = now;

        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Pres[kPa]:");
        lcd.setCursor(0,1);
        lcd.print("Vel[RPM]:");

        lcd.setCursor(10,0);
        lcd.print(presion);
        lcd.setCursor(10,1);
        lcd.print(val/4);
    }
    
}
