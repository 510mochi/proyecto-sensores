#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ==========================================
// SECCIÓN: VARIABLES GENERALES Y DEFINICIONES
// ==========================================

// --- [SENSOR: PANTALLA OLED] ---
#define ANCHO 128
#define ALTO 64
#define OLED_RESET -1
Adafruit_SSD1306 display(ANCHO, ALTO, &Wire, OLED_RESET);

// --- [SENSOR: BME280 - HUMEDAD/TEMP] ---
Adafruit_BME280 bme;
#define SDA_PIN 21
#define SCL_PIN 22

// --- [SENSOR: ULTRASONIDO - DISTANCIA] ---
const int trigPin = 4;   // GPIO 4 (Trig)
const int echoPin = 16;  // GPIO 16 (Echo)

// --- [SENSOR: LDR - FOTORRESISTOR DE LUZ] ---
#define LDR A0 // Pin analógico (ej: GPIO 36)
int valorLuz = 0;
int porcentaje = 0;

// --- [SENSOR: PIR Y MÓDULO RELÉ - MOVIMIENTO] ---
const int pirPin = 12;   // GPIO 12 (Entrada del PIR)
const int relePin = 13;  // GPIO 13 (Salida al Relé / Foco)
int pirsensor = 0;


// ==========================================
// SECCIÓN: CONFIGURACIÓN INICIAL (SETUP)
// ==========================================
void setup() {
    // Inicializar puerto serial para depuración
    Serial.begin(115200);

    // Inicializar comunicación I2C para el BME280 y la OLED
    Wire.begin(SDA_PIN, SCL_PIN);

    // --- [CONFIGURACIÓN: PANTALLA OLED] ---
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("Error al iniciar OLED");
        while (true);
    }
    // Mostrar pantalla de bienvenida
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(20, 20);
    display.println("SISTEMA MULTISENSOR");
    display.setCursor(35, 35);
    display.println("VERSION FINAL");
    display.display();
    delay(2000);

    // --- [CONFIGURACIÓN: BME280] ---
    if (!bme.begin(0x76)) {
        Serial.println("Error: No se detecta el sensor BME280");
        while (1);
    }
    Serial.println("Sensor BME280 iniciado correctamente");
 
    // --- [CONFIGURACIÓN: ULTRASONIDO] ---
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);

    // --- [CONFIGURACIÓN: PIR Y RELÉ] ---
    pinMode(pirPin, INPUT);
    pinMode(relePin, OUTPUT);
    
    Serial.println("Sistema unificado listo.");
    Serial.println("------------------------------------");
}


// ==========================================
// SECCIÓN: BUCLE PRINCIPAL (LOOP)
// ==========================================
void loop() {
    
    // --- [LECTURA SENSOR: ULTRASONIDO] ---
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
 
    long duration = pulseIn(echoPin, HIGH);
    float distance = duration * 0.034 / 2; // Cálculo de distancia en cm
 
    // --- [LECTURA SENSOR: BME280] ---
    float humedad = bme.readHumidity();

    // --- [LECTURA SENSOR: LDR] ---
    valorLuz = analogRead(LDR);
    porcentaje = map(valorLuz, 0, 4095, 0, 100); // Conversión a porcentaje (ADC 12 bits)

    // --- [LECTURA SENSOR: PIR Y ACCIÓN DEL RELÉ] ---
    pirsensor = digitalRead(pirPin);

    if (pirsensor == HIGH) {
        digitalWrite(relePin, HIGH); // Encender foco si detecta movimiento
    } else {
        digitalWrite(relePin, LOW);  // Apagar foco si no hay movimiento
    }


    // ==========================================
    // SECCIÓN: SALIDA DE DATOS (MONITOR SERIE)
    // ==========================================
    Serial.print("Dist: "); Serial.print(distance); Serial.print("cm | ");
    Serial.print("Hum: "); Serial.print(humedad); Serial.print("% | ");
    Serial.print("Luz: "); Serial.print(porcentaje); Serial.print("% | ");
    Serial.print("PIR: "); Serial.println(pirsensor == HIGH ? "MOVIMIENTO!" : "OK");
 

    // ==========================================
    // SECCIÓN: DISEÑO DE LA PANTALLA OLED
    // ==========================================
    display.clearDisplay();

    // Estructura y Título
    display.setTextSize(1);
    display.setCursor(10, 0);
    display.println("MONITOR CENTRAL ESP32");
    display.drawLine(0, 9, 128, 9, SSD1306_WHITE);

    // Muestra datos del ULTRASONIDO
    display.setCursor(0, 13);
    display.print("Distancia: ");
    display.print(distance, 1);
    display.print(" cm");

    // Muestra datos del BME280
    display.setCursor(0, 23);
    display.print("Humedad:   ");
    display.print(humedad, 1);
    display.print(" %");

    // Muestra datos del PIR
    display.setCursor(0, 33);
    display.print("Movimiento: ");
    if (pirsensor == HIGH) {
        display.print("[ SI ]");
    } else {
        display.print("[ NO ]");
    }

    // Muestra datos del LDR (Numérico)
    display.setCursor(0, 43);
    display.print("Luz LDR:   ");
    display.print(porcentaje);
    display.print(" %");

    // Muestra datos del LDR (Barra gráfica inferior)
    display.drawRect(0, 55, 128, 9, SSD1306_WHITE);
    display.fillRect(1, 56, porcentaje * 126 / 100, 7, SSD1306_WHITE);

    // Enviar todo el diseño compilado a la pantalla física
    display.display();

    // Tiempo de espera entre lecturas
    delay(500);
}
