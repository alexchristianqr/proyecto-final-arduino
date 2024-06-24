#include <Servo.h>
#include <SoftwareSerial.h>
#include <TCS3200.h>

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

const int s0 = 7;   // Pin S0
const int s1 = 0;   // Pin S1
const int s2 = 0;   // Pin S2
const int s3 = 0;   // Pin S3
const int out = 7;  // Pin OUT del sensor

TCS3200 tcs3200(s0, s1, s2, s3, out);

// Definimos una estructura para los colores
struct Color {
  int r;
  int g;
  int b;
};

// Lista de colores que se asemejan a papel bond
Color papelBond[] = {
  { 255, 255, 255 },
  { 250, 250, 250 },
  { 245, 245, 245 },
  { 240, 240, 240 },
  { 235, 235, 235 },
  { 230, 230, 230 }
};

// Lista de colores que se asemejan a cartón
Color carton[] = {
  { 200, 170, 120 },
  { 185, 155, 105 },
  { 170, 140, 90 },
  { 160, 130, 80 },
  { 150, 120, 70 }
};

const int numColorsBond = sizeof(papelBond) / sizeof(papelBond[0]);
const int numColorsCarton = sizeof(carton) / sizeof(carton[0]);

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

  // Initialize the TCS3200 sensor and set frequency scaling to 20%
  tcs3200.begin();
  tcs3200.frequency_scaling(TCS3200_OFREQ_20P);

  // Initialize Serial communication for output
  Serial.begin(115200);
  Serial.println("TCS3200 Full Example");
  Serial.println("-----------------------------------");

  // Calibrate the sensor for accurate color readings
  Serial.println("Calibrating...");

  // Calibrate the sensor for light surface (white calibration)
  Serial.println("Please face the sensor to white surface.");
  delay(1000);
  tcs3200.calibrate_light();

  // Calibrate the sensor for dark surface (black calibration)
  Serial.println("Please face the sensor to dark surface.");
  delay(1000);
  tcs3200.calibrate_dark();

  // Complete calibration and enable calibrated readings
  Serial.println("Done calibrating!");
  tcs3200.calibrate();

  // Add a delay to allow time for the serial output to be read
  delay(1000);
}

void loop() {
  //startSmartTrash();
}

bool esPapelBond(int r, int g, int b) {
  for (int i = 0; i < numColorsBond; i++) {
    if (abs(papelBond[i].r - r) <= 10 && abs(papelBond[i].g - g) <= 10 && abs(papelBond[i].b - b) <= 10) {
      return true;
    }
  }
  return false;
}

bool esCarton(int r, int g, int b) {
  for (int i = 0; i < numColorsCarton; i++) {
    if (abs(carton[i].r - r) <= 10 && abs(carton[i].g - g) <= 10 && abs(carton[i].b - b) <= 10) {
      return true;
    }
  }
  return false;
}

// Iniciar tacho de basura inteligente
void startSmartTrash() {
  tcs3200.loop();

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

  RGBColor rgb_color = tcs3200.read_rgb_color();
  int r = rgb_color.red;
  int g = rgb_color.green;
  int b = rgb_color.blue;

  Serial.print("R: ");
  Serial.print(r);
  Serial.print("G: ");
  Serial.print(g);
  Serial.print("B: ");
  Serial.println(b);

  if (esPapelBond(r, g, b)) {
    Serial.println("Material: PAPEL");
    abrirTacho();
  } else if (esCarton(r, g, b)) {
    Serial.println("Material: CARTON");
    abrirTacho();
  } else {
    Serial.println("Material desconocido");
  }

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