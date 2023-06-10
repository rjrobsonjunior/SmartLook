#include <Keypad.h>
#include <Arduino.h>

#include <WiFi.h>
#include <HTTPClient.h>

#include <display.h>

#define RELE_PIN 4
#define BUZZER_PIN 5
#define NOTA_BUZZER 528
#define RECPTOR_PIN 15
#define MAG_PIN 23

// Configurações do WiFi
const char* ssid = "GalaxyS";
const char* password_wifi = "utfpr1234";

//Configuração IP ESP32
IPAddress local_IP(192, 168, 0, 10); // Endereço IP do ESP32
IPAddress gateway(192, 168, 144, 139);   // Endereço do gateway
IPAddress subnet(255, 255, 255, 0);  // Máscara de sub-rede

// Configurações do teclado matricial
const byte ROWS = 4; // Número de linhas do teclado matricial
const byte COLS = 4; // Número de colunas do teclado matricial

char keys[ROWS][COLS] = { 
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {13, 12, 14, 27}; // Pinos do ESP32 conectados às linhas do teclado matricial
byte colPins[COLS] = {26, 25, 33, 32}; // Pinos do ESP32 conectados às colunas do teclado matricial

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

//Inicializa display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Definir as constantes de conexão com o servidor
String serverName = "https://fechaduraeletronica.000webhostapp.com/post.php";

// Variáveis para armazenar o nome de usuário e a senha
String login = "123";
String senha = "12345";

void startPin()
{
  pinMode(RELE_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RECPTOR_PIN, INPUT);
  pinMode(MAG_PIN, INPUT_PULLUP);
}

void connect_wifi()
{
  
  WiFi.begin(ssid, password_wifi);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.print(".");

  }

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

String getPassword() 
{
  
  String password = "";
  char key;

  display.clearDisplay();

  while(password.length() < 5) {
    
    display.setCursor(0, 0);
    display.println("Registro de Senha");
    display.display();

    key = keypad.getKey();

    display.setCursor(0, (display.height() - 8) / 2);

    if(key != NO_KEY) {
      password += key;
      
      //Impressão no monitor serial e display OLED
      Serial.print(key);       
      display.print(password);
      display.display();

      delay(200); // aguarda um pouco para a próxima tecla ser pressionada
    }
  }

  Serial.println("\nSenha cadastrada com sucesso!");

  display.clearDisplay();
  display.setCursor(0, (display.height() - 8) / 2);
  display.println("Senha cadastrada!");
  display.display();

  delay(1000);
  
  return password;
}

String getLogin() 
{

  String login_f = "";
  char key;

  display.clearDisplay();

  while(login_f.length() < 3) {
    
    display.setCursor(0, 0);
    display.println("Registro de Login");
    display.display();

    key = keypad.getKey();
    
    display.setCursor(0, (display.height() - 8) / 2);

    if(key != NO_KEY) {
      login_f += key;
      
      Serial.print(key);  
      display.print(login_f);
      display.display();

      delay(200); // aguarda um pouco para a próxima tecla ser pressionada
    }
  }

  Serial.println("\nLogin cadastrado com sucesso!");
  
  display.clearDisplay();
  display.setCursor(0, (display.height() - 8) / 2);
  display.println("Senha cadastrada!");
  display.display();
  delay(1000);

  return login_f;
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
  
  tone(BUZZER_PIN, NOTA_BUZZER, 500); 
  
  //ABRIR RELE
  digitalWrite(RELE_PIN, HIGH);

  //flags para contagem de pessoas
  bool saida = false;
  bool estado = false;
  bool estado_porta = digitalRead(MAG_PIN);


  //MODO A PARTIR DO TIME
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
    else if(saida == false && estado==true)
    {
      estado = false;
    }

    estado_porta = digitalRead(MAG_PIN);
  }

  //Quando a porta fecha -> Rele desativado
  delay(500);
  digitalWrite(RELE_PIN, LOW);

  //Envio da quantidade de pessooas ao dashboard web
  Serial.println(pessoas);
  

}

void setup() {
  
  Serial.begin(9600);
  startPin();
  start_display(display);

  connect_wifi();
  //ip_esp();


}

void loop() 
{
  display_home(display);

  char key = keypad.getKey();

  //Primeira tecla pressionada, se for diferente dessas, ele vai registrar o login
  if (key == 'A') {
    Serial.println("--- Registro de Login ---");

    login = getLogin();

    Serial.println("\n");
  }

  if (key == 'B') {
    Serial.println("--- Registro de Senha ---");
  
    senha = getPassword();
    
    Serial.println("\n");
  }

  if(key == 'C')
  {
    Serial.println("--- Credenciais atuais ---");

    Serial.println("Login = " + login);
    Serial.println("Senha = " + senha);
    Serial.println("\n");

    //Impressão display

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Credenciais atuais");
    display.setCursor(0, 16);
    display.println("Login = " + login);
    display.println("Senha = " + senha);
    display.display();
    delay(2500);

  }


  if(key == 'D')
  {
    Serial.println("--- Limpando Credenciais ---");

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Limpando Credenciais...");
    display.display();
    delay(1000);

    login = "";
    senha = "";
  }

  //Se precionar essa tecla ele realiza a consulta no banco de dados atraves de uma requisao html
  if (key == '*') {
    Serial.println("--- Consulta de Acesso ---");

    // Crie um objeto HTTPClient
    HTTPClient client;
    
    if(login != "" && senha != ""){

      // Defina a URL e o conteúdo da requisição POST
      client.begin(serverName);
      client.addHeader("Content-Type", "application/x-www-form-urlencoded");
      String postData = "login=" + login + "&senha=" + senha;

      // Envie a requisição POST e imprima a resposta
      int httpResponseCode = client.POST(postData);

      if (httpResponseCode == 200) 
      {
        String response = client.getString();

        if (response == "true") {
          Serial.println("Acesso liberado!");

            display_acesso_liberado(display);
            //abrir_fechadura();
        } 
          
        else {
          Serial.println("Senha incorreta!");
          display_acesso_negado(display);
        }

      } 

      else 
      {
        Serial.println("Erro na conexão com o servidor!");
        Serial.println("HttpCode = " + httpResponseCode);
      }

      client.end();
    }
    else
    {
      Serial.println("Não há informações para o acesso");

      //DISPLAY
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Nao ha informacoes para o acesso!");
      display.display(); 
      delay(3000);

    }
  }
  
  if(key == '#')
  {
    Serial.println("Abrindo fechadura...");
    abrir_fechadura();
  }
  
}






