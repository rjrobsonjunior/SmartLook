#include <Keypad.h>
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Definição dos pinos
#define RELE_PIN 2
#define BUZZER_PIN 5
#define NOTA_BUZZER 528
#define RECPTOR_PIN 15
#define MAG_PIN 23
#define FECHADURA_PIN 35

/* ---------------- Display ---------------- */


//Configuração Display
#define SCREEN_WIDTH 128 // Largura do display OLED em pixels
#define SCREEN_HEIGHT 32 // Altura do display OLED em pixels
#define OLED_RESET -1 // Pin reset do display OLED (ou -1 se não tiver)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //Inicializa display


/* ---------------- Teclado Matricial ---------------- */


const byte ROWS = 4; // Número de linhas do teclado matricial
const byte COLS = 4; // Número de colunas do teclado matricial

char keys[ROWS][COLS] = { 
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {32, 33, 25, 26}; // Pinos do ESP32 conectados às linhas do teclado matricial
byte colPins[COLS] = {27, 14, 12, 13}; // Pinos do ESP32 conectados às colunas do teclado matricial

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS); //Inicializa teclado


/* ---------------- Configurações de Rede ---------------- */

// Wifi
const char* ssid = "Adryan e Enzo";
const char* password_wifi = "corvssan";

// Rede ESP32
IPAddress local_IP(192, 168, 0, 10); // Endereço IP do ESP32
IPAddress gateway(192, 168, 144, 139);   // Endereço do gateway
IPAddress subnet(255, 255, 255, 0);  // Máscara de sub-rede

// Rotas importantes
String url_tirarFoto = "http://192.168.1.11/capture";
const char* ip_espCAM = "192.168.1.11";
String url_analiseLogin = "http://192.168.0.10:8800/login";

String rotaFoto = "/capture";
String rotaFace = "/analisaFoto";

// Variáveis para armazenar o nome de usuário e a senha
String login = "";
String senha = "";
