const int buttonPin = 2;
const int ledPin = 13;

int buttonState = 0;

void setup() {
  pinMode(buttonPin, INPUT);  // ENTRADA
  pinMode(ledPin, OUTPUT);    // SALIDA
}

void loop() {
  // Lee el estado del botón
  buttonState = digitalRead(buttonPin);

  // Enciende el LED si el botón está presionado
  if (buttonState == HIGH) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
}
