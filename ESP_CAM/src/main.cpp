#include "includes.h"

//Variavel de controle
boolean takeNewPhoto = false;

//Variavel que armazena sempre a ultima foto
camera_fb_t* last_photo = NULL;

WiFiClient client;
WiFiClientSecure cliente;

String messages = "";
String serverESP = "";

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
      <button onclick="capturePhoto()">TIRAR FOTO</button>
      <button onclick="location.reload();">RECARREGAR</button>
      <button onclick="analisaFoto()">ANALISAR</button>
    </p>
    <p id="countdown"></p>
  </div>
  <div><img src="saved-photo" id="photo" width="70%"></div>
</body>
<script>
  function capturePhoto() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', "/capture", true);
    xhr.send();

    // Inicia o contador
    var seconds = 5;
    var countdown = document.getElementById("countdown");
    countdown.innerHTML = seconds;
    var interval = setInterval(function() {
      seconds--;
      countdown.innerHTML = seconds;
      if (seconds == 0) {
        clearInterval(interval);
        countdown.innerHTML = "";
      }
    }, 1000);
  }

  function analisaFoto() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', "/analisaFoto", true);
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
  function isOdd(n) { return Math.abs(n % 2) == 1; }

</script>
</html>)rawliteral";

//Função que imprime no monitor serial e no /log o servidor web a mensagemm
void Aprint(String mensagem)
{
  Serial.println(mensagem);

  //Requisição POST para imprimir na pagina web
  HTTPClient req;
  //String url = serverESP + ":80/log";
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

  //Iniicaliza o flash
  pinMode(FLASH_GPIO_NUM, OUTPUT);

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

void piscarLED()
{
  //INDICAR QUE O ESPCAM ESTA LIGANDO
  digitalWrite(FLASH_GPIO_NUM, HIGH);
  delay(500); 
  digitalWrite(FLASH_GPIO_NUM, LOW); 
}

void initSPIFFS()
{
  //Inicilizando SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    ESP.restart();
  }
  else {
    delay(500);
    Serial.println("SPIFFS mounted successfully");
  }
}

void connectWiFi()
{
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
}

// Função para salvar a imagem no SPIFFS
void salvarImagemSPIFFS(camera_fb_t* img) 
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
  
  do{

    // Captura a imagem em JPEG
    //fb = esp_camera_fb_get();

    for(int i = 3; i>0; i--)
    {
      Serial.println("Foto sera tirada em " + i);
      digitalWrite(FLASH_GPIO_NUM, HIGH);
      delay(1000);
      digitalWrite(FLASH_GPIO_NUM, LOW);  
    }

    //Tirando a foto
    digitalWrite(FLASH_GPIO_NUM, HIGH);
    delay(200); 
    last_photo = esp_camera_fb_get();
    digitalWrite(FLASH_GPIO_NUM, LOW);

    
    // Verifica se a imagem foi capturada com sucesso
    if(!last_photo) {
      Serial.println("Falha na captura da imagem");
    }
    else
    {
      Serial.println("Imagem capturada!");
    }

    //Salva a foto no esp32cam
    salvarImagemSPIFFS(last_photo);

    //Verifico se a foto foi salva corretamente
    ok = checarSalvamento(SPIFFS);
    
  }
  while(!ok);  //do-while forma de loop que permite com que o bloco seja executado a primeira vez para ai se verificar a condição


}

//Envia a foto para a aplicação node.js que ja compara para ver se existe no banco de dados (FUNCIONANDO COM O FS)
String EnvioAnaliseIMG()
{
  String resposta_servidor = "";

  // Abre o arquivo de imagem salvo no SPIFFS
  File file = SPIFFS.open(FILE_PHOTO, FILE_READ);
  
  if (!file) {
    Serial.println("Falha ao abrir o arquivo de imagem");
    resposta_servidor = "Falha ao abrir o arquivo de imagem";
    return resposta_servidor;
  }

  else{  
    // Lê o conteúdo do arquivo e armazena em um buffer
    uint8_t* buffer = (uint8_t*) malloc(file.size());
    file.read(buffer, file.size());

    // Envia a imagem para o servidor
    HTTPClient http;
    http.begin(serverUrlANALISE);

    // Anexa o arquivo de imagem como um anexo à solicitação HTTP POST
    http.addHeader("Content-Disposition", "attachment; filename=imagem.jpg");
    http.addHeader("Content-Type", "image/jpeg");
    http.addHeader("Content-Length", String(file.size()));

    Serial.print("Enviando a imagem para o servidor: ");
    Serial.println(serverUrlANALISE); 

    int httpResponseCode = http.POST(buffer, file.size());  

    if(httpResponseCode == 200) {
      
      resposta_servidor = http.getString();
      Serial.print("Tamanho da imagem: ");
      Serial.println(file.size());

      Serial.print("Resposta do servidor: ");   
      Serial.println(httpResponseCode);
      Serial.println(resposta_servidor);

    } else {
      Serial.println("Falha ao enviar a imagem");
      resposta_servidor = "Falha ao enviar a imagem. Resposta do servidor: " + httpResponseCode;
    }
    
    free (buffer);

    //Fecha o arquivo
    file.close();

    http.end();
    return resposta_servidor;
  }

}

//(MULTER) Envia a foto para a aplicação node.js que ja compara para ver se existe no banco de dados
String EnvioAnaliseIMGMulter()
{
  String resposta_servidor = "";

  // Abre o arquivo de imagem salvo no SPIFFS
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

    Serial.print("REQUISIÇÃO | Enviando a imagem para o servidor: ");
    Serial.println(serverUrlANALISE); 

    // Envie a requisição HTTP
    client.print(request);

    int statusCode = 0;
    String statusCode2 = "";

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

//Abre a foto salva no SPIFFS do ESP32CAM
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

  //Mostra a foto na memoria SPIFFS do ESPCAM
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
  piscarLED();
  initSPIFFS();
  connectWiFi();
  servidorWeb();

}

void loop() {

  if (takeNewPhoto) {
    tiraFoto();
    takeNewPhoto = false;
  }
  delay(1);

  //Ao final de todo loop ele libera o framebuffer da ultima foto tirada
  if(last_photo)
    esp_camera_fb_return(last_photo);

}