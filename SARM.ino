#define pinPotenciometro A0

void setup(){
    Serial.begin(9600);

}

void loop(){
    int val = analogRead(pinPotenciometro);
    Serial.println(val);
}