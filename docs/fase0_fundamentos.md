# Wolffer-Z32 — Documentación Técnica
> **FenrirNet** | Ingeniería en Sistemas Computacionales | Universidad Autonóma de Aguascalientes
> Materias: Sistemas Embebidos & Ciberseguridad

---

## Información

| Campo                | Valor                             |
|----------------------|-----------------------------------|
| **Fase**             | Fase 0 — Fundamentos              |
| **Versión firmware** | 0.1.0-fase0                       |
| **Fecha**            | 02/05/2026                        |
| **Autores**          | D4N V0ID H0WL / DaiserNet         |
| **Hardware**         | ESP32-S3-N16R8 + Orange Pi Zero 3 |

---

## 1. Configuración del Entorno de Desarrollo

### 1.1 Herramientas utilizadas

- **PlatformIO**: Facilidad para combinar Arduino Framework y C++ de una manera más sencilla
- **KiCad 10**: Limpieza y entendimiento de los esquematicos 
- **VS Code**: Editor de código abierto que tiene la extensión de PlatformIO 

### 1.2 Configuración de platformio.ini

```ini
[env:esp32s3-n16r8] ---> Placa ESP32-S3
platform = espressif32
board = 4d_systems_esp32s3_gen4_r8n16 ---> Identificador de la versión de mi placa y sus especificaciones
framework = arduino ---> Para usar la estructura clásica de setup() y loop()

monitor_speed = 115200 ---> Baudios a los que va a ir la placa
monitor_filters = esp32_exception_decoder
monitor_port = COM7 ---> El ESP32-S3 tiene dos puertos, y solamente se conectó uno, el COM7 es el identificador que el equipo le dió
upload_port = COM7 -----^

build_flags = 
    -DARDUINO_USB_CDC_ON_BOOT=0
    -DBOARD_HAS_PSRAM ---> Decirle al compilador que la placa tiene PSRAM

lib_deps =
    adafruit/DHT sensor library ---> Usar las funciones del DHT11 existentes
    adafruit/Adafruit Unified Sensor ---> Usar funciones de lectura de sensores existentes
```

**¿Por qué `4d_systems_esp32s3_gen4_r8n16` y no `esp32-s3-devkitc-1`?**
La placa empleada para el proyecto (ESP32-S3-N16R8) esta descrita de esa manera en \framework-arduinoespressif32\variants\
~La ruta cambia según el sistema operativo (y de si modificaste la ruta de instalación al agregar la Extensión a VS Code):
Windows: C:\Users\%tu_usuario%\.platformio\packages\framework-arduinoespressif32\variants\
Linux: ~/.platformio\packages\framework-arduinoespressif32\variants\

**Banderas de compilación**
Las banderas de compilación son la manera que tenemos para definir la manera en que queremos se haga el proceso de compilación, ensamblado y enlace a los archivos fuente en C/C++ 
La que se usó en fase 0 del proyecto tiene el siguiente formato:
-D name ---> define el 'name' como una macro

**¿Qué hace `-DARDUINO_USB_CDC_ON_BOOT=0`?**
La bandera controla si la placa pueda comunicarse con nuestro sistema via USB como si fuera un puerto serial.
En nuestra configuración se encuentra en 0, pues se conectó la ESP32-S3 mediante el puerto COM al equipo, provocando una comunicación nativa COM, también si se revisa upload_port y monitor_port todo se realiza mediante este único puerto; aquí solamente buscamos que siempre se comunique como COM, no como USB (especialmente útil para evitar errores en modo SERIAL MONITOR para debug).

**¿Qué hace `-DBOARD_HAS_PSRAM`?**
Le indicamos que tome en cuenta que nuestra placa tiene PSRAM; este tipo de memoria permite a la placa poder manejar una gran densidad de datos, lo cual sucede en las auditorias. La PSRAM se traduce como RAM pseudoestática.
Esta bandera de compilación

---

## 2. Hardware — ESP32-S3-N16R8

### 2.1 Especificaciones del chip
| Componente | Especificación       | Explicación |
|------------|----------------------|-----------------------------------------------------------------------------------|
| Flash      | 16MB                 | Memoria RAM optimizada para la gestión de recursos                                |
| PSRAM      | 8MB OPI              | Amplia la memoria RAM y maneja datos a gran velocidad                             |
| CPU        | Xtensa LX7 dual-core | La placa cuenta con 2 núcleos de procesamiento para efectuar cálculos             |
| Frecuencia | 240MHz               | Velocidad (Ciclos de reloj) en la que ejecuta los cálculos                        |
| USB        | OTG nativo           | El OTG significa que la placa se puede conectar y comunicar con otros dispositivos|

### 2.2 PSRAM — Preguntas clave

**¿Qué es PSRAM OPI y por qué el chip muestra 7MB en lugar de 8MB?**
Se asigna 1MB internamente para el sistema (esto lo hace automáticamente al compilar y cargar el código a la placa)

**¿Por qué `ESP.getFreeHeap()` y `heap_caps_get_free_size(MALLOC_CAP_SPIRAM)` dan valores diferentes?**
ESP.getFreeHeap() retorna la cantidad de RAM interna de la placa; esta se usa para variables normales
heap_caps_get_free_size() retorna la cantidad total de PSRAM; esta se usa para buffers grandes, señales RF y procesamiento de Imagenes

La placa tiene multiples RAMs:
- DRAM: contiene los datos que no son estáticos en la ejecución del código fuente
- IRAM: contiene las instrucciones que no se usan en la ejecución del código fuente

Es importante conocer las que nuestra placa contiene, ya que, al ser un sistema "pequeño" tenemos que optimizar y eficientar nuestros códigos al máximo para evitar desbordamientos de memoria.

**¿Qué es el sistema CAPS de ESP-IDF?**
Son las capacidades de memoria, en ESP-IDF se usa para pedir memoria con características específicas, por ejemplo:
1) Que sea RAM interna
2) Que sea ejectuable
En pocas palabras, caps es la etiqueta que se usa para solicitar memoria

### 2.3 Puertos de comunicación

| Puerto físico | Etiqueta placa | Windows | Chip | Uso |
|---|---|---|---|---|
| UART | COM | COM7 | CH343 | Upload + Monitor |
| USB OTG | USB | COM8 | ESP32-S3 nativo | HID, CDC, OTG |

**¿Por qué se necesitan dos cables durante el desarrollo?**
El ESP32-S3 tiene dos puertos, el COM y el USB, se necesitan ambos, porque facilitan el debugeo y mejoran la velocidad de desarrollo.
monitor_port = COM7 
upload_port = COM8
Por ejemplo, con lo anterior, hacemos que la carga del código a la placa sea más rápido, mientras que se use el COM para la comunicación serie; siendo lo más eficiente.

### 2.4 Pines restringidos — No usar

| Tipo            | Pines                                              | Razón                   |
|-----------------|----------------------------------------------------|-------------------------|
| Flash/PSRAM SPI | GPIO28, 30, 31, 32, 33, 34, 35, 38, 39, 40, 41, 42 | Uso interno del chip    |
| Strapping       | GPIO0, GPIO3, GPIO45, GPIO46                       | Afectan el modo de boot |

---

## 3. Hardware — Sensores

### 3.1 Esquemático V1

> Archivo: `hardware/schematic_v1/schematic_v1.kicad_pro`

**Componentes del esquemático:**
Placa ESP32-S3
Placa Orange Pi Zero 3
DHT11
HC-SR501 (PIR)
MQ-135 (AO y DO)

### 3.2 Tabla de conexiones final

| Sensor    | Pin ESP32-S3 | Tipo señal         | Voltaje operación | Voltaje señal | Resistencia  |
|-----------|--------------|--------------------|-------------------|---------------|--------------|
| DHT11     | GPIO5        | Digital 1-Wire     | 3.3V              | 3.3V          | 10KΩ pull-up |
| HC-SR501  | GPIO7        | Digital salida     | 5V                | 3.3V          | Ninguna      |
| MQ-135 DO | GPIO6        | Digital            | 5V                | 3.3V          | En módulo    |
| MQ-135 AO | GPIO10       | Analógico ADC1_CH4 | 5V                | v < 3.3V      | En módulo    |

### 3.3 Alimentación

**¿Por qué HC-SR501 y MQ-135 requieren 5V?**
Así lo dicta su Datasheet. Para cualquier prueba que se desee aplicar, las datasheet mandan y es mejor apegarse a esos valores antes que hacer experimentos.

**Solución implementada:**
El ESP32-S3 no tiene una salida de 5 Volts directamente para alimentar a los sensores; por lo tanto encontré dos soluciones:
1) Un divisor de voltaje con una pila de 9 Volts, con R1 = 330 Ohms y R2 = 430 Ohms, los valores pueden variar según lo que tengamos a la manos, la idea es que de un Voltaje entre 4.9 Volts y 5.1 Volts
2) En mi caso, tengo un ESP32 Devkit v1, el cual cuenta con una salida de 5 Volts; para efectos de procesamiento es muy inferior no obstante lo que entrega sirve bastante.

### 3.4 DHT11

- **Librería:** Adafruit DHT sensor library
- **Pull-up:** 10KΩ entre VCC y DATA
- **Tiempo de estabilización:** 2 segundos mínimo

### 3.5 HC-SR501

- **Alimentación:** 5V obligatorio (BISS0001 no opera a 3.3V)
- **Jumper:** Posición H (Repeatable trigger)
- **Tiempo de inicialización:** 60 segundos
- **Potenciómetros:** Sensibilidad máxima, tiempo mínimo

### 3.6 MQ-135

- **Alimentación:** 5V (calentador interno requiere 5V)
- **Pin DO:** Salida digital — compara con umbral del potenciómetro
- **Pin AO:** Salida analógica — valor crudo ADC 0-4095
- **Tiempo de precalentamiento:** 3 minutos mínimo

**¿Cómo convertir el valor crudo AO a voltaje?**
La lectura analogica da un valor entre 0 y 4095; los anteriores no son al azar, sino que los pines con ADC del ESP32-S3 son a 12 bits.
Los valores crudos no nos sirven de nada, pero como buenos Ingenieros los podemos interpretar en voltaje, por lo tanto, para convertirlos tenemos que aplicar la siguiente fórmula:
Voltaje = (lectura_cruda / 4095) * 3.3 Volts
La lectura_cruda es el valor que lee el ESP32-S3 mediante analogRead(), el 4095 es el valor máximo y los 3.3 Volts es el voltaje máximo del ESP32-S3 (NUNCA HAY QUE PONERLE MÁS)

---

## 4. Firmware — Fase 0/1

### 4.1 Estructura del proyecto

```
Wolffer-Z32/
├── firmware/
│   ├── platformio.ini
│   └── src/
│       └── main.cpp
├── hardware/
│   └── schematic_v1/
├── backend/
└── docs/
```

### 4.2 Problemas encontrados y soluciones

| Problema | Causa | Solución |
|---|---|---|
| PSRAM no detectada | Board genérico sin config OPI | Cambiar a `4d_systems_esp32s3_gen4_r8n16` |
| Firmware no arrancaba | Flash sucia con partition scheme anterior | `esptool erase_flash` |
| Serial sin salida | CDC on boot activo, monitor en puerto incorrecto | `-DARDUINO_USB_CDC_ON_BOOT=0` |
| DHT11 devuelve `nan` | Interferencia del USB CDC + pin flotante | Desconectar COM8, agregar `pinMode INPUT_PULLUP` |
| HC-SR501 siempre 0 | GPIO4 tiene función Touch activa + problema físico en sensor | Mover a GPIO7 + troubleshooting físico |
| Sin 5V disponibles | Pin 5V del ESP32-S3 no entrega corriente hacia afuera | Usar VIN del ESP32 DevKit V1 como fuente |

---

## 5. KiCad

**¿Qué es un PWR_FLAG y por qué KiCad lo requiere?**
Un PWR_FLAG es la manera de indicarle al programa de donde viene la alimentación; es necesario pues de lo contrario KiCad lo tomará como que están al aire, un sensor al aire que necesita 5 Volts, jamás va a funcionar.

**¿Cómo identificar una conexión válida en KiCad?**
Dirigete a la barra de tareas y sigue los siguientes pasos:
1) Inspeccionar
2) Control de las reglas eléctricas (ERC)
3) Seleccionar el check de Todo (en Mostrar)
4) Ejecutar ERC
Aquí nos dirá las conexiones inválidas y cual es su error; si no aparecen aquí, son conexiones válidas

**¿Qué son las Net Labels y cuándo usarlas?**
Son las etiquetas que puedes usar en KiCad, siempre hay que usarlas cuando son muchas conexiones; ayudan para mantener limpio y legible el esquemático

---

## 6. Referencias

| Recurso              | URL                                    | Para qué se usó               |
|----------------------|----------------------------------------|-------------------------------|
| ESP32-S3 Datasheet   | espressif.com                          | Restricciones de pines, PSRAM |
| arduino-esp32 repo   | github.com/espressif/arduino-esp32     | Configuración PSRAM, UART     |
| PlatformIO docs      | docs.platformio.org                    | platformio.ini, boards        |
| Adafruit DHT library | github.com/adafruit/DHT-sensor-library | Lectura DHT11                 |
| KiCad docs           | docs.kicad.org                         | Esquemático, ERC, Net Labels  |

---

## 7. Advertencias
**Esta es una herramienta de auditoria en Hardware Embebido para detectar fallas en la seguridad para aplicaciones IoT, equipos y asociados; nunca se debe usar para atacar equipos al que no tienes explicito permiso o no sean de tu propiedad**
**No nos hacemos responsables de usos indebidos**

**Documentación en Español para incentivar a la comunidad Hispana a sacar más proyectos al alcance de los hablantes de nuestra lengua**

*Wolffer-Z32 — FenrirNet © 2026*