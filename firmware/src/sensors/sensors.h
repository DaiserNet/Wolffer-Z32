#pragma once
#include <Arduino.h>

// ─────────────────────────────────────────────
// Estructura que representa una lectura de sensores
// ─────────────────────────────────────────────
struct SensorData {
    // Temperatura (ej: DHT22, DS18B20)
    float temperatura_c;
    bool  temperatura_valida;
    
    // Movimiento (ej: PIR HC-SR501)
    bool  movimiento_detectado;
    unsigned long ultimo_movimiento_ms;
    
    // Calidad del aire (ej: MQ-135, SGP30)
    int   aire_ppm;
    bool  aire_valido;
    
    // Timestamp de la lectura
    unsigned long timestamp_ms;
};

class GestorSensores {
public:
    void inicializar(int pin_temp, int pin_pir, int pin_aire);
    SensorData leer();
    void imprimirLectura(const SensorData& datos);
};