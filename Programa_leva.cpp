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

void Programa_leva::run(){

    if (estado == stop){
        analogWrite(pinEnableMotor, 0);
    }
    else if (estado == giro){
        analogWrite(pinEnableMotor, 255);
    }
    else if (estado == colocacion){
        if (pizzometro1_state && !last_pizzometro1_state){
            analogWrite(pinEnableMotor, 0);
            estado = waitPos;
        }
        else{
            analogWrite(pinEnableMotor, 100);
        }
    }
    else if (estado == waitPos){
        analogWrite(pinEnableMotor, 0);
    }
}

void Programa_leva::giraMotor(){
    estado = giro;
}

void Programa_leva::stopMotor(){
    estado = stop;
}

int Programa_leva::go2waitPos(){
    if (estado != waitPos){
        estado = colocacion;
        return 0;
    }
    else{
        return 1;
    }
}

void Programa_leva::printCSV(){
    Serial.print(estado);
    Serial.print(",");
    Serial.print(pizzometro1_state);
    Serial.print(",");
    Serial.print(pizzometro2_state);
}