#pragma once

#define pinEnableMotor 3
#define pinPizzometro1 5
#define pinPizzometro2 2

class Programa_leva{

    bool pizzometro1_state;
    bool pizzometro2_state;
    bool last_pizzometro1_state;
    bool last_pizzometro2_state;

    enum State{stop, giro, colocacion, waitPos};
    
public:
    State estado;
    void setup_();
    void updateSensors();
    void run();
    void giraMotor();
    void stopMotor();
    void go2waitPos();
};