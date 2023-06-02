#pragma once

//Configuração Display
#define SCREEN_WIDTH 128 // Largura do display OLED em pixels
#define SCREEN_HEIGHT 32 // Altura do display OLED em pixels
#define OLED_RESET -1 // Pin reset do display OLED (ou -1 se não tiver)

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//funções configuraçao
void setCursorMeio(Adafruit_SSD1306 display);

//encapsulamento
void start_display(Adafruit_SSD1306 display);
void display_acesso_liberado(Adafruit_SSD1306 display);
void display_acesso_negado(Adafruit_SSD1306 display);
void display_home(Adafruit_SSD1306 display);


