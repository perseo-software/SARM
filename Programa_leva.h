#pragma once

#define pinEnableMotor 3
#define pinPizzometro1 5
#define pinPizzometro2 2

class Programa_leva{

private:
    bool pizzometro1_state;
    bool pizzometro2_state;
    bool last_pizzometro1_state;
    bool last_pizzometro2_state;

public:
    void setup_();
    void updateSensors();
};