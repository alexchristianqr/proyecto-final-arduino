const int triggerPin = 9;
const int echoPin = 10;
long duration;
int distance;

void setup() {
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  // Limpia el pin Trigger
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);

  // Genera un pulso de 10us en el pin Trig
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  // Lee el tiempo en el pin Echo
  duration = pulseIn(echoPin, HIGH);

  // Calcula la distancia en centímetros
  distance = duration * 0.034 / 2;

  // Imprime la distancia en el monitor serie
  Serial.print("Distancia: ");
  Serial.print(distance);
  Serial.println(" cm");

  delay(500);
}
