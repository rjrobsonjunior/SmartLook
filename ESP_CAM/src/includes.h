
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ESPAsyncWebServer.h>
#include <AsyncWebSocket.h>
#include <esp_task_wdt.h>

/*--- Rede ---*/

const char* ssid = "Adryan e Enzo";
const char* password = "corvssan";
AsyncWebServer server(80);

//Servidor local - Node.js
const char* serverIP = "192.168.1.10";
const char* serverUrlANALISE = "http://192.168.1.10:8800/recognition";

//Envio Requisição HTTPS (SERVIDOR WEB EXTERNO)
String serverNameS = "fechaduraeletronica.000webhostapp.com";
String serverPathS= "/upload.php";
const int serverPortS = 443;

/*--- CAMERA ---*/

#include "esp_camera.h"
#include "SPIFFS.h"
#include "esp_timer.h"
#include "img_converters.h"
#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems
#include "driver/rtc_io.h"
#include <StringArray.h>
#include <FS.h>
#include "fb_gfx.h"

// OV2640 camera module pins (CAMERA_MODEL_AI_THINKER)
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22
#define FLASH_GPIO_NUM 4

//Caminho da foto
#define FILE_PHOTO "/imagem.jpg"

