#include <Arduino.h>
#include <HardwareSerial.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>

#include "esp_camera.h"
#include "esp_http_client.h"
#include "SPIFFS.h"

const char* serverUrl = "http://endereco.do.seu.servidor";
const char* ssid = "seuSSID";
const char* password = "suaSenha";

WebServer server(80);

size_t fbSize = 0;

//Comunicação Serial
HardwareSerial SerialESP32(1);
#define PORT_RX 3 
#define PORT_TX 1

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

void initCamera()
{

  // OV2640 camera module
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound())
  {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  }
  else
  {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  // Inicializacao da camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK)
  {
    Serial.printf("Camera init failed with error 0x%x", err);
    ESP.restart();
  }
}

// Função para salvar a imagem no SPIFFS
void salvarImagem(uint8_t* buffer) 
{
  // Inicializa o SPIFFS
  if(!SPIFFS.begin(true)) {
    Serial.println("Falha ao montar o sistema de arquivos SPIFFS");
    return;
  }

  // Abre o arquivo no modo de escrita
  File file = SPIFFS.open("/imagem.jpg", FILE_WRITE);

  // Verifica se o arquivo foi aberto com sucesso
  if(!file) {
    Serial.println("Falha ao criar o arquivo");
    return;
  }

  // Escreve o conteúdo do buffer no arquivo
  if(file.write(buffer, fbSize) != fbSize){
    Serial.println("Erro ao gravar a imagem");
  }
  else{
    Serial.println("Imagem salva com sucesso!");
  }

  // Fecha o arquivo
  file.close();


}

//uint8_t é um tipo de inteiro sem o sinal negativo
//Essa função tira a foto após 5s
uint8_t* fotoJPEG()
{

  for(int i = 5; i >= 1; i--)
  {
    Serial.print("A foto será tirada em ");
    Serial.print(i);
    Serial.println(" segundos");
    delay(1000);
  }

  // Captura a imagem em JPEG
  camera_fb_t* fb = esp_camera_fb_get();
  
  // Verifica se a imagem foi capturada com sucesso
  if(!fb) {
    Serial.println("Falha na captura da imagem");
    return;
  }
  else
  {
    Serial.println("Imagem capturada!");
  }
  
  // Converte a imagem para formato JPEG
  fbSize = 0;
  uint8_t *jpgBuffer = NULL; //IMAGEM EM JPEG

  bool jpegSuc = frame2jpg(fb, 80, &jpgBuffer, &fbSize);

  esp_camera_fb_return(fb);
  
  // Verifica se a imagem foi convertida com sucesso
  if(!jpegSuc) {
    Serial.println("Falha na conversão para JPEG");
    return NULL;
  }
  else
  {
    Serial.println("Imagem convertida com sucesso para JPEG!");

    //Salva a foto no esp32cam
    salvarImagem(jpgBuffer);

    //Retorna buffer da imagem
    return jpgBuffer;
  }
}

//Envia a foto para a aplicação node.js
void enviarFotoJPEG(uint8_t* foto)
{

  // Abre o arquivo de imagem salvo no SPIFFS
  File file = SPIFFS.open("/imagem.jpg", FILE_READ);

  if (!file) {
    Serial.println("Falha ao abrir o arquivo de imagem");
    return;
  }

  // Envia a imagem para o servidor
  HTTPClient http;
  
  Serial.print("Enviando a imagem para o servidor: ");
  Serial.println(serverUrl);
  
  http.begin(serverUrl);

  // Anexa o arquivo de imagem como um anexo à solicitação HTTP POST
  http.addHeader("Content-Disposition", "attachment; filename=imagem.jpg");
  http.addHeader("Content-Type", "image/jpeg");
  http.addHeader("Content-Length", String(file.size()));

  
  int httpResponseCode = http.POST(foto, fbSize);
  
  if(httpResponseCode > 0) {
    Serial.print("Resposta do servidor: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.println("Falha ao enviar a imagem");
  }
  
  // Libera a memória alocada para o buffer da imagem
  if(foto != NULL) {
    free(foto);
  }

  //Fecha o arquivo
  file.close();

  http.end();

}

//Feito para conferir a ultima foto tirada (Ele abre a foto salva no SPIFFS do ESP32CAM)
void servidorWeb()
{
  // Inicialize o SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("Falha ao montar o sistema de arquivos SPIFFS");
    return;
  }

  // Defina a rota para a imagem
  server.on("/", []() {
    
    // Abra o arquivo da imagem
    File file = SPIFFS.open("/imagem.jpg", "r");
    if (!file) 
    {
      Serial.println("Falha ao abrir o arquivo da imagem");
      server.send(404, "text/plain", "Arquivo nao encontrado");
      return;
    }

    // Defina o tipo de conteúdo como imagem/jpeg
    server.sendHeader("Content-Type", "image/jpeg");

    // Envie o tamanho do arquivo como o cabeçalho Content-Length
    server.sendHeader("Content-Length", String(file.size()));

    // Envie a imagem para o cliente
    server.streamFile(file, "image/jpeg");

    // Feche o arquivo
    file.close();
  });

  server.begin();
  Serial.println("Servidor iniciado");


}


void setup() {

  Serial.begin(9600);

  // Inicializa a comunicação serial com o ESP32
  SerialESP32.begin(9600, SERIAL_8N1, PORT_RX, PORT_TX); 

  WiFi.begin(ssid, password);
  Serial.print("Conectando à rede Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conectado à rede Wi-Fi");
  Serial.print("Endereço de IP: ");
  Serial.println(WiFi.localIP());


  initCamera();
  servidorWeb();

}

void loop() {

  server.handleClient();

  String mensagem = "";

  // Verifica se há dados disponíveis na porta serial
  if (SerialESP32.available()) 
  {
    mensagem = SerialESP32.readStringUntil('\n'); // Lê a string enviada pelo ESP32
    Serial.println("Mensagem recebida do ESP32: " + mensagem); // Exibe a mensagem no monitor serial
  }

  //Se o ESP32 mandar o comando a foto é tirada
  if(mensagem == "foto")
  {

    //A foto é tirada. É armazenada na memoria do ESP e retornada num buffer
    uint8_t* foto = fotoJPEG();

    if(foto)
    {
      //Realiza o envio da foto para o Node.js para
      enviarFotoJPEG(foto);
    }
    else{
      Serial.println("Nao há fotos!");
    }
  }
  
  delay(500);

}