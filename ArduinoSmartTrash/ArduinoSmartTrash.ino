#include <Servo.h>
#include <SoftwareSerial.h>
// #include <IRremote.h>

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
int pulsador = 2;     // Pulsar para vaciar tacho

/*const int ir = 7;  // adentro (nivel)
IRrecv irrecv(ir);
decode_results results;*/

// Pines utilizados
int TRIGGER = 5;
int ECHO = 6;

// Variables
Servo servo;
int nivelTachoLleno = 3;  // El tacho no esta lleno
unsigned long milisAct;   // Milisegundos

// Constantes
const float sonido = 34300.0;  // Velocidad del sonido en cm/s
const float d30 = 30.0;        // Distancia de 30 centimetros: pintar verde
const float d20 = 20.0;        // Distancia de 20 centimetros: pintar amarillo
const float d10 = 10.0;        // Distancia de 10 centimetros: pintar rojo

// Materiales
const float PAPEL_GROSOR = 5.0;    // Groso promedio de papel
const float CARTON_GROSOR = 10.0;  // Groso promedio de cartón


void setup() {
  // Inicializar componentes
  Serial.begin(9600);
  /*irrecv.enableIRIn();  // Inicializa el receptor IR
  Serial.println("Listo para recibir señales IR");*/

  // Servo motor
  servo.write(0);
  servo.attach(microServo);

  // Pines LED
  pinMode(ledVerde, OUTPUT);
  pinMode(ledAmarillo, OUTPUT);
  pinMode(ledRojo, OUTPUT);

  // Pines RGB
  pinMode(rgbRojo, OUTPUT);
  pinMode(rgbVerde, OUTPUT);
  pinMode(rgbAzul, OUTPUT);

  // Pin sensor infrarojo
  // pinMode(ir, INPUT);

  // Pin buzzer sonido
  pinMode(piezoBuzzer, OUTPUT);

  // Pines para el sensor de ultrasonido
  pinMode(ECHO, INPUT);
  pinMode(TRIGGER, OUTPUT);

  // Pin boton pulsador
  pinMode(pulsador, INPUT);

  milisAct = 0;
  apagarLEDs();
}

void loop() {
  //start();
}

void start() {
  // Preparar el sensor de ultrasonidos
  iniciarTrigger();

  // Obtener la distancia
  float distancia = calcularDistancia();

  // Apagar todos los LEDs
  apagarLEDs();

  // Lanzar alerta si estamos dentro de la distancia deseada
  if (distancia <= d30) {
    alertas(distancia);  // Lanzamos alertas
  } else {
    servo.write(0);
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
  digitalWrite(ledRojo, LOW);
}

// Comprobar si hay que lanzar alguna alerta visual o sonora
void alertas(float distancia) {
  if (distancia >= d20 && distancia < d30) {  // >=20 and <30

    Serial.print("d30: ");
    Serial.print(d30);
    Serial.print(" cm");
    Serial.println();
    abrirTacho();
    digitalWrite(ledVerde, HIGH);  // Encendemos el LED verde
    tone(piezoBuzzer, 2000, 50);

  } else if (distancia > d10 && distancia < d20) {  // >10 AND <20

    Serial.print("d20: ");
    Serial.print(d20);
    Serial.print(" cm");
    Serial.println();
    abrirTacho();
    digitalWrite(ledAmarillo, HIGH);  // Encendemos el LED amarillo
    tone(piezoBuzzer, 2500, 50);


  } else if (distancia <= d10) {  // <=10

    Serial.print("d10: ");
    Serial.print(d10);
    Serial.print(" cm");
    Serial.println();
    abrirTacho();
    digitalWrite(ledRojo, HIGH);  // Encendemos el LED rojo
    tone(piezoBuzzer, 3000, 50);
  }
}

boolean tachoLleno() {
  //Serial.println(digitalRead(ir));
  int irDetected = 0; // digitalRead(ir) 
  if (irDetected == HIGH) {
    Serial.println("IR no detectado...");
    nivelTachoLleno = 0;
    return false;
  } else {
    Serial.println("IR detectado...");
    nivelTachoLleno++;
  }

  if (nivelTachoLleno == 1) {
    milisAct = millis();
  }

  if (millis() - milisAct >= 5000) {
    return true;
  } else {
    return false;
  }
}

void alarma() {
  cerrarTacho();

  digitalWrite(ledVerde, LOW);
  digitalWrite(ledRojo, HIGH);

  // Mientras el pulsador haya sido presionado
  while (!pulsadorPresionado()) {
    Serial.println("Pulsador: FALSE");

    tone(piezoBuzzer, 440, 300);
    noTone(piezoBuzzer);
    tone(piezoBuzzer, 300, 440);
  }

  vaciarTachoLleno();
  Serial.println("Pulsador: OK");
}

void cerrarTacho() {
  servo.write(180);
  delay(600);
}

boolean pulsadorPresionado() {
  int valorPulsador = digitalRead(pulsador);

  if (valorPulsador == LOW) {
    Serial.print("pulsador: ");
    Serial.print(valorPulsador);
    Serial.println();
    return false;
  } else {
    return true;
  }
}

void vaciarTachoLleno() {
  digitalWrite(ledVerde, HIGH);
  digitalWrite(ledRojo, HIGH);

  abrirTacho();

  while (pulsadorPresionado()) {
    darkvioletRGB();
    Serial.println("Vaciando el tacho lleno...");
  }

  digitalWrite(ledRojo, LOW);
  apagarRGB();

  cerrarTacho();
}

void abrirTacho() {
  servo.write(90);
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

void darkvioletRGB() {
  digitalWrite(rgbRojo, 148);
  digitalWrite(rgbVerde, 0);
  digitalWrite(rgbAzul, 211);
}

void amarilloRGB() {
  digitalWrite(rgbRojo, 255);
  digitalWrite(rgbVerde, 255);
  digitalWrite(rgbAzul, 0);
}

void testSetup() {
  // pinMode(piezoBuzzer, OUTPUT); // Descomentar para hacer sonido infinito
  // pruebaTachoLleno();  // Descomentar para probar
}

void testLoop() {
  // Prueba pines LED
  /*digitalWrite(ledVerde, HIGH);
  delay(1000);
  digitalWrite(ledVerde, LOW);
  delay(1000);
  digitalWrite(ledAmarillo, HIGH);
  delay(1000);
  digitalWrite(ledAmarillo, LOW);
  delay(1000);
  digitalWrite(ledRojo, HIGH);
  delay(1000);
  digitalWrite(ledRojo, LOW);
  delay(1000);*/

  // Prueba pines RGB
  /*digitalWrite(rgbRojo, HIGH);
  delay(1000);
  digitalWrite(rgbRojo, LOW);
  delay(1000);
  digitalWrite(rgbVerde, HIGH);
  delay(1000);
  digitalWrite(rgbVerde, LOW);
  delay(1000);
  digitalWrite(rgbAzul, HIGH);
  delay(1000);
  digitalWrite(rgbAzul, LOW);
  delay(1000);*/

  // Prueba sonido piezobuzzer y pulsador
  //alarma();


  // Prueba tachito lleno
  /*if (tachoLleno()) {
    alarma();
  }*/
}
