#include <Servo.h>

const int pinServo = 8;
Servo servo;

void setup() {
  // Inicializar componentes
  Serial.begin(9600);

  // Servo motor
  servo.write(180);
  servo.attach(pinServo);
}

void loop() {
  // Mover 90° grados
  servo.write(90);
  delay(3000);

  // Regresar a 180° grados
  servo.write(180);
  delay(1000);
}
