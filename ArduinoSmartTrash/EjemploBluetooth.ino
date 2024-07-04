#include <SoftwareSerial.h>

int pinRX = 10;
int pinTX = 11;

SoftwareSerial BTSerial(pinRX, pinTx);

const int ledRojo = 2;
const int ledVerde = 3;
const int ledAmarillo = 4;

void setup() {
  Serial.begin(9600);

  pinMode(ledRojo, OUTPUT);
  pinMode(ledVerde, OUTPUT);
  pinMode(ledAmarillo, OUTPUT);

  BTSerial.begin(9600);
}

void loop() {
  if (!BTSerial.available()) {
    return;
  }

  char command = BTSerial.read();

  switch (command) {
    case "rojo":
      digitalWrite(ledRojo, HIGH);  // ENCENDER LED
      digitalWrite(ledVerde, LOW);
      digitalWrite(ledAmarillo, LOW);
      break;
    case "verde":
      digitalWrite(ledVerde, HIGH);  // ENCENDER LED
      digitalWrite(ledRojo, LOW);
      digitalWrite(ledAmarillo, LOW);
      break;
    case "amarillo":
      digitalWrite(ledAmarillo, HIGH);  // ENCENDER LED
      digitalWrite(ledVerde, LOW);
      digitalWrite(ledRojo, LOW);
      break;
    default:
      break;
  }
}
