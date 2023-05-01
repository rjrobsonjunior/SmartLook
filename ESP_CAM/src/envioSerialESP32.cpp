#include <HardwareSerial.h>

HardwareSerial SerialCam(1); // Use a porta serial 1 para se comunicar com a ESP32-CAM

void setup() {
  Serial.begin(115200); // Inicializa a comunicação serial com o monitor serial do Arduino
  SerialCam.begin(115200, SERIAL_8N1, 13, 12); // Inicializa a comunicação serial com a ESP32-CAM
}

void loop() {
  SerialCam.println("Hello ESP32-CAM!"); // Envia a string "Hello ESP32-CAM!" para a ESP32-CAM
  delay(1000); // Espera 1 segundo
}
