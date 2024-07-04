const int buzzerPin = 8;

void setup() {
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  // Genera un tono de 1000 Hz (1 kHz) durante 500 ms
  tone(buzzerPin, 1000); // 1000 Hz
  delay(500);
  
  // Detiene el tono durante 500 ms
  noTone(buzzerPin);
  delay(500);
  
  // Genera un tono de 2000 Hz (2 kHz) durante 500 ms
  tone(buzzerPin, 2000); // 2000 Hz
  delay(500);
  
  // Detiene el tono durante 500 ms
  noTone(buzzerPin);
  delay(500);
  
  // Genera un tono de 500 Hz durante 500 ms
  tone(buzzerPin, 500); // 500 Hz
  delay(500);
  
  // Detiene el tono durante 500 ms
  noTone(buzzerPin);
  delay(500);
}
