// ======================================================
// ROBOT SEGUIDOR DE LÍNEA - ARDUINO NANO
// 5 SENSORES IR + DRIVER L298N + 2 MOTORES N20
// ======================================================


// ----------------------
// SENSORES IR
// ----------------------

// Orden: izquierda -> derecha
const int SENSOR_IZQ  = 2;
const int SENSOR_IZQ2 = 3;
const int SENSOR_CENT = 4;
const int SENSOR_DER2 = 5;
const int SENSOR_DER  = 6;


// ----------------------
// DRIVER L298N
// ----------------------

// Motor izquierdo
const int IN1 = 7;
const int IN2 = 8;
const int ENA = 9;     // PWM

// Motor derecho
const int ENB = 10;    // PWM
const int IN3 = 11;
const int IN4 = 12;


// ======================================================
// CONFIGURACIÓN
// ======================================================

// La mayoría de los módulos IR entregan LOW cuando
// detectan la línea negra.
//
// Si tus se-nsores funcionan al revés, cambia LOW por HIGH.
const int LINEA_DETECTADA = LOW;


// Velocidad normal del robot.
// Valores posibles: 0 - 255
int velocidadBase = 150;


// Velocidad máxima
int velocidadMaxima = 255;


// Velocidad utilizada cuando pierde la línea
int velocidadBusqueda = 110;


// Factor de corrección.
// Si gira muy poco -> aumentarlo.
// Si hace zigzag demasiado -> disminuirlo.
float Kp = 45.0;


// Guarda hacia qué lado estaba la línea antes de perderla.
// -1 = izquierda
//  1 = derecha
int ultimaDireccion = 1;


// ======================================================
// CONFIGURACIÓN INICIAL
// ======================================================

void setup() {

  // Sensores
  pinMode(SENSOR_IZQ, INPUT);
  pinMode(SENSOR_IZQ2, INPUT);
  pinMode(SENSOR_CENT, INPUT);
  pinMode(SENSOR_DER2, INPUT);
  pinMode(SENSOR_DER, INPUT);


  // Pines del L298N
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);


  // Al comenzar, motores detenidos
  detenerMotores();

  delay(1000);
}


// ======================================================
// BUCLE PRINCIPAL
// ======================================================

void loop() {

  // Leemos los 5 sensores
  int s1 = digitalRead(SENSOR_IZQ);
  int s2 = digitalRead(SENSOR_IZQ2);
  int s3 = digitalRead(SENSOR_CENT);
  int s4 = digitalRead(SENSOR_DER2);
  int s5 = digitalRead(SENSOR_DER);


  // Convertimos cada sensor a:
  // 1 = detectó línea
  // 0 = no detectó línea

  bool izquierda  = (s1 == LINEA_DETECTADA);
  bool izquierda2 = (s2 == LINEA_DETECTADA);
  bool centro     = (s3 == LINEA_DETECTADA);
  bool derecha2   = (s4 == LINEA_DETECTADA);
  bool derecha    = (s5 == LINEA_DETECTADA);


  // ----------------------------------------------------
  // CALCULAR POSICIÓN DE LA LÍNEA
  // ----------------------------------------------------

  int suma = 0;
  int cantidad = 0;

  // Cada sensor tiene un peso dependiendo de su posición.

  if (izquierda) {
    suma += -4;
    cantidad++;
  }

  if (izquierda2) {
    suma += -2;
    cantidad++;
  }

  if (centro) {
    suma += 0;
    cantidad++;
  }

  if (derecha2) {
    suma += 2;
    cantidad++;
  }

  if (derecha) {
    suma += 4;
    cantidad++;
  }


  // ----------------------------------------------------
  // SI NO SE DETECTA NINGUNA LÍNEA
  // ----------------------------------------------------

  if (cantidad == 0) {

    // La línea se perdió.
    // Buscamos hacia el último lado conocido.

    if (ultimaDireccion < 0) {
      buscarIzquierda();
    }
    else {
      buscarDerecha();
    }

    return;
  }


  // ----------------------------------------------------
  // CALCULAR ERROR
  // ----------------------------------------------------

  float error = (float)suma / cantidad;


  // Guardamos la última dirección conocida

  if (error < -0.2) {
    ultimaDireccion = -1;
  }

  if (error > 0.2) {
    ultimaDireccion = 1;
  }


  // ----------------------------------------------------
  // CORRECCIÓN PROPORCIONAL
  // ----------------------------------------------------

  float correccion = error * Kp;


  // Si la línea está a la izquierda:
  // el motor izquierdo disminuye velocidad
  // y el derecho aumenta.

  int velocidadIzquierda = velocidadBase + correccion;
  int velocidadDerecha   = velocidadBase - correccion;


  // Limitar valores entre 0 y 255

  velocidadIzquierda = constrain(
    velocidadIzquierda,
    0,
    velocidadMaxima
  );

  velocidadDerecha = constrain(
    velocidadDerecha,
    0,
    velocidadMaxima
  );


  // ----------------------------------------------------
  // AVANZAR
  // ----------------------------------------------------

  avanzar(
    velocidadIzquierda,
    velocidadDerecha
  );
}


// ======================================================
// FUNCIONES DE MOTORES
// ======================================================


// ------------------------------------------------------
// AVANZAR
// ------------------------------------------------------

void avanzar(int velocidadIzquierda, int velocidadDerecha) {

  // Motor izquierdo hacia adelante
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  // Motor derecho hacia adelante
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);


  // Control de velocidad mediante PWM
  analogWrite(ENA, velocidadIzquierda);
  analogWrite(ENB, velocidadDerecha);
}


// ------------------------------------------------------
// DETENER MOTORES
// ------------------------------------------------------

void detenerMotores() {

  analogWrite(ENA, 0);
  analogWrite(ENB, 0);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}


// ------------------------------------------------------
// BUSCAR LÍNEA HACIA LA IZQUIERDA
// ------------------------------------------------------

void buscarIzquierda() {

  // Motor izquierdo hacia atrás
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  // Motor derecho hacia adelante
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);


  analogWrite(ENA, velocidadBusqueda);
  analogWrite(ENB, velocidadBusqueda);
}


// ------------------------------------------------------
// BUSCAR LÍNEA HACIA LA DERECHA
// ------------------------------------------------------

void buscarDerecha() {

  // Motor izquierdo hacia adelante
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  // Motor derecho hacia atrás
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);


  analogWrite(ENA, velocidadBusqueda);
  analogWrite(ENB, velocidadBusqueda);
}
