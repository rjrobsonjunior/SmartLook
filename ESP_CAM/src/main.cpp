#include "includes.h"

String messages = "";
String serverESP = "";

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
    xhr.open('GET', "/reconhecimentoFacial", true);
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
  Serial.print("Endereço MAC: ");
  Serial.println(WiFi.macAddress()); 
  
  
}

//Nessa função eu tiro a foto e mando para a aplicação web. 
bool tirarFotoServidor()
{
  //A aplicação pega a foto e exibe, se o usuario quiser ele salva a foto e manda para a analise, caso nao, ele manda tirar outra foto
  String resposta = "";
  bool foto_foi_tirada = false;
  int cont = 0;
  camera_fb_t* foto_a = NULL;

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
    
    delay(1000);

    foto_a = esp_camera_fb_get();
  
    if(!foto_a)
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
  requestBody += String((char*)foto_a->buf, foto_a->len) + "\r\n";
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
  esp_camera_fb_return(foto_a);
  foto_a = NULL;

  delay(1000);

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
  connectWiFi();

  //Permite o acesso de clientes de diferentes origens a recursos específicos ao servidor
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

  servidorWeb();

}

void loop() {
  
  delay(1);

}