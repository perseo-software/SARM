#include "Programa_leva.h"

void Programa_leva::setup(){
    pinMode(pinEnableMotor, OUTPUT);
    analogWrite(pinEnableMotor, 0);

    pinMode(pinPizzometro1, INPUT);
    pinMode(pinPizzometro2,INPUT);

    last_pizzometro1_state = digitalRead(pinPizzometro1);
    last_pizzometro2_state = digitalRead(pinPizzometro2);
}

void Programa_leva::updateSensors(){
    last_pizzometro1_state = pizzometro1_state;
    last_pizzometro2_state = pizzometro2_state;
    
    pizzometro1_state = digitalRead(pinPizzometro1);
    pizzometro2_state = digitalRead(pinPizzometro2);
}

void Programa_leva::run(){

    Serial.println("LEVA.RUN");

    if (estado == stop){
        Serial.println("estado=stop");
        analogWrite(pinEnableMotor, 0);
    }
    else if (estado == giro){
        analogWrite(pinEnableMotor, 255);
    }
    else if (estado == colocacion){
        Serial.println("estado=colocacion");
        if (pizzometro1_state && !last_pizzometro1_state){
            analogWrite(pinEnableMotor, 0);
            estado = waitPos;
            Serial.println("LEVA.2waitPos_state");
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
    Serial.println("LEVA.STOP_MOTOR");
    estado = stop;
}

int Programa_leva::go2waitPos(){
    Serial.println("go2waitPos");
    if (estado != waitPos){
        Serial.println("a estado colocacion");
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
    Serial.println(pizzometro2_state);
}