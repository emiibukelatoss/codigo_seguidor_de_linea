# codigo_seguidor_de_linea
# Robot Seguidor de Línea

## Descripción del proyecto

Este proyecto consiste en un **robot velocista seguidor de línea**, diseñado con un **Arduino Nano**. El objetivo del robot es seguir automáticamente una línea negra marcada sobre una superficie clara.

Para hacerlo, el robot utiliza **5 sensores infrarrojos (IR)** que detectan la posición de la línea. Dependiendo de qué sensores la detecten, el Arduino calcula hacia qué lado debe corregir su movimiento.

El robot utiliza dos motores N20 controlados mediante un **driver L298N**, lo que permite controlar tanto la dirección como la velocidad de cada motor.

Además, cuando el robot pierde la línea, utiliza la información de la última dirección en la que la detectó para intentar encontrarla nuevamente.

---

## Objetivos

* Construir un robot capaz de seguir una línea de manera autónoma.
* Utilizar sensores IR para detectar la posición de la línea.
* Controlar dos motores mediante un driver L298N.
* Regular la velocidad de los motores mediante PWM.
* Realizar correcciones de dirección automáticamente.
* Lograr que el robot pueda recuperar la línea cuando la pierde.

---

## Componentes utilizados

* Arduino Nano
* 5 sensores infrarrojos (IR)
* Driver de motores L298N
* 2 motores N20
* 2 ruedas
* Rueda loca o rueda de apoyo
* Batería y portabatería
* Interruptor
* Regulador/elevador de tensión
* Protoboard
* Capacitores
* Cables de conexión
* Chasis del robot

---

## Conexiones

### Sensores IR

Los sensores están colocados de **izquierda a derecha**:

| Sensor             | Pin Arduino |
| ------------------ | ----------: |
| Sensor izquierdo   |          D2 |
| Sensor izquierdo 2 |          D3 |
| Sensor central     |          D4 |
| Sensor derecho 2   |          D5 |
| Sensor derecho     |          D6 |

### Driver L298N

#### Motor izquierdo

| Función | Pin Arduino |
| ------- | ----------: |
| IN1     |          D7 |
| IN2     |          D8 |
| ENA     |    D9 (PWM) |

#### Motor derecho

| Función | Pin Arduino |
| ------- | ----------: |
| ENB     |   D10 (PWM) |
| IN3     |         D11 |
| IN4     |         D12 |

---

## Cómo funciona

El funcionamiento del robot se puede dividir en varios pasos.

### 1. Lectura de los sensores

Los 5 sensores IR leen constantemente la superficie.

En este proyecto:

* `1` significa que el sensor detectó la línea.
* `0` significa que no detectó la línea.

La constante utilizada para indicar cuándo se detecta la línea es:

`LINEA_DETECTADA = LOW`

Esto significa que los sensores están configurados para detectar la línea negra cuando entregan un valor `LOW`.

Si los sensores funcionan al revés, se puede cambiar `LOW` por `HIGH`.

---

### 2. Determinar dónde está la línea

Cada sensor tiene un **peso diferente** dependiendo de su posición:

| Sensor      | Peso |
| ----------- | ---: |
| Izquierdo   |   -4 |
| Izquierdo 2 |   -2 |
| Centro      |    0 |
| Derecho 2   |    2 |
| Derecho     |    4 |

Los valores negativos representan el lado izquierdo y los positivos el lado derecho.

Por ejemplo, si se activa el sensor central, la posición calculada es `0`, por lo que el robot debería avanzar prácticamente recto.

Si se activa un sensor del lado izquierdo, el robot sabe que necesita corregir hacia ese lado.

---

### 3. Cálculo del error

El programa suma los pesos de los sensores que detectaron la línea y luego divide por la cantidad de sensores activos.

De esta forma obtiene un valor llamado **error**:

```text
error = suma / cantidad
```

Este valor indica aproximadamente hacia qué lado se encuentra la línea con respecto al centro del robot.

* Error negativo: línea hacia la izquierda.
* Error cercano a 0: línea centrada.
* Error positivo: línea hacia la derecha.

---

## Corrección proporcional

El robot utiliza una **corrección proporcional**, conocida como `Kp`.

En el código:

```text
Kp = 45.0
```

La corrección se calcula multiplicando el error por `Kp`:

```text
correccion = error * Kp
```

Después se modifica la velocidad de cada motor.

### Si la línea está hacia la izquierda

* El motor izquierdo disminuye su velocidad.
* El motor derecho aumenta su velocidad.

### Si la línea está hacia la derecha

* El motor izquierdo aumenta su velocidad.
* El motor derecho disminuye su velocidad.

Esto permite que el robot gire gradualmente en lugar de utilizar solamente movimientos de izquierda y derecha.

---

## Control de velocidad

La velocidad normal del robot está definida por:

```text
velocidadBase = 150
```

Los valores de velocidad pueden ir desde `0` hasta `255`.

También se establece una velocidad máxima:

```text
velocidadMaxima = 255
```

El programa utiliza `constrain()` para evitar que las velocidades superen esos límites.

La velocidad de búsqueda cuando el robot pierde la línea es:

```text
velocidadBusqueda = 110
```

---

## Qué pasa cuando pierde la línea

Si ninguno de los 5 sensores detecta la línea, el programa considera que el robot la perdió.

En ese momento utiliza la variable:

```text
ultimaDireccion
```

Esta variable guarda hacia qué lado estaba la línea antes de perderla.

* `-1`: izquierda
* `1`: derecha

Por ejemplo, si antes de perder la línea esta se encontraba hacia la izquierda, el robot comienza a girar hacia la izquierda para intentar encontrarla nuevamente.

Esto se realiza mediante las funciones:

* `buscarIzquierda()`
* `buscarDerecha()`

En estas funciones un motor gira hacia adelante y el otro hacia atrás, haciendo que el robot gire sobre su propio eje.

---

## Funciones principales

### `setup()`

Se ejecuta una sola vez al iniciar el Arduino.

Configura los pines de los sensores y del L298N y deja los motores detenidos durante un segundo antes de comenzar.

### `loop()`

Es el funcionamiento principal del robot.

Se repite constantemente y realiza este proceso:

1. Lee los 5 sensores.
2. Determina cuáles detectaron la línea.
3. Calcula la posición de la línea.
4. Calcula el error.
5. Realiza la corrección.
6. Ajusta la velocidad de cada motor.
7. Hace avanzar al robot.
8. Si no encuentra la línea, intenta buscarla.

### `avanzar()`

Hace que ambos motores avancen y establece la velocidad de cada uno mediante PWM.

### `detenerMotores()`

Detiene completamente los dos motores.

### `buscarIzquierda()`

Hace girar al robot hacia la izquierda cuando perdió la línea y la última dirección conocida era la izquierda.

### `buscarDerecha()`

Hace girar al robot hacia la derecha cuando perdió la línea y la última dirección conocida era la derecha.

---

## Lógica general

El funcionamiento puede resumirse de la siguiente manera:

```text
INICIO
  |
  v
Leer los 5 sensores
  |
  v
¿Algún sensor detecta la línea?
  |
  +------------------+
  |                  |
  NO                 SÍ
  |                  |
  v                  v
Buscar hacia      Calcular posición
la última         de la línea
dirección            |
  |                  v
  |              Calcular error
  |                  |
  |                  v
  |          Aplicar corrección
  |                  |
  |                  v
  |          Ajustar motores
  |                  |
  +---------> Avanzar
                  |
                  v
               REPETIR
```

---

## Estructura del proyecto

```text
Robot-Seguidor-de-Linea/
|
├── RobotSeguidor.ino
└── README.md
```

El archivo `.ino` contiene todo el código utilizado para controlar el robot y el `README.md` contiene la explicación del proyecto.

---

## Parámetros configurables

Algunos valores pueden modificarse para adaptar el robot a diferentes pistas:

| Variable            | Función                                |
| ------------------- | -------------------------------------- |
| `velocidadBase`     | Velocidad normal                       |
| `velocidadMaxima`   | Velocidad máxima                       |
| `velocidadBusqueda` | Velocidad al buscar la línea           |
| `Kp`                | Intensidad de la corrección            |
| `LINEA_DETECTADA`   | Valor utilizado para detectar la línea |

Por ejemplo, el valor de `Kp` puede modificarse dependiendo del comportamiento del robot:

* Si corrige muy poco, aumentar `Kp`.
* Si realiza demasiado zigzag, disminuir `Kp`.

---

## Proyecto escolar

Este proyecto fue realizado como parte de un trabajo de **robótica y programación**, aplicando conocimientos de:

* Arduino
* Electrónica
* Sensores
* Control de motores
* PWM
* Programación en C/C++
* Lógica de control
* Robótica autónoma

---

## Resultado esperado

El resultado esperado es un robot que pueda recorrer una pista siguiendo una línea negra de forma autónoma, realizando correcciones según la posición de la línea y recuperándola cuando momentáneamente deje de detectarla.

link de wokwi: https://wokwi.com/projects/473330498458242049
link de documentación: https://docs.google.com/document/d/1bF2HbH2YWNZNG3CmXG7nk6wNBK1XAkp3UjP9ga0ktmE/edit?usp=sharing
