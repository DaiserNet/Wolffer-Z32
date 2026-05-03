/*
 * Wolffer-Z32 - Custom Flipper Zero
 * Fase 0: Hardware y Lectura de Sensores Digitales/Analogicos
 * 
 * Hardware target: ESP32-S3-N16R8
 * - 16MB Flash (N16)
 * - 8MB PSRAM OPI (R8)
 * - USB OTG nativo
 * Autores: D4N V0ID H0WL y DaiserNet
 */

#include <Arduino.h>
#include <DHT.h>

// ─────────────────────────────────────────────
// Constantes del Hardware
// ─────────────────────────────────────────────

// Pines definidos en el esquematico
#define DHT_PIN           4     // Pin para el sensor de temperatura y humedad
#define HCSR501_PIN       5     // Pin para el sensor de movimiento
#define MQ135_DO_PIN      6     // Pin para el sensor de calidad del aire (lectura digital)
#define MQ135_AO_PIN      10    // Pin para el sensor de calidad del aire (lectura analogica)
#define SERIAL_BAUD       115200

#define FW_VERSION        "0.1.0-fase0"
#define DEVICE_NAME       "Wolffer-Z32"

// Variables globales de lectura
DHT dht(DHT_PIN, DHT11);

void verificarHardware() {
    Serial.println("╔══════════════════════════════════╗");
    Serial.println("║     Wolffer-Z32 - Fase 0         ║");
    Serial.printf( "║     Firmware v%s                 ║\n", FW_VERSION);
    Serial.println("╚══════════════════════════════════╝\n");

    Serial.printf("[HW] Flash size:    %d MB\n", ESP.getFlashChipSize() / (1024*1024));
    Serial.printf("[HW] Heap libre:    %d bytes\n", ESP.getFreeHeap());
    Serial.printf("[HW] PSRAM heap libre: %d bytes\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    Serial.printf("[HW] Chip modelo:   %s\n", ESP.getChipModel());
    Serial.printf("[HW] Núcleos CPU:   %d\n", ESP.getChipCores());
    Serial.printf("[HW] Frecuencia:    %d MHz\n", ESP.getCpuFreqMHz());
    Serial.println();
}

void setup() {
    Serial.begin(SERIAL_BAUD);
    verificarHardware();

    pinMode(DHT_PIN, INPUT_PULLUP);
    dht.begin();
    
    pinMode(HCSR501_PIN, INPUT);

    delay(2000);
}

void loop() {
    delay(2000);

    float tempC = dht.readTemperature();
    float hum = dht.readHumidity();

    int mov = digitalRead(HCSR501_PIN);

    int airDO = digitalRead(MQ135_DO_PIN); // High o Low (1 o 0)
    int airAO = analogRead(MQ135_AO_PIN); // 0 - 4095 crudo

    float airV = (airAO / 4095) * 3.3; // Valor en voltaje de la lectura analogica del MQ-135

    // Impresion de Valores
    Serial.printf("Valor de la temperatura: %f °C\n", tempC);
    Serial.printf("Valor de la humedad: %f g/m3\n", hum);
    Serial.printf("Valor del movimiento: %d \n", mov);
    Serial.printf("Estado del sensor de aire: %d \n", airDO);
    Serial.printf("Valor de la lectura del aire en crudo: %d \n", airAO);
    Serial.printf("Valor de la lectura del aire en Voltaje: %.2f \n", airV);
}