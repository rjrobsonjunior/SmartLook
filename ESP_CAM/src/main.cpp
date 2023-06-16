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
      <button onclick="qrCode()">TESTE</button>
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
    xhr.open('GET', "/testeFoto", true);
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
void tiraFotoSPIFFS()
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

//Nessa função eu tiro a foto e mando para a aplicação web. 
bool tirarFotoServidor()
{
  //A aplicação pega a foto e exibe, se o usuario quiser ele salva a foto e manda para a analise, caso nao, ele manda tirar outra foto
  String resposta = "";
  bool foto_foi_tirada = false;
  int cont = 0;
  camera_fb_t* foto;

  //Repete por ate tres vezes a captura de imagem ate que de certo
  do{

    //Limite de tentatitivas para tirar foto
    if(cont == 3)
    {
      return false;
    }

    delay(100);

    //Contador de três segundos para tirar a foto
    for(int i = 3; i>0; i--)
    {
      Serial.print("Foto sera tirada em ");
      Serial.println(i);
      digitalWrite(FLASH_GPIO_NUM, HIGH);
      delay(300);
      digitalWrite(FLASH_GPIO_NUM, LOW);
      delay(700);
    }
    
    delay(500);

    foto = esp_camera_fb_get();
  
    if(!foto)
    {
      Serial.println("Erro ao capturar a foto!");
      foto_foi_tirada = false;
    }

    else
    {
      Serial.println("Tirei uma foto!");
      foto_foi_tirada = true;
    }

    cont++;

  } while(!foto_foi_tirada);

  //Envio da foto via requisiçao POST
  WiFiClient client;
  if (!client.connect(serverIP, 8800)) {
    Serial.println("Falha ao conectar ao servidor");
    return false;;
  }

  // Gerar um valor de boundary único | Valor serve para identificar o começo e final da requisição
  String boundary = "--------------------------" + String(millis());

  // Criação do corpo da requisição
  String requestBody = "--" + boundary + "\r\n";
  requestBody += "Content-Disposition: form-data; name=\"imagem\"; filename=\"imagem.jpg\"\r\n";
  requestBody += "Content-Type: image/jpeg\r\n";
  requestBody += "\r\n";
  requestBody += String((char*)foto->buf, foto->len) + "\r\n";
  requestBody += "--" + boundary + "--\r\n";

  // Construir a requisição HTTP
  String request = "POST /foto HTTP/1.1\r\n";
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
  esp_camera_fb_return(foto);

  // Verifique a resposta
  if (statusCode == 200) {

    Serial.println("Foto tirada e salvada no servidor com sucesso!");
  }

  else
  {
    Serial.println("REQUISIÇÃO | Falha na requisição POST!");
    return false;
  }

  return true;
}

String analiseFaceServidor()
{
  // Conectar ao servidor
  WiFiClient client;
  
  if (!client.connect(serverIP, portServer)) 
  {
    Serial.println("Falha na conexão com o servidor");
    return "500";
  }

  Serial.println("Mandando o espcam analisar a foto...");

  // Enviar a requisição GET
  client.print("GET /recognition HTTP/1.1\r\n");
  client.print("Host: 192.168.1.6\r\n");
  client.print("Connection: close\r\n\r\n");

  // Aguardar a resposta do servidor
  while (client.connected() && !client.available()) {
    delay(1);
  }

  // Ler a resposta do servidor
  String resposta = "";
  int statusCode = 0;

  while (client.available()) {
    resposta = client.readStringUntil('\n');
    Serial.println("analiseFaceServidor | Resposta completa do servidor:");
    Serial.println(client.readString());
  }

  // Extrair o código de resposta
  int statusCodeStart = resposta.indexOf(' ') + 1;
  int statusCodeEnd = resposta.indexOf(' ', statusCodeStart);
  String statusCodeString = resposta.substring(statusCodeStart, statusCodeEnd);
  statusCode = atoi(statusCodeString.c_str());
  

  // Analisar a resposta do servidor
  if (statusCode == 200) 
  {
    // Face reconhecida
    Serial.println("Face reconhecida!");

    // Extrair a última linha da resposta (Nome da pessoa)
    int lastNewlinePos = resposta.lastIndexOf('\n');
    String lastLine = resposta.substring(lastNewlinePos + 1);

    resposta = lastLine;
    
  }

  resposta = String(statusCode);

  // Fechar a conexão
  client.stop();
  return resposta;
} 

String analiseQRCODE()
{
  // Conectar ao servidor
  WiFiClient client;
  
  if (!client.connect(serverIP, portServer)) 
  {
    Serial.println("Falha na conexão com o servidor");
    return "false";
  }

  // Enviar a requisição GET
  client.print("GET /qrcodeAnalise HTTP/1.1\r\n");
  client.print("Host: 192.168.1.6\r\n");
  client.print("Connection: close\r\n\r\n");

  // Aguardar a resposta do servidor
  while (client.connected() && !client.available()) {
    delay(1);
  }

  // Ler a resposta do servidor
  String resposta = "";
  int statusCode = 0;

  while (client.available()) {
    resposta = client.readStringUntil('\n');
    Serial.println("analiseQRCODE | Resposta completa do servidor:");
    Serial.println(client.readString());
  }

  // Extrair o código de resposta
  int statusCodeStart = resposta.indexOf(' ') + 1;
  int statusCodeEnd = resposta.indexOf(' ', statusCodeStart);
  String statusCodeString = resposta.substring(statusCodeStart, statusCodeEnd);
  statusCode = atoi(statusCodeString.c_str());
  

  // Analisar a resposta do servidor
  if (statusCode == 200) 
  {
    // Face reconhecida
    Serial.println("Qr code reconhecido!");

    // Extrair a última linha da resposta (Nome da pessoa)
    int lastNewlinePos = resposta.lastIndexOf('\n');
    String lastLine = resposta.substring(lastNewlinePos + 1);

    resposta = lastLine;
    
  }
  
  resposta = "false";


  // Fechar a conexão
  client.stop();
  return resposta;

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
    //takeNewPhoto = true;
    bool resposta_bool = tirarFotoServidor();

    if(resposta_bool){
      request->send_P(200, "text/plain", "Foto Capturada!");
    }

    else{
      request->send_P(200, "text/plain", "Foto Capturada!");
    }
  });

  //Captura a foto atraves das funçõeos
  server.on("/reconhecimentoFacial", HTTP_GET, [](AsyncWebServerRequest * request) {

    //Tira a foto
    bool resposta_bool = tirarFotoServidor();
    
    if(!resposta_bool){
      request->send_P(500, "text/plain", "Erro na captura da foto!");
    }
    
    Serial.println("ESPCAM /reconhecimentoFacial | Foto capturada e enviada com sucesso!");
    
    String resposta = "";

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

    resposta = analiseFaceServidor();

    //Ocorreu algum erro na analise da imagem (Nao achou rosto ou outra coisa)
    if(resposta != "400" && resposta != "500" && resposta != "550")
    {
      request->send_P(200, "text/plain", resposta.c_str());
      Serial.println(resposta);
  
    }
    else{

      if(resposta == "400")
      {
        resposta = "Rosto nao consta no banco de dados!";

      }

      else if(resposta == "550")
      {
        resposta = "Rosto nao detectado!";
      } 
      
      else 
      {
        resposta = "Outro problema";
      }

      request->send_P(500, "text/plain", resposta.c_str());
      Serial.println("Erro: " + resposta);
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

  //Permite o acesso de clientes de diferentes origens a recursos específicos ao servidor
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

  servidorWeb();

}

void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (takeNewPhoto) {
    
  } 
  
  delay(3);

}