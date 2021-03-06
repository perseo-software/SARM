#include <LiquidCrystal_I2C.h>
#include "programa_sensorPresion.h"
#include "Programa_leva.h"
#include "FS2012.h"

#define pinPotenciometro A1

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
Programa_sensorPresion prog_pSensor;
Programa_leva prog_leva;

//Informacion por Serial
unsigned long last_t_serial = 0;
#define refresh_rate_serial 100    // tasa de refresco en ms

unsigned long now;
unsigned long last_lcd = 0;
#define refresh_lcd_rate 500    // tasa de refresco en ms

bool error_pSensor;

enum State{
   booting,
   calibration,
   active,
   colocacion_motor,
   error_BMP
};

State program_state;

void setup(){
    program_state = booting;

    lcd.begin(16,2);
    lcd.backlight();
    lcd.setCursor(2,0);
    lcd.print("**** SARM ****");
    lcd.setCursor(0,1); 
    lcd.print("-booting-");

    Serial.begin(9600);
    error_pSensor = prog_pSensor.begin_();
    prog_leva.setup();

    program_state = colocacion_motor;    
}

void loop(){
    
    now = millis();
    prog_pSensor.getMuestra();
    prog_leva.updateSensors();
    int pot = analogRead(pinPotenciometro)/4;
    float flowRate = getFlowRate();
    flowRate *= 21; //Constante del conducto

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
                prog_leva.giraMotor();
            }
            else {
                prog_leva.go2waitPos();
            }
        }
        else {
            prog_leva.stopMotor();
        }
    }

    else if (program_state == colocacion_motor){
        bool arrived = prog_leva.go2waitPos();
        if (arrived){
            program_state = calibration;
            prog_pSensor.t_start_calib = millis();
        }
    }

    prog_leva.run();

    //Envio informacion por Serial
    //now = millis();
    //if (now - last_t_serial >= refresh_rate_serial){
    //    last_t_serial = now;
//
    //    Serial.print("#");
    //    Serial.print(millis()/1000.0,2);
    //    Serial.print(",");
    //    prog_pSensor.printCSV();
    //    Serial.print(",");
    //    Serial.print(program_state);
    //    Serial.print(",");
    //    prog_leva.printCSV();
    //    Serial.print(",");
    //    Serial.println(pot);
    //}

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
            lcd.print(prog_pSensor.presion);
            lcd.setCursor(10,1);
            lcd.print(pot);
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

    Serial.print("pot1.val=");
    Serial.print(pot);
    Serial.write(0xFF);
    Serial.write(0xFF);
    Serial.write(0xFF);

    Serial.print("presion.txt=\"");
    Serial.print(prog_pSensor.presion,4);
    Serial.print("\"");
    Serial.write(0xFF);
    Serial.write(0xFF);
    Serial.write(0xFF);

    Serial.print("caudal.txt=\"");
    Serial.print(flowRate,3);
    Serial.print("\"");
    Serial.write(0xFF);
    Serial.write(0xFF);
    Serial.write(0xFF);

    //Waveform
    int wf_presion = prog_pSensor.relative_pressure * 20.0 + 40.0;
    Serial.print("add 2,0,");
    Serial.print(wf_presion);
    Serial.write(0xFF);
    Serial.write(0xFF);
    Serial.write(0xFF);

    //Waveform
    int wf_caudal = flowRate * 255 / 40;
    Serial.print("add 5,0,");
    Serial.print(wf_caudal);
    Serial.write(0xFF);
    Serial.write(0xFF);
    Serial.write(0xFF);
}
