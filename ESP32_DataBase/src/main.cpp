#include <includes.h>

void startPin()
{
  pinMode(RELE_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RECPTOR_PIN, INPUT);
  pinMode(MAG_PIN, INPUT_PULLUP);
  pinMode(FECHADURA_PIN, INPUT_PULLUP);
}

void connect_wifi()
{
  
  WiFi.begin(ssid, password_wifi);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.print(".");

  }

  Serial.println("");

  // Imprime os dados de conexão
  Serial.println(WiFi.SSID());
  Serial.print("Endereço IP: http://");
  Serial.println(WiFi.localIP());
  Serial.print("Endereço MAC: ");
  Serial.println(WiFi.macAddress());

  Serial.println("Conectado à rede WiFi");
}

void ip_esp()
{
  // Configurar o IP estático
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("Falha ao configurar o IP estático!");
  }
  else {
    Serial.println("Configuração de IP estático concluída!");
  }

}

/* DISPLAY */
void setCursorMeio()
{
    display.setCursor(0, (display.height() - 8) / 2); // Define a posição do cursor no meio do display

}

void start_display()
{
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) 
    { 
        // Endereço 0x3D para 128x64
        Serial.println(F("Falha na alocação SSD1306"));
        for(;;);
    }
    delay(500);
    display.clearDisplay();
    
    // CONFIGURAÇÃO TEXTO
    display.setTextSize(1);
    display.setFont();
    display.setTextColor(WHITE);

    //Mensagem de boas vindas
    setCursorMeio();
    display.println("Fechadura Eletrônica");
    display.display();

    delay(500);
    display.setCursor(0, 24);
    display.println("Bem vindo...");
    display.display();

    delay(1500);


}

void display_acesso_liberado(String usuario)
{
    display.clearDisplay();
    setCursorMeio();

    display.println("Acesso liberado!"); // Texto a ser exibido
    display.display(); 
    delay(500);

    display.clearDisplay();
    display.println("Bem-vindo " + usuario); // Texto a ser exibido
    display.display(); 
    
    
    delay(3000);

}

void display_acesso_liberado()
{
    display.clearDisplay();
    setCursorMeio();

    display.println("Acesso liberado!"); // Texto a ser exibido
    display.display(); 
    
    delay(3000);

}

void display_acesso_negado()
{
    display.clearDisplay();
  
    setCursorMeio();
    display.println("Acesso negado!"); // Texto a ser exibido
    display.display();  
    
    delay(3000);
}

void display_home()
{
    display.clearDisplay();

    display.setCursor(0, 0);
    display.println("Menu de Selecao");

    display.setCursor(0, 8);
    display.println("A - Login e Senha");
    display.println("B - R. Facial");
    display.println("C - QR Code");
    display.display();  

    // Exibe o emoji de fechadura
    //display.drawBitmap(0, (display.height() - 8) / 2, lockIcon, 32, 16, 1);
    //display.display();
}

void credenciaisLogin()
{

  String login_f = "";
  String senha_f = "";
  
  char key;

  Serial.print("Login = ");
  while(login_f.length() < 3) {

    key = keypad.getKey();
    
    display.setCursor(0, 8);
    display.print("Login = ");
    display.display();

    

    if(key != NO_KEY) {
      login_f += key;
      
      Serial.print(key);  
      display.print(login_f);
      display.display();

      delay(100); // aguarda um pouco para a próxima tecla ser pressionada
    }
  }

  Serial.print("\nSenha = ");

  while(senha_f.length() < 5) {

    key = keypad.getKey();
    
    display.setCursor(0, 16);
    display.print("Senha = ");
    display.display();

    if(key != NO_KEY) {
      senha_f += key;
      
      Serial.print(key);  
      display.print(senha_f);
      display.display();

      delay(100); // aguarda um pouco para a próxima tecla ser pressionada
    }
  }

  login = login_f;
  senha = senha_f;

  delay(500);

}

//Faz a checagem se o login existe no Banco de Dados
String checarLoginDB()
{
  Serial.println("--- Consulta de Acesso ---");

  // Crie um objeto HTTPClient
  HTTPClient client;
  client.begin(url_analiseLogin);
  /*
  client.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String postData = "login=" + login + "&senha=" + senha;
  */
  client.addHeader("Content-Type", "application/json");

  // Crie um objeto JSON com os dados de login e senha
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["login"] = login;
  jsonDoc["senha"] = senha;

  // Converta o objeto JSON para uma string
  String jsonData;
  serializeJson(jsonDoc, jsonData);

  int httpResponseCode = client.POST(jsonData);
  String response = client.getString();

  if (httpResponseCode == 200) 
  {
    Serial.println("checarLoginDB | Usuario encontrado!");
  } 
  else if(httpResponseCode == 400)
  {
    Serial.println("checarLoginDB | Senha invalida!");
    response = "false";
  }

  else 
  {
    Serial.println("checarLoginDB | Erro na conexão com o servidor!");
    Serial.println("HttpCode = " + httpResponseCode);
    response = "false";
  }

  return response;
  client.end();
    
}

//Faz a checagem se o login existe no Banco de Dados
bool checarCredenciaisSaida()
{
  Serial.println("--- Consulta de Acesso ---");

  // Crie um objeto HTTPClient
  HTTPClient client;
  client.begin(url_analiseCredenciaisSaida);
  client.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String postData = "login=" + login + "&senha=" + senha;
  int httpResponseCode = client.POST(postData);

  if (httpResponseCode == 200) 
  { 
    Serial.println("Usario deletado da lista de presentes!");
    return true;
  } 

  else 
  {
    Serial.println("checarCredenciaisSaida | Erro na conexão com o servidor!");
    Serial.println("HttpCode = " + httpResponseCode);
  }

  return false;
  client.end();
    
}

bool checarFaceDB()
{
  // Conectar ao servidor
  WiFiClient client;
  
  if (!client.connect(ip_espCAM, 80)) 
  {
    Serial.println("Falha na conexão com o servidor");
    return false;
  }

  Serial.println("Mandando o espcam analisar a foto...");

  // Enviar a requisição GET
  client.print("GET /reconhecimentoFacial HTTP/1.1\r\n");
  client.print("Host: 192.168.1.11\r\n");
  client.print("Connection: close\r\n\r\n");

  // Aguardar a resposta do servidor
  while (client.connected() && !client.available()) {
    delay(1);
  }

  // Ler a resposta do servidor
  String resposta = "";

  while (client.available()) {
    resposta = client.readStringUntil('\n');
    Serial.println("Resposta completa do servidor:");
    Serial.println(client.readString());
  }

  // Extrair a última linha da resposta 
  int lastNewlinePos = response.lastIndexOf('\n');
  String lastLine = response.substring(lastNewlinePos + 1);

  // Analisar a resposta do servidor
  if (resposta.startsWith("HTTP/1.1 200 OK")) 
  {
    client.stop();
    // Face reconhecida
    Serial.println("Face reconhecida!");
    nome_usuario = lastLine;
    return true;
  } 
   
  else 
  {
    // Outro problema
    Serial.println("Outro problema");
    Serial.println("Erro: " + lastLine);
  }

  // Fechar a conexão
  client.stop();
  return false;
}

bool checarQrCodeDB()
{
  // Conectar ao servidor
  WiFiClient client;
  
  if (!client.connect("192.168.1.6", 8800)) 
  {
    Serial.println("Falha na conexão com o servidor");
    return false;
  }

  // Enviar a requisição GET
  client.print("GET /analisaQR HTTP/1.1\r\n");
  client.print("Host: 192.168.1.6\r\n");
  client.print("Connection: close\r\n\r\n");

  // Aguardar a resposta do servidor
  while (client.connected() && !client.available()) {
    delay(1);
  }

  // Ler a resposta do servidor
  String resposta = "";

  while (client.available()) {
    resposta = client.readStringUntil('\n');
    Serial.println("Resposta completa do servidor:");
    Serial.println(client.readString());
  }

  // Extrair a última linha da resposta 
  int lastNewlinePos = response.lastIndexOf('\n');
  String lastLine = response.substring(lastNewlinePos + 1);

  // Analisar a resposta do servidor
  if (resposta.startsWith("HTTP/1.1 200 OK")) 
  {
    client.stop();
    // Face reconhecida
    Serial.println("Qr Code reconhecido!");
    nome_usuario = lastLine;
    return true;
  } 
   
  else 
  {
    // Outro problema
    Serial.println("Usuario nao encontrado");
  }

  // Fechar a conexão
  client.stop();
  return false;
}

//Envia uma instrução para o ESPCAM tirar a foto
void tirarFoto()
{
  //GET - WebServer do ESPCAM
  Serial.println("URL = " + url_tirarFoto);
  
  HTTPClient http;
  http.begin(url_tirarFoto);
  //delay(3000);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    Serial.println("Requisição enviada com sucesso. Foto tirada!");
  } else {
    Serial.printf("Erro ao enviar a requisição: %d\n", httpCode);
  }

  http.end();
}

int contagem_pessoas()
{
  int leitura = digitalRead(RECPTOR_PIN);

  //LEITURA = 1 -> LASER NO RECEPTOR | LEITURA == 0 -> LASER NAO TA CHEGANDO NO RECEPTOR (PASSANDO GENTE)

  if(leitura == 1)
  {
    return 0;
  }
  else{
    return 1;
  }
}

void abrir_fechadura()
{

  //SENSOR MAGNÉTICO: 0 - FECHADO(SENSOR SE ENCONSTANDO) | 1 - ABERTO
  
  int tempo_atual = millis();
  int pessoas = 0;
  
  //tone(BUZZER_PIN, NOTA_BUZZER, 500); 
  
  //ABRIR RELE
  digitalWrite(RELE_PIN, HIGH);

  //flags para contagem de pessoas
  bool saida = false;
  bool estado = false;
  bool estado_porta = digitalRead(MAG_PIN);

  /*
  while(tempo_atual + 5000 > millis())
  {

    digitalWrite(RELE_PIN, HIGH); 
    
    saida = contagem_pessoas();

    if(saida == true && estado == false)
    {
      estado = true;
      pessoas++;
    }
    else if(saida == false && estado==true)
    {
      estado = false;
    }

    bool mag = digitalRead(MAG_PIN);

    Serial.println(mag);

  }*/

  //ENQUANTO A PORTA NAO É ABERTA  
  while(estado_porta == false)
  {
    estado_porta = digitalRead(MAG_PIN);
  }

  //MODO A PARTIR DO SENSOR MAGNETICO
  while(estado_porta == true)
  {   
    saida = contagem_pessoas();

    if(saida == true && estado == false)
    {
      estado = true;
      pessoas++;
    }
    else if(saida == false && estado == true)
    {
      estado = false;
    }

    estado_porta = digitalRead(MAG_PIN);
  }

  //Quando a porta fecha -> Rele desativado
  delay(500);
  digitalWrite(RELE_PIN, LOW);

  //Envio da quantidade de pessooas ao dashboard web
  Serial.println(pessoas + "pessoas entraram no ambiente!");
  

}

void setup() {
  
  Serial.begin(115200);
  startPin();
  start_display();

  connect_wifi();
  //ip_esp();


}

void loop() 
{
  display_home();

  char key = keypad.getKey();

  if (key == 'A') {
    Serial.println("--- Login e Senha ---");

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("--- Login e Senha ---");
    display.display();

    //Registrando o login e senha
    credenciaisLogin();

    //Cadastro Finalizado
    Serial.println("\nCadastro finalizado com sucesso!");
  
    display.clearDisplay();
    display.setCursor(0, (display.height() - 8) / 2);
    display.println("Credenciais digitadas!");
    display.display();
    delay(1000);

    //Manda para a base de dados
    String respostaConsulta = checarLoginDB();

    if(respostaConsulta != "false")
    {
      Serial.println("Acesso Liberado! Bem vindo " + respostaConsulta);
      display_acesso_liberado(respostaConsulta);
      abrir_fechadura();
    }

    else
    {
      Serial.println("Acesso Negado!");
      display_acesso_negado();
    }

  }

  //Reconhecimento Facial
  if (key == 'B') 
  {
    Serial.println("--- Reconhecimento Facial ---");

    display.clearDisplay();
    display.setCursor(0,0);
    display.println("--- R. facial ---");
    display.display();

    //Mando o servidor analisar a foto
    if(checarFaceDB())
    {
      Serial.println("Acesso Liberado! Bem vindo " + nome_usuario);
      display_acesso_liberado(nome_usuario);
      abrir_fechadura();

    }
    else
    {
      Serial.println("Acesso Negado!");
      display_acesso_negado();
    }
    
    
  }

  //Qr Code
  if(key == 'C')
  {
    Serial.println("--- QR CODE ---");

    display.clearDisplay();
    display.setCursor(0,0);
    display.println("--- QR Code ---");
    display.display();


    //Mando o servidor analisar a foto
    if(checarQrCodeDB())
    {
      Serial.println("Acesso Liberado! Bem vindo " + nome_usuario);
      display_acesso_liberado(nome_usuario);
      abrir_fechadura();

    }
    else
    {
      Serial.println("Acesso Negado!");
      display_acesso_negado();
    }
    

  }

  if(key == 'D')
  {
    Serial.println("--- Tirando foto ---");

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Tirando foto...");
    display.display();
    delay(1000);
  }

  if (key == '*') {
    //Pessoa apertou o botao para sair
    Serial.println("--- Saida ---");

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("--- Saida ---");
    display.display();
    delay(500);

    credenciaisLogin();

    display.setCursor(0, (display.height() - 8) / 2);

    //Manda para a base de dados
    if(checarCredenciaisSaida())
    {
      Serial.println("Saida Liberada! Obrigado pela visita!");
      display.clearDisplay();
      display.println("Saida Liberada!");
      display.display();
      delay(500);

      //Enviar rota para o servidor tirar o usuario da lista de presentes

    }

    else
    {
      Serial.println("Usuario não reconhecido!");
      display.clearDisplay();
      display.println("Usuario não reconhecido!");
      display.display();
      delay(500);
    }

    display.clearDisplay();
    display.println("Obrigado pela visita!");
    display.display();
    delay(1000);

  }
  
  
  if(digitalRead(FECHADURA_PIN) == LOW)
  {
    //Pessoa apertou o botao para sair
    Serial.println("--- Saida ---");

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("--- Saida ---");
    display.display();
    delay(500);

    credenciaisLogin();

    display.setCursor(0, (display.height() - 8) / 2);

    //Manda para a base de dados
    if(checarCredenciaisSaida())
    {
      Serial.println("Saida Liberada! Obrigado pela visita!");
      display.clearDisplay();
      display.println("Saida Liberada!");
      display.display();
      delay(500);

      //Enviar rota para o servidor tirar o usuario da lista de presentes

    }

    else
    {
      Serial.println("Usuario não reconhecido!");
      display.clearDisplay();
      display.println("Usuario não reconhecido!");
      display.display();
      delay(500);
    }

    display.clearDisplay();
    display.println("Obrigado pela visita!");
    display.display();
    delay(1000);
  }
  
  
}






