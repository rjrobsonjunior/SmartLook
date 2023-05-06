#include <Arduino.h>
#include <HardwareSerial.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ESPAsyncWebServer.h>
#include <AsyncWebSocket.h>

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

//#include "esp_http_client.h"
//#include "esp_http_server.h"

const char* serverUrl = "http://192.168.1.5:3000/uploadImagem";
const char* ssid = "Adryan e Enzo";
const char* password = "corvssan";

AsyncWebServer server(80);
String messages = "";
String serverESP = "";


//Teste envio da imagem em partes
WiFiClient client;
WiFiClientSecure cliente;

String serverNameS = "fechaduraeletronica.000webhostapp.com";
String serverPathS= "/upload.php";
const int serverPortS = 443;

String serverName = "192.168.1.5";
String serverPath = "/uploadPedacos";
String serverPath2 = "/uploadImagem";
const int serverPort = 3000;

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


//Caminho da foto
#define FILE_PHOTO "/imagem.jpg"

//Variavel de controle
boolean takeNewPhoto = false;

//Variavel que armazena sempre a ultima foto
camera_fb_t* last_photo = NULL;


/*
//Referente ao liveview
#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

//httpd_handle_t stream_httpd = NULL;
*/

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { text-align:center; }
    .vert { margin-bottom: 10%; }
    .hori{ margin-bottom: 0%; }
  </style>
</head>
<body>
  <div id="container">
    <h2>ESP32-CAM Foto</h2>
    <p>
      <button onclick="rotatePhoto();">ROTATE</button>
      <button onclick="capturePhoto()">CAPTURE PHOTO</button>
      <button onclick="location.reload();">REFRESH PAGE</button>
    </p>
  </div>
  <div><img src="saved-photo" id="photo" width="70%"></div>
</body>
<script>
  var deg = 0;
  function capturePhoto() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', "/capture", true);
    xhr.send();
  }
  function rotatePhoto() {
    var img = document.getElementById("photo");
    deg += 90;
    if(isOdd(deg/90)){ document.getElementById("container").className = "vert"; }
    else{ document.getElementById("container").className = "hori"; }
    img.style.transform = "rotate(" + deg + "deg)";
  }
  function isOdd(n) { return Math.abs(n % 2) == 1; }
</script>
</html>)rawliteral";

//Função que imprime no monitor serial e no /log o servidor web a mensagemm
void Aprint(String mensagem)
{
  Serial.println(mensagem);

  //Requisição POST para imprimir na pagina web
  HTTPClient req;
  String url = serverESP + ":80/log";
  req.begin(url);
  req.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpResponseCode = req.POST(mensagem);


  if (httpResponseCode > 0) {
    Serial.print("void print() | HTTP Response code: ");
    Serial.println(httpResponseCode);
  } 
  else {
    Serial.print("void print() | Error on HTTP request: ");
    Serial.println(httpResponseCode);
  }
  
  req.end();
}


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
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound())
  {
    config.frame_size = FRAMESIZE_SVGA;
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
void salvarImagem(camera_fb_t* img) 
{
  Serial.printf("Picture file name: %s\n", FILE_PHOTO);

  // Abre o arquivo no modo de escrita
  File file = SPIFFS.open(FILE_PHOTO, FILE_WRITE);

  // Verifica se o arquivo foi aberto com sucesso
  if(!file) {
    Serial.println("Falha ao criar o arquivo");
  }

  else 
  {
    // buf - Imagem, len - Tamanho
    file.write(img->buf, img->len); 
    Serial.print("A foto foi salva em  ");
    Serial.print(FILE_PHOTO);
    Serial.print(" - Size: ");
    Serial.print(file.size());
    Serial.println(" bytes");
  }

  file.close();

}

bool checarSalvamento(FS &fs)
{
  File f_pic = fs.open( FILE_PHOTO );
  unsigned int pic_sz = f_pic.size();
  return ( pic_sz > 100 );
} 

//Essa função tira a foto após 5s e salva no SPIFFS
void tiraFoto()
{
  //Bool que indica se a foto foi salva corretamente
  bool ok = 0; 

  //camera_fb_t* fb = NULL;  
  Aprint("tirando foto!");
  
  do{
    for(int i = 5; i >= 1; i--)
    {
      Serial.print("A foto será tirada em ");
      Serial.print(i);
      Serial.println(" segundos");
      delay(1000);
    }

    // Captura a imagem em JPEG
    //fb = esp_camera_fb_get();

    //Variavel global para teste de envio
    last_photo = esp_camera_fb_get();

    
    // Verifica se a imagem foi capturada com sucesso
    if(!last_photo) {
      Serial.println("Falha na captura da imagem");
    }
    else
    {
      Serial.println("Imagem capturada!");
    }

    //Salva a foto no esp32cam
    salvarImagem(last_photo);

    //Verifico se a foto foi salva corretamente
    ok = checarSalvamento(SPIFFS);
    
  }
  while(!ok);
  
  //do-while forma de loop que permite com que o bloco seja executado a primeira vez para ai se verificar a condição

}

//Envia a foto para a aplicação node.js
void enviarFotoJPEG()
{

  // Abre o arquivo de imagem salvo no SPIFFS
  File file = SPIFFS.open(FILE_PHOTO, FILE_READ);

  if (!file) {
    Serial.println("Falha ao abrir o arquivo de imagem");
    return;
  }

  // Lê o conteúdo do arquivo e armazena em um buffer
  uint8_t* buffer = (uint8_t*) malloc(file.size());
  file.read(buffer, file.size());

  // Envia a imagem para o servidor
  HTTPClient http;
  http.begin(serverUrl);

  // Anexa o arquivo de imagem como um anexo à solicitação HTTP POST
  http.addHeader("Content-Disposition", "attachment; filename=imagem.jpg");
  http.addHeader("Content-Type", "image/jpeg");
  http.addHeader("Content-Length", String(file.size()));

  Serial.print("Enviando a imagem para o servidor: ");
  Serial.println(serverUrl); 

  int httpResponseCode = http.POST(buffer, file.size());  
  
  if(httpResponseCode > 0) {

    Serial.print("Tamanho da imagem: ");
    Serial.println(file.size());

    Serial.print("Resposta do servidor: ");   
    Serial.println(httpResponseCode);
    Serial.println(http.getString());

  } else {
    Serial.println("Falha ao enviar a imagem");
  }
  
  free (buffer);

  //Fecha o arquivo
  file.close();

  http.end();

}

//Envia a foto para um servidor externo rodando um script php
String sendPhotoHTTPS() {
  String getAll;
  String getBody;
  
  Serial.println("sendPhotoHTTPS | Connecting to server: " + serverNameS);
  
  cliente.setInsecure(); //skip certificate validation
  if (cliente.connect(serverNameS.c_str(), serverPortS)) {
    Serial.println("sendPhotoHTTPS | Connection successful!");    
    String head = "--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"imageFile\"; filename=\"esp32-cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--RandomNerdTutorials--\r\n";

    uint32_t imageLen = last_photo->len;
    uint32_t extraLen = head.length() + tail.length();
    uint32_t totalLen = imageLen + extraLen;
  
    cliente.println("POST " + serverPathS + " HTTP/1.1");
    cliente.println("Host: " + serverNameS);
    cliente.println("Content-Length: " + String(totalLen));
    cliente.println("Content-Type: multipart/form-data; boundary=RandomNerdTutorials");
    cliente.println();
    cliente.print(head);
  
    uint8_t *fbBuf = last_photo->buf;
    size_t fbLen = last_photo->len;
    for (size_t n=0; n<fbLen; n=n+1024) {
      if (n+1024 < fbLen) {
        cliente.write(fbBuf, 1024);
        fbBuf += 1024;
      }
      else if (fbLen%1024>0) {
        size_t remainder = fbLen%1024;
        cliente.write(fbBuf, remainder);
      }
    }   
    cliente.print(tail);
    
    int timoutTimer = 10000;
    long startTimer = millis();
    boolean state = false;
    
    while ((startTimer + timoutTimer) > millis()) {
      Serial.print(".");
      delay(100);      
      while (cliente.available()) {
        char c = cliente.read();
        if (c == '\n') {
          if (getAll.length()==0) { state=true; }
          getAll = "";
        }
        else if (c != '\r') { getAll += String(c); }
        if (state==true) { getBody += String(c); }
        startTimer = millis();
      }
      if (getBody.length()>0) { break; }
    }
    Serial.println("sendPhotoHTTPS | ");
    cliente.stop();
    Serial.println(getBody);
  }
  else {
    getBody = "Connection to " + serverNameS +  " failed.";
    Serial.println("sendPhotoHTTPS |" +getBody);
  }
  return getBody;
}

//Parte de Stream da Camera -> Ver depois pois da conflito entre a blbioteca ESPAsyncWebserver e a esp_http_server
/*
static esp_err_t stream_handler(httpd_req_t *req){
  camera_fb_t * fb = NULL;
  esp_err_t res = ESP_OK;
  size_t _jpg_buf_len = 0;
  uint8_t * _jpg_buf = NULL;
  char * part_buf[64];
  res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
  if(res != ESP_OK){
    return res;
  }
  while(true){
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      res = ESP_FAIL;
    } else {
      if(fb->width > 400){
        if(fb->format != PIXFORMAT_JPEG){
          bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
          esp_camera_fb_return(fb);
          fb = NULL;
          if(!jpeg_converted){
            Serial.println("JPEG compression failed");
            res = ESP_FAIL;
          }
        } else {
          _jpg_buf_len = fb->len;
          _jpg_buf = fb->buf;
        }
      }
    }
    if(res == ESP_OK){
      size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);
      res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
    }
    if(res == ESP_OK){
      res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
    }
    if(res == ESP_OK){
      res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
    }
    if(fb){
      esp_camera_fb_return(fb);
      fb = NULL;
      _jpg_buf = NULL;
    } else if(_jpg_buf){
      free(_jpg_buf);
      _jpg_buf = NULL;
    }
    if(res != ESP_OK){
      break;
    }
  }
  return res;
}

void streamCamera(){
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 80;
  httpd_uri_t index_uri = {
    .uri       = "/live",
    .method    = HTTP_GET,
    .handler   = stream_handler,
    .user_ctx  = NULL
  };  
  
  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(stream_httpd, &index_uri);
  }
}
*/

//Abre a foto salva no SPIFFS do ESP32CAM
void servidorWeb()
{

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", index_html);
  });

  server.on("/capture", HTTP_GET, [](AsyncWebServerRequest * request) {
    takeNewPhoto = true;
    request->send_P(200, "text/plain", "Taking Photo");
  });

  server.on("/saved-photo", [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, FILE_PHOTO, "image/jpg", false);
  });

  // Rota para exibir as mensagens do buffer serial
  server.on("/log", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<html><body><h1>Messages:</h1><ul>";
    html += messages;
    html += "</ul></body></html>";
    request->send(200, "text/html", html);
  });

  server.on("/log", HTTP_POST, [](AsyncWebServerRequest *request){
    if (request->hasParam("message")) {
      AsyncWebParameter* param = request->getParam("message");
      messages += "<li>" + param->value() + "</li>";
    }
    request->send(200, "text/plain", "Message received");
  });

  // Start server
  server.begin();

  Serial.println("Servidor Iniciado!");


}


void setup() {

  Serial.begin(115200);

  initCamera();

  //Inicilizando SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    ESP.restart();
  }
  else {
    delay(500);
    Serial.println("SPIFFS mounted successfully");
  }

  // Inicializa a comunicação serial com o ESP32
  //SerialESP32.begin(9600, SERIAL_8N1, PORT_RX, PORT_TX); 

  WiFi.begin(ssid, password);
  Serial.print("Conectando à rede Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conectado à rede Wi-Fi");
  Serial.print("Endereço de IP: http://");

  IPAddress ipESP = WiFi.localIP();
  serverESP = ipESP.toString();

  Serial.println(ipESP);


  servidorWeb();

}

void loop() {

  if (takeNewPhoto) {
    tiraFoto();
    enviarFotoJPEG();
    
    takeNewPhoto = false;
  }
  delay(1);

  String mensagem = "foto";

  // Verifica se há dados disponíveis na porta serial
  /*
  if (SerialESP32.available()) 
  {
    mensagem = SerialESP32.readStringUntil('\n'); // Lê a string enviada pelo ESP32
    Serial.println("Mensagem recebida do ESP32: " + mensagem); // Exibe a mensagem no monitor serial
  }
  */

  /*
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
  */

  //Ao final de todo loop ele libera o framebuffer da ultima foto tirada
  if(last_photo)
    esp_camera_fb_return(last_photo);

}