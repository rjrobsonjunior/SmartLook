#include "includes.h"

String messages = "";
String serverESP = "";

camera_fb_t * photo = NULL; // pointer

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
      <button id="captureButton" onclick="capturePhoto()">TIRAR FOTO</button>
      <button onclick="location.reload();">RECARREGAR</button>
      <button onclick="analisaFoto()">ANALISAR</button>
      <button onclick="qrCode()">QR CODE</button>
    </p>
  </div>
  <div><img src="saved-photo" id="photo" width="70%"></div>
</body>
<script>
  function capturePhoto() {
    var captureButton = document.getElementById("captureButton");
    captureButton.disabled = true;  // Desativa o botão

    setTimeout(function() {
      captureButton.disabled = false;  // Reativa o botão após 5 segundos
      location.reload();  // Recarrega a página
    }, 5000);


    var xhr = new XMLHttpRequest();
    xhr.open('GET', "/capture", true);
    xhr.send();
  }

  function analisaFoto() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', "/analisaFoto", true);
    xhr.send();
  }

  function qrCode() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', "/analisaQR", true);
    xhr.send();
  }

  var deg = 0;
  function rotatePhoto() {
    var img = document.getElementById("photo");
    deg += 90;
    if(isOdd(deg/90)){ document.getElementById("container").className = "vert"; }
    else{ document.getElementById("container").className = "hori"; }
    img.style.transform = "rotate(" + deg + "deg)";
  }

</script>
</html>)rawliteral";

//Função que imprime no monitor serial e no /log o servidor web a mensagemm
void Aprint(String mensagem)
{
  Serial.println(mensagem);

  //Requisição POST para imprimir na pagina web
  HTTPClient req;
  String url = "http://192.168.1.19/log";
  mensagem = "message="+mensagem;
  req.begin(url);
  req.addHeader("Content-Type", "application/x-www-form-urlencoded");
  req.addHeader("Content-Length", String(mensagem.length()));

  int httpResponseCode = req.POST(mensagem);


  if (httpResponseCode > 0) {
    Serial.print("void Aprint() | HTTP Response code: ");
    Serial.println(httpResponseCode);
  } 
  else {
    Serial.print("void Aprint() | Error on HTTP request: ");
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

bool changeFrameSize(framesize_t frameSize) {
  sensor_t *s = esp_camera_sensor_get();
  if (s == NULL) {
    //return ESP_ERR_CAMERA_NOT_DETECTED;
    return false;
  }

  if (s->status.framesize == frameSize) {
    return false;
  }

  s->set_framesize(s, frameSize);
  //s->set_special_effect(s, frameSize == FRAMESIZE_UXGA ? 1 : 0); // 0 to 6 (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)
  Serial.print("Frame size changed to ");
  Serial.println(frameSize);
  delay(400); // It seems to take a while before the new frame size is active. If the photo is taken too early, only the amount of bytes needed to fill the old frame size are captured, though they seem to be captured at the new frame size.

  return true;
}


void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  // TODO: this was all copied from an example and most of it can probably be deleted
  if(type == WS_EVT_CONNECT){
    Serial.printf("ws[%s][%u] connect\n\r", server->url(), client->id());
    //client->printf("Hello Client %u :)", client->id());
    //client->ping();
  } else if(type == WS_EVT_DISCONNECT){
    Serial.printf("ws[%s][%u] disconnect\n\r", server->url(), client->id());
  } else if(type == WS_EVT_ERROR){
    Serial.printf("ws[%s][%u] error(%u): %s\n\r", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
  } else if(type == WS_EVT_PONG){
    Serial.printf("ws[%s][%u] pong[%u]: %s\n\r", server->url(), client->id(), len, (len)?(char*)data:"");
  } else if(type == WS_EVT_DATA){
    AwsFrameInfo * info = (AwsFrameInfo*)arg;
    String msg = "";
    if(info->final && info->index == 0 && info->len == len){
      //the whole message is in a single frame and we got all of it's data
      Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len);

      if(info->opcode == WS_TEXT){
        for(size_t i=0; i < info->len; i++) {
          msg += (char) data[i];
        }
      } else {
        char buff[3];
        for(size_t i=0; i < info->len; i++) {
          sprintf(buff, "%02x ", (uint8_t) data[i]);
          msg += buff ;
        }
      }
      Serial.printf("%s\n\r",msg.c_str());

      if(info->opcode == WS_TEXT) {
        //client->text("I got your text message");
      } else {
        //client->binary("I got your binary message");
      }
    } else {
      //message is comprised of multiple frames or the frame is split into multiple packets
      if(info->index == 0){
        if(info->num == 0)
          Serial.printf("ws[%s][%u] %s-message start\n\r", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
        Serial.printf("ws[%s][%u] frame[%u] start[%llu]\n\r", server->url(), client->id(), info->num, info->len);
      }

      Serial.printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT)?"text":"binary", info->index, info->index + len);

      if(info->opcode == WS_TEXT){
        for(size_t i=0; i < len; i++) {
          msg += (char) data[i];
        }
      } else {
        char buff[3];
        for(size_t i=0; i < len; i++) {
          sprintf(buff, "%02x ", (uint8_t) data[i]);
          msg += buff ;
        }
      }
      Serial.printf("%s\n\r",msg.c_str());

      if((info->index + len) == info->len){
        Serial.printf("ws[%s][%u] frame[%u] end[%llu]\n\r", server->url(), client->id(), info->num, info->len);
        if(info->final){
          Serial.printf("ws[%s][%u] %s-message end\n\r", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
          if(info->message_opcode == WS_TEXT) {
            //client->text("I got your text message");
          } else {
            //client->binary("I got your binary message");
          }
        }
      }
    }
  }
}

esp_err_t streamPhoto( void ) {
  photo = NULL; // pointer

  if (changeFrameSize(FRAMESIZE_VGA)) {
    delay(1000); 
  }

  photo = esp_camera_fb_get();

  if (!photo) {
    Serial.println("Camera capture failed");
    return ESP_OK;
  }

  if (photo->format == PIXFORMAT_JPEG) {
    //Serial.println("fb->format is jpeg");
  } else {
    Serial.println("fb->format is NOT jpeg");
  }

  AsyncWebSocket::AsyncWebSocketClientLinkedList clients = asyncWs.getClients();
  clients.front()->binary((uint8_t*)photo->buf, photo->len);

  esp_camera_fb_return(photo);

  return ESP_OK;
}

void desligarCamera()
{
  esp_camera_deinit();
}

void piscarLED()
{
  //INDICAR QUE O ESPCAM ESTA LIGANDO
  digitalWrite(FLASH_GPIO_NUM, HIGH);
  delay(500); 
  digitalWrite(FLASH_GPIO_NUM, LOW); 
}

void initLittleFS()
{
  //Inicilizando LittleFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting LittleFS");
    ESP.restart();
  }
  else {
    delay(500);
    Serial.println("LittleFS mounted successfully");
  }
}

void connectWiFi()  
{
  WiFi.begin(ssid, password);
  Serial.print("Conectando à rede Wi-Fi");
  
  for (int i = 0; i < 15 && WiFi.status() != WL_CONNECTED; i++) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  if (WiFi.status() != WL_CONNECTED) {
    ESP.restart();
  }

  //Desativa o modo de suspensão (sleep mode) da WiFi
  WiFi.setSleep(false);

  Serial.println("");
  Serial.println("Conectado à rede Wi-Fi");
  Serial.print("Endereço de IP: http://");

  IPAddress ipESP = WiFi.localIP();
  serverESP = ipESP.toString();

  Serial.println(ipESP);
}

// Função para salvar a imagem no LittleFS
void salvarImagemLITTLEFS(camera_fb_t* img) 
{
  Serial.printf("Picture file name: %s\n", FILE_PHOTO);
  
  
  if (SPIFFS.remove(FILE_PHOTO)) {
    Serial.println("Imagem excluída com sucesso");
  } else {
    Serial.println("Falha ao excluir a imagem");
  }
  

  // Abre o arquivo no modo de escrita
  File file = SPIFFS.open(FILE_PHOTO, FILE_WRITE);
  Serial.print(" - Size: ");
  Serial.println(file.size());

  // Verifica se o arquivo foi aberto com sucesso
  if(!file) {
    Serial.println("Falha ao criar o arquivo");
  }

  else 
  {
    // buf - Imagem, len - Tamanho
    int result = file.write(img->buf, img->len);
    Serial.print("Result = ");
    Serial.println(result); 
    
    /* DEBUG */
    Serial.print("A foto foi salva em  ");
    Serial.print(FILE_PHOTO);
    Serial.print(" - Size: ");
    Serial.print(file.size());
    Serial.print(" bytes");
    Serial.print(" || ");
    Serial.print("img->len =  ");
    Serial.println(img->len);
  }

  // Verifica se o arquivo foi salvo corretamente
  if (file.size() == img->len) {
    Serial.println("Imagem salva corretamente");
  } else {
    Serial.println("Falha ao salvar a imagem");
  } 

  file.close();

}

bool checarSalvamento(FS &fs)
{
  File f_pic = fs.open( FILE_PHOTO );
  unsigned int pic_sz = f_pic.size();
  return ( pic_sz > 100 );
} 

//Essa função tira a foto após 5s e salva no LittleFS
void tiraFoto()
{
  //Bool que indica se a foto foi salva corretamente
  bool ok = 0; 
  camera_fb_t* fb = NULL;  

  changeFrameSize(FRAMESIZE_UXGA);
  
  do{

    
    for(int i = 3; i>0; i--)
    {
      Serial.print("Foto sera tirada em ");
      Serial.println(i);
      digitalWrite(FLASH_GPIO_NUM, HIGH);
      delay(300);
      digitalWrite(FLASH_GPIO_NUM, LOW);
      delay(700);
    }
    

    fb = esp_camera_fb_get();
    Serial.println("Tirei uma foto!");
    
    // Verifica se a imagem foi capturada com sucesso
    if(!fb) {
      Serial.println("Falha na captura da imagem");
    }
    else
    {
      Serial.println("Imagem capturada!");
    }

    //Salva a foto no esp32cam
    salvarImagemLITTLEFS(fb);

    //Verifico se a foto foi salva corretamente
    ok = checarSalvamento(SPIFFS);
    
  }
  while(!ok);  //do-while forma de loop que permite com que o bloco seja executado a primeira vez para ai se verificar a condição

  esp_camera_fb_return(fb);

}

//(MULTER) Envia a foto para a aplicação node.js que ja compara para ver se existe no banco de dados
String EnvioAnaliseIMGMulter()
{
  String resposta_servidor = "";

  // Abre o arquivo de imagem salvo no LittleFS
  File file = SPIFFS.open(FILE_PHOTO, FILE_READ);
  
  if (!file) {
    Serial.println("Falha ao abrir o arquivo de imagem");
    resposta_servidor = "Falha ao abrir o arquivo de imagem";
  }

  else{  
    // Lê o conteúdo do arquivo e armazena em um buffer
    size_t size = file.size();
    uint8_t* buffer = (uint8_t*) malloc(size);
    file.read(buffer, size);

    WiFiClient client;
    if (!client.connect(serverIP, 8800)) {
      Serial.println("Falha ao conectar ao servidor");
      resposta_servidor = "Falha ao conectar ao servidor";
      return resposta_servidor;
    }

    // Gerar um valor de boundary único | Valor serve para identificar o começo e final da requisição
    String boundary = "--------------------------" + String(millis());

    // Criação do corpo da requisição
    String requestBody = "--" + boundary + "\r\n";
    requestBody += "Content-Disposition: form-data; name=\"imagem\"; filename=\"imagem.jpg\"\r\n";
    requestBody += "Content-Type: image/jpeg\r\n";
    requestBody += "\r\n";
    requestBody += String((char*)buffer, size) + "\r\n";
    requestBody += "--" + boundary + "--\r\n";

    // Construir a requisição HTTP
    String request = "POST /recognition HTTP/1.1\r\n";
    request += "Host: " + String(serverIP) + "\r\n";
    request += "Content-Type: multipart/form-data; boundary=" + boundary + "\r\n";
    request += "Content-Length: " + String(requestBody.length()) + "\r\n";
    request += "\r\n";
    request += requestBody;

    Serial.print("ESPCAM - REQUISIÇÃO | Enviando a imagem para o servidor: ");
    Serial.println(serverUrlANALISE); 

    // Envie a requisição HTTP
    client.print(request);

    int statusCode = 0;

    // Aguarde a resposta do servidor
    while (client.connected()) {

      if (client.available()) {

        // Leia e processe a resposta do servidor
        String response = client.readString();
        Serial.println("\nESPCAM - REQUISIÇÃO | Codigo completo da resposta da requisição:\n");
        Serial.println(response);

        // Extrair o código de resposta
        int statusCodeStart = response.indexOf(' ') + 1;
        int statusCodeEnd = response.indexOf(' ', statusCodeStart);
        String statusCodeString = response.substring(statusCodeStart, statusCodeEnd);
        statusCode = atoi(statusCodeString.c_str());

        // Extrair a última linha da resposta
        int lastNewlinePos = response.lastIndexOf('\n');
        String lastLine = response.substring(lastNewlinePos + 1);

        resposta_servidor = lastLine;
        break;
      }

      esp_task_wdt_reset();

    }

    Serial.println("-----------------------------------------");
    Serial.print("Codigo da requisição = ");
    Serial.println(statusCode);
    Serial.println("Resposta da ultima linha = " + resposta_servidor);
    Serial.println("-----------------------------------------");

    // Feche a conexão
    client.stop();

    // Verifique a resposta
    if (statusCode == 200) {

      // Processar a resposta do servidor
      Serial.println();
      Serial.println("Analise realizada com sucesso!");
      Serial.println("Resposta do servidor: " + resposta_servidor);
    }
    else if(statusCode == 550)
    {
      // Lidar com a falha na comunicação
      Serial.println("Rosto nao encontrado na imagem!");
      resposta_servidor = "550";
    }
    else
    {
      Serial.println("REQUISIÇÃO | Falha na requisição POST!");
    }

    //Fecha o arquivo
    file.close();

    free(buffer);

    Serial.println("REQUISIÇÃO | Resposta = " + resposta_servidor);
  }

  return resposta_servidor;
}

//(MULTER) Envia a foto para a aplicação node.js que ja compara para ver se existe no banco de dados
String EnvioAnaliseQRCODEMulter()
{
  String resposta_servidor = "";

  // Abre o arquivo de imagem salvo no LittleFS
  File file = SPIFFS.open(FILE_PHOTO, FILE_READ);
  
  if (!file) {
    Serial.println("Falha ao abrir o arquivo de imagem");
    resposta_servidor = "Falha ao abrir o arquivo de imagem";
  }

  else{  
    // Lê o conteúdo do arquivo e armazena em um buffer
    size_t size = file.size();
    uint8_t* buffer = (uint8_t*) malloc(size);
    file.read(buffer, size);

    WiFiClient client;
    if (!client.connect(serverIP, 8800)) {
      Serial.println("Falha ao conectar ao servidor");
      resposta_servidor = "Falha ao conectar ao servidor";
      return resposta_servidor;
    }

    // Gerar um valor de boundary único | Valor serve para identificar o começo e final da requisição
    String boundary = "--------------------------" + String(millis());

    // Criação do corpo da requisição
    String requestBody = "--" + boundary + "\r\n";
    requestBody += "Content-Disposition: form-data; name=\"qrcode\"; filename=\"imagem.jpg\"\r\n";
    requestBody += "Content-Type: image/jpeg\r\n";
    requestBody += "\r\n";
    requestBody += String((char*)buffer, size) + "\r\n";
    requestBody += "--" + boundary + "--\r\n";

    // Construir a requisição HTTP
    String request = "POST /qrCode HTTP/1.1\r\n";
    request += "Host: " + String(serverIP) + "\r\n";
    request += "Content-Type: multipart/form-data; boundary=" + boundary + "\r\n";
    request += "Content-Length: " + String(requestBody.length()) + "\r\n";
    request += "\r\n";
    request += requestBody;

    // Envie a requisição HTTP
    client.print(request);

    int statusCode = 0;

    // Aguarde a resposta do servidor
    while (client.connected()) {

      if (client.available()) {

        // Leia e processe a resposta do servidor
        String response = client.readString();
        Serial.println("\nREQUISIÇÃO | Codigo completo da resposta da requisição:\n");
        Serial.println(response);

        // Extrair o código de resposta
        int statusCodeStart = response.indexOf(' ') + 1;
        int statusCodeEnd = response.indexOf(' ', statusCodeStart);
        String statusCodeString = response.substring(statusCodeStart, statusCodeEnd);
        statusCode = atoi(statusCodeString.c_str());

        // Extrair a última linha da resposta
        int lastNewlinePos = response.lastIndexOf('\n');
        String lastLine = response.substring(lastNewlinePos + 1);

        resposta_servidor = lastLine;
        break;
      }

      esp_task_wdt_reset();

    }

    Serial.println("-----------------------------------------");
    Serial.print("Codigo da requisição = ");
    Serial.println(statusCode);
    Serial.println("Resposta da ultima linha = " + resposta_servidor);
    Serial.println("-----------------------------------------");

    // Feche a conexão
    client.stop();

    // Verifique a resposta
    if (statusCode == 200) {

      // Processar a resposta do servidor
      Serial.println();
      Serial.println("Analise realizada com sucesso!");
      Serial.println("Resposta do servidor: " + resposta_servidor);
    }
    else if(statusCode == 450)
    {
      // Lidar com a falha na comunicação
      Serial.println("QR Code nao identificado na imagem!");
      resposta_servidor = "550";
    }
    else
    {
      Serial.println("REQUISIÇÃO | Falha na requisição POST!");
    }

    //Fecha o arquivo
    file.close();

    free(buffer);

    Serial.println("REQUISIÇÃO | Resposta = " + resposta_servidor);
  }

  return resposta_servidor;
}


bool tirarFotoServidor()
{
  //Nessa função eu tiro a foto e mando para a aplicação web. 

  //A aplicação pega a foto e exibe, se o usuario quiser ele salva a foto e manda para a analise, caso nao, ele manda tirar outra foto

  camera_fb_t* foto = esp_camera_fb_get();
  String resposta = "";


  if(!foto)
  {
    Serial.println("Erro ao capturar a foto!");
    return false;;
  }

  //Envio da foto via requisiçao POST
  WiFiClient client;
  if (!client.connect(serverIP, 8800)) {
    Serial.println("Falha ao conectar ao servidor");
    return false;;
  }

  // Gerar um valor de boundary único | Valor serve para identificar o começo e final da requisição
  String boundary = "--------------------------" + String(millis());

  client.println("POST /upload HTTP/1.1");
  client.println("Host: " + String(serverIP));
  client.println("Content-Type: multipart/form-data; boundary=" + boundary);
  client.println();

  client.println(boundary);
  client.println("Content-Disposition: form-data; name=\"imagem\"; filename=\"photo.jpg\"");
  client.println("Content-Type: image/jpeg");
  client.println();

  // Escreva os dados da imagem no corpo da requisição
  client.write(foto->buf, foto->len);

  client.println();
  client.print(boundary);
  client.println("--");

  Serial.print("ESPCAM - tirarFotoServidor | Enviando a imagem para o servidor: ");
  Serial.println(serverUrlANALISE); 

  int statusCode = 0;

  // Aguarde a resposta do servidor
  while (client.connected()) {

    if (client.available()) {

      // Leia e processe a resposta do servidor
      String response = client.readString();
      Serial.println("\nESPCAM - tirarFotoServidor | Codigo completo da resposta da requisição:\n");
      Serial.println(response);

      // Extrair o código de resposta
      int statusCodeStart = response.indexOf(' ') + 1;
      int statusCodeEnd = response.indexOf(' ', statusCodeStart);
      String statusCodeString = response.substring(statusCodeStart, statusCodeEnd);
      statusCode = atoi(statusCodeString.c_str());

      // Extrair a última linha da resposta
      int lastNewlinePos = response.lastIndexOf('\n');
      String lastLine = response.substring(lastNewlinePos + 1);
      resposta = lastLine;
      break;
    }

    esp_task_wdt_reset();

  }

  Serial.println("-----------------------------------------");
  Serial.print("Codigo da requisição = ");
  Serial.println(statusCode);
  Serial.println("Resposta da ultima linha = " + resposta);
  Serial.println("-----------------------------------------");

  // Feche a conexão
  client.stop();

  // Verifique a resposta
  if (statusCode == 200) {

    Serial.println("Foto tirada e salvada com sucesso!");
  }

  else
  {
    Serial.println("REQUISIÇÃO | Falha na requisição POST!");
    return false;
  }

  return true;
}

//Abre a foto salva no LittleFS do ESP32CAM
void servidorWeb()
{
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", index_html);
  });

  //Captura a foto alterando o bool
  server.on("/capture", HTTP_GET, [](AsyncWebServerRequest * request) {
    takeNewPhoto = true;
    request->send_P(200, "text/plain", "Foto Capturada!");
  });

  //Captura a foto atraves das funçõeos
  server.on("/analisaFoto", HTTP_GET, [](AsyncWebServerRequest * request) {

    String resposta = "";
    int cont = 0;

    /*
    //Repitir se a imagem nao conter uma face
    do{
      
      //Se for a primeira iteração do loop, resposta == "". So precisa tirar uma nova foto se a resposta for == 550
      if(resposta != "")
      {
        Serial.println("Tirando uma nova foto");
        tiraFoto();
        cont++;
      }

      resposta = EnvioAnaliseIMGMulter();

    }
    while(resposta == "550" || cont < 3);
    */

    resposta = EnvioAnaliseIMGMulter();

    //Ocorreu algum erro na analise da imagem
    if(resposta != "true" && resposta != "false")
    {
      request->send_P(500, "text/plain", resposta.c_str());
      Serial.println("Erro: " + resposta);  
    }
    else{
      request->send_P(200, "text/plain", resposta.c_str());
      Serial.println(resposta);
    }




  });

   //Captura a foto atraves das funçõeos
  server.on("/analisaQR", HTTP_GET, [](AsyncWebServerRequest * request) {

    String resposta = "";
    int cont = 0;

    resposta = EnvioAnaliseQRCODEMulter();

    //Ocorreu algum erro na analise da imagem
    if(resposta != "true" && resposta != "false")
    {
      request->send_P(500, "text/plain", resposta.c_str());
      Serial.println("Erro: " + resposta);  
    }
    else{
      request->send_P(200, "text/plain", resposta.c_str());
      Serial.println(resposta);
    }

  });

  server.on("/testeFoto", HTTP_GET, [](AsyncWebServerRequest * request) {


    //Apenas tira a foto e manda para o servidor
    bool resposta = tirarFotoServidor();

    //Ocorreu algum erro na analise da imagem
    if(!resposta)
    {
      request->send_P(500, "text/plain", "Falha no envio da imagem");  
    }
    else{
      request->send_P(200, "text/plain", "Imagem enviada e salva com sucesso!");
    }

  }); 



  //Mostra a foto na memoria LittleFS do ESPCAM
  server.on("/saved-photo", [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, FILE_PHOTO, "image/jpg", false);
  });

  server.on("/stream", HTTP_GET, [](AsyncWebServerRequest * request) {
      
      return;
      Serial.println("Lets stream it up!");

      camera_fb_t * fb = NULL;
      size_t fb_buffer_sent = 0;

      AsyncWebServerResponse *response = request->beginChunkedResponse(_STREAM_CONTENT_TYPE, [fb, fb_buffer_sent](uint8_t *buffer, size_t maxLen, size_t index) mutable -> size_t {
        uint8_t *end_of_buffer = buffer;
        size_t remaining_space = maxLen;

        if (!fb) {
          fb = esp_camera_fb_get();
          if (!fb) {
            Serial.println("Camera capture failed");
            return 0;
          }

          //res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
          size_t len = snprintf((char *)end_of_buffer, remaining_space, _STREAM_BOUNDARY);
          end_of_buffer += len;
          remaining_space -= len;

          size_t hlen = snprintf((char *)end_of_buffer, remaining_space, _STREAM_PART, fb->len);
          end_of_buffer += hlen;
          remaining_space -= hlen;

        }

        //res = httpd_resp_send_chunk(req, (const char *)fb->buf, fb->len);
        //TODO: only send max len and later finish sending the buffer
        size_t fb_bytes_to_send = min(remaining_space, fb->len-fb_buffer_sent);
        memcpy((char *)end_of_buffer, fb->buf+fb_buffer_sent, fb_bytes_to_send);
        end_of_buffer += fb_bytes_to_send;
        remaining_space -= fb_bytes_to_send;
        fb_buffer_sent += fb_bytes_to_send;

        if(fb && fb_buffer_sent == fb->len){
          esp_camera_fb_return(fb);
          fb = NULL;
          fb_buffer_sent = 0;
        }

        return maxLen - remaining_space;
      });

      response->addHeader("Access-Control-Allow-Origin", "*");
      request->send(response);
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

  //Iniicaliza o flash
  pinMode(FLASH_GPIO_NUM, OUTPUT);

  initCamera();
  piscarLED();
  initLittleFS();
  connectWiFi();

  asyncWs.onEvent(onWsEvent);
  server.addHandler(&asyncWs);

  //Permite o acesso de clientes de diferentes origens a recursos específicos ao servidor
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

  servidorWeb();

  counter = 0;

}

void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (takeNewPhoto) {
    tiraFoto();
    takeNewPhoto = false;
  } else if (counter % 100 == 0 && !asyncWs.getClients().isEmpty()) {
    streamPhoto();
  }
  counter++;

  asyncWs.cleanupClients();

  delay(3);

}