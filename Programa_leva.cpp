#include "Programa_leva.h"

void Programa_leva::setup(){
    pinMode(pinEnableMotor, OUTPUT);
    analogWrite(pinEnableMotor, 0);

    pinMode(pinPizzometro1, INPUT);
    pinMode(pinPizzometro2,INPUT);

    pizzometro1_state = digitalRead(pinPizzometro1);
    pizzometro2_state = digitalRead(pinPizzometro2);
}

void Programa_leva::updateSensors(){
    last_pizzometro1_state = pizzometro1_state;
    last_pizzometro2_state = pizzometro2_state;
    
    pizzometro1_state = digitalRead(pinPizzometro1);
    pizzometro2_state = digitalRead(pinPizzometro2);
}