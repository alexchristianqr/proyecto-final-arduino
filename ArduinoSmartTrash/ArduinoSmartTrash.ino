#include <Servo.h>
#include <SoftwareSerial.h>

// Pines LED
int ledVerde = 13;     // Tacho con espacio
int ledAmarillo = 12;  // Tapa abierta
int ledRojo = 8;       // Tacho lleno

// Pines RGB
int rgbRojo = 11;
int rgbAzul = 10;
int rgbVerde = 9;  // Cambio de color según luz del ambiente

int piezoBuzzer = 3;  // Alarma sonido
int microServo = 4;   // Controlador de la tapa

// Pines utilizados
int TRIGGER = 5;
int ECHO = 6;

// Variables
Servo servo;

// Constantes
const float sonido = 34300.0;  // Velocidad del sonido en cm/s
const float dCarton = 10.0;    // Distancia de 10 centimetros: 5-10 pintar amarillo
const float dPapel = 5.0;      // Distancia de 5 centimetros: 0-5 pintar verde

void setup() {
  // Inicializar componentes
  Serial.begin(9600);

  // Servo motor
  servo.write(180);
  servo.attach(microServo);

  // Pines LED
  pinMode(ledVerde, OUTPUT);
  pinMode(ledAmarillo, OUTPUT);
  pinMode(ledRojo, OUTPUT);

  // Pines RGB
  pinMode(rgbRojo, OUTPUT);
  pinMode(rgbVerde, OUTPUT);
  pinMode(rgbAzul, OUTPUT);

  // Pin buzzer sonido
  pinMode(piezoBuzzer, OUTPUT);

  // Pines para el sensor de ultrasonido
  pinMode(ECHO, INPUT);
  pinMode(TRIGGER, OUTPUT);

  apagarLEDs();
  encenderLedRojo();
}

void loop() {
  // startSmartTrash();
}

// Iniciar tacho de basura inteligente
void startSmartTrash() {
  // Preparar el sensor de ultrasonidos
  iniciarTrigger();

  // Obtener la distancia
  float distancia = calcularDistancia();

  // Apagar todos los LEDs
  apagarLEDs();

  // Lanzar alerta si estamos dentro de la distancia deseada
  if (distancia <= dCarton) {
    alertas(distancia);  // Lanzamos alertas
  } else {
    cerrarTacho();
    encenderLedRojo();
  }
}

// Iniciar la secuencia del Trigger para comenzar a medir
void iniciarTrigger() {
  // Ponemos el Trigger en estado bajo y esperamos 2 ms
  digitalWrite(TRIGGER, LOW);
  delayMicroseconds(2);

  // Ponemos el pin Trigger a estado alto y esperamos 10 ms
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER, LOW);
}

// Calcular la distancia a la que se encuentra un objeto.
float calcularDistancia() {

  // La función pulseIn obtiene el tiempo que tarda en cambiar entre estados, en este caso a HIGH
  unsigned long tiempo = pulseIn(ECHO, HIGH);

  // Obtenemos la distancia en cm, hay que convertir el tiempo en segudos ya que está en microsegundos. Por eso se multiplica por 0.000001
  float tiempoEnSegundos = tiempo * 0.000001;  // Microsegundos
  float distancia = (tiempoEnSegundos * sonido) / 2.0;
  // float distancia = (tiempo / 2) / 29.1;

  Serial.print("distancia capturada: ");
  Serial.print(distancia);
  Serial.print(" cm");
  Serial.println();

  // Determinar si es papel o carton
  /*if (distancia < PAPEL_GROSOR) {
    Serial.println("Material: PAPEL");
  } else if (distancia >= PAPEL_GROSOR && distancia < CARTON_GROSOR) {
    Serial.println("Material: CARTON");
  } else {
    Serial.println("Material desconocido");
  }*/

  delay(1000);

  return distancia;
}

// Apagar todos los LEDs
void apagarLEDs() {
  digitalWrite(ledVerde, LOW);
  digitalWrite(ledAmarillo, LOW);
}

// Comprobar si hay que lanzar alguna alerta visual o sonora
void alertas(float distancia) {
  if (distancia >= 0 && distancia <= dPapel) {  // >=0 && <=5
    apagarLedRojo();
    Serial.print("dPapel: ");
    Serial.print(dPapel);
    Serial.print(" cm");
    Serial.println();
    encenderSoloLedAmarillo();  // Encendemos el LED amarillo
    abrirTacho();
    tone(piezoBuzzer, 2500, 50);
  } else if (distancia > dPapel && distancia <= dCarton) {  // >5 && <= 10
    apagarLedRojo();
    Serial.print("dCarton: ");
    Serial.print(dCarton);
    Serial.print(" cm");
    Serial.println();
    encenderSoloLedVerde();  // Encendemos el LED verde
    abrirTacho();
    tone(piezoBuzzer, 2000, 50);
  }
}

void abrirTacho() {
  servo.write(90);
  delay(600);
}

void cerrarTacho() {
  servo.write(180);
  delay(600);
}

void apagarRGB() {
  digitalWrite(rgbRojo, 0);
  digitalWrite(rgbVerde, 0);
  digitalWrite(rgbAzul, 0);
}

void magentaRGB() {
  digitalWrite(rgbRojo, 255);
  digitalWrite(rgbVerde, 0);
  digitalWrite(rgbAzul, 255);
}

void encenderLedRojo() {
  digitalWrite(ledRojo, HIGH);
}

void apagarLedRojo() {
  digitalWrite(ledRojo, LOW);
}

void encenderSoloLedAmarillo() {
  apagarLedRojo();
  digitalWrite(ledVerde, LOW);
  digitalWrite(ledAmarillo, HIGH);
}

void encenderSoloLedVerde() {
  apagarLedRojo();
  digitalWrite(ledAmarillo, LOW);
  digitalWrite(ledVerde, HIGH);
}
