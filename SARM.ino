#include "programa_sensorPresion.h"
#include <LiquidCrystal_I2C.h>

#define pinPotenciometro A1
#define pinEnableMotor 3
#define pinPizzometro1 5
#define pinPizzometro2 2

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
Programa_sensorPresion prog_pSensor;

//Informacion por Serial
unsigned long last_t_serial = 0;
#define refresh_rate_serial 100    // tasa de refresco en ms


unsigned long now;
unsigned long last_lcd = 0;
#define refresh_lcd_rate 500    // tasa de refresco en ms

bool pizzometro1_state;
bool pizzometro2_state;
bool last_pizzometro1_state;
bool last_pizzometro2_state;

bool error_pSensor;

enum State{
   booting,
   calibration,
   active,
   colocacion_motor,
   error_BMP
};
enum MotorState{
   espera,
   stop,
   gira,
   no_hacer_nada
};

State program_state;
unsigned long t_start_calib;
MotorState motor_state;

void setup(){
    program_state = booting;

    lcd.begin(16,2);
    lcd.backlight();
    lcd.setCursor(2,0);
    lcd.print("**** SARM ****");
    lcd.setCursor(0,1); 
    lcd.print("-booting-");

    Serial.begin(115200);
    error_pSensor = prog_pSensor.begin_();
    
    
    pinMode(pinEnableMotor, OUTPUT);
    analogWrite(pinEnableMotor, 0);

    pinMode(pinPizzometro1, INPUT);
    pinMode(pinPizzometro2,INPUT);

    pizzometro1_state = digitalRead(pinPizzometro1);
    pizzometro2_state = digitalRead(pinPizzometro2);

    program_state = colocacion_motor;
    
}

void loop(){
    
    now = millis();
    // Lee valores del sensor a 10Hz
    prog_pSensor.getMuestra();
    
    last_pizzometro1_state = pizzometro1_state;
    last_pizzometro2_state = pizzometro2_state;
    pizzometro1_state = digitalRead(pinPizzometro1);
    pizzometro2_state = digitalRead(pinPizzometro2);

    int pot = analogRead(pinPotenciometro);

    // Program State
    if (program_state == calibration){
        bool calib_done = prog_pSensor.doCalibration();
        if (calib_done){
            program_state = active;
        }
    }

    else if (program_state == active){
        prog_pSensor.run();
        if (prog_pSensor.nRespiraciones >= 3  && pot > 10){
            if (prog_pSensor.sector == -1){
                motor_state = gira;
            }
            else {
                if (motor_state == gira){
                    motor_state = espera;
                }
            }
        }
        else {
            motor_state = stop;
        }
    }

    else if (program_state == colocacion_motor){
    // Motor control
        motor_state = no_hacer_nada;
        if (pizzometro1_state && !last_pizzometro1_state){
            analogWrite(pinEnableMotor, 0);
            motor_state = stop;
            program_state = calibration;
            t_start_calib = millis();
        }
        else{
            analogWrite(pinEnableMotor, 100);
        }
    }
    

    //Programa motor
    if (motor_state == espera){
        if (pizzometro1_state && !last_pizzometro1_state){
            analogWrite(pinEnableMotor, 0);
            motor_state = stop;
        }
        else{
            analogWrite(pinEnableMotor, 100);
        }
    }
    else if (motor_state == stop){
            analogWrite(pinEnableMotor, 0);
    }
    else if (motor_state == gira){
        analogWrite(pinEnableMotor, 255);
    }
    //else if (motor_state == no_hacer_nada){}


    //Envio informacion por Serial

    now = millis();
    if (now - last_t_serial >= refresh_rate_serial){
        last_t_serial = now;

        Serial.print("#");
        Serial.print(millis()/1000.0,2);
        Serial.print(",");
        Serial.print(presion,4);
        Serial.print(",");
        Serial.print(relative_pressure,4);
        Serial.print(",");
        Serial.print(d_pressure,4);
        Serial.print(",");
        Serial.print(sector);
        Serial.print(",");
        Serial.print(program_state);
        Serial.print(",");
        Serial.print(motor_state);
        Serial.print(",");
        Serial.print(pizzometro1_state);
        Serial.print(",");
        Serial.println(pizzometro2_state);
        //Serial.print(" kPa");
        //Serial.print("\t");
        //Serial.print(("Temp: "));
        //Serial.print(temperatura);
        //Serial.println(" *C");
    }

    //LCD update
    now = millis();
    if (now - last_lcd >= refresh_lcd_rate){
        last_lcd = now;
        if (program_state == active){
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Pres[kPa]:");
            lcd.setCursor(0,1);
            lcd.print("Vel[RPM]:");

            lcd.setCursor(10,0);
            lcd.print(presion);
            lcd.setCursor(10,1);
            lcd.print(pot/4);
        }
        else if (program_state == calibration){
            lcd.clear();
            lcd.setCursor(2,0);
            lcd.print("**** SARM ****");
            lcd.setCursor(0,1); 
            lcd.print("-calibration-");
        }
        else if (program_state == colocacion_motor){
            lcd.clear();
            lcd.setCursor(2,0);
            lcd.print("**** SARM ****");
            lcd.setCursor(0,1); 
            lcd.print("-colocacion motor-");
        }
    }
    
}
