#include <Keypad.h>
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define RELE_PIN 4
#define BUZZER_PIN 5
#define NOTA_BUZZER 528
#define RECPTOR_PIN 15

// Configurações do WiFi
const char* ssid = "pco";
const char* password_wifi = "junior1521";

//Configuração IP ESP32
IPAddress local_IP(192, 168, 184, 200); // Endereço IP do ESP32
IPAddress gateway(192, 168, 184, 17);   // Endereço do gateway
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

//Configuração Display
#define SCREEN_WIDTH 128 // Largura do display OLED em pixels
#define SCREEN_HEIGHT 32 // Altura do display OLED em pixels
#define OLED_RESET -1 // Pin reset do display OLED (ou -1 se não tiver)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Array de imagem do emoji de fechadura
const unsigned char lockIcon[128] PROGMEM = {
  // Dados da imagem aqui
  0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xe3, 0xff, 0x00, 0x00, 0xe1, 0xff, 0x00, 
0x00, 0xc1, 0xff, 0x00, 0x00, 0xc0, 0x03, 0x00, 0x00, 0xc1, 0xff, 0x00, 0x00, 0xc1, 0xff, 0x00, 
0x00, 0xc1, 0xff, 0x00, 0x00, 0xc1, 0xff, 0x00, 0x00, 0xc1, 0xff, 0x00, 0x00, 0xc1, 0xff, 0x00, 
0x00, 0xc1, 0xff, 0x00, 0x00, 0xe3, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00
};

// -------------------------------------------------------------------------------------------------

// Definir as constantes de conexão com o servidor
const char* serverName = "http://34.151.239.205/?login=";

// Variáveis para armazenar o nome de usuário e a senha
String login = "123";
String senha = "12365";

void connect_wifi()
{
  // Conecta-se à rede WiFi
  WiFi.begin(ssid, password_wifi);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.println("Conectando-se à rede WiFi...");

  }

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

void start_display()
{
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) 
  { // Endereço 0x3D para 128x64
    Serial.println(F("Falha na alocação SSD1306"));
    for(;;);
  }
  delay(500);
  display.clearDisplay();
  
  // CONFIGURAÇÃO TEXTO
  display.setTextSize(1);
  display.setFont();
  display.setTextColor(WHITE);

  display.display();
}

String getPassword() {
  
  String password = "";
  char key;

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

String getLogin() {

  String login_f = "";
  char key;

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

void display_acesso_liberado()
{
  display.clearDisplay();
 
  //display.setCursor(0,20); // Posição do cursor para o texto
  display.setCursor(0, (display.height() - 8) / 2); // Define a posição do cursor no meio do display

  display.println("Acesso liberado!"); // Texto a ser exibido
  display.display();  
  
  delay(3000);

  display.clearDisplay();
  display.display();
}

void display_acesso_negado()
{
  display.clearDisplay();
  
  //display.setCursor(0,20); // Posição do cursor para o texto
  display.setCursor(0, (display.height() - 8) / 2); // Define a posição do cursor no meio do display

  display.println("Acesso negado!"); // Texto a ser exibido
  display.display();  
  
  delay(3000);

  display.clearDisplay();
  display.display();
}

void display_home()
{
  display.clearDisplay();

  //display.setCursor(0,20); // Posição do cursor para o texto
  display.setCursor(0, (display.height() - 8) / 2); // Define a posição do cursor no meio do display

  display.println("Fechadura Eletronica"); // Texto a ser exibido
  display.display();  

  // Exibe o emoji de fechadura
  //display.drawBitmap(0, (display.height() - 8) / 2, lockIcon, 32, 16, 1);
  //display.display();

}

int contagem_pessoas()
{
  int leitura = digitalRead(RECPTOR_PIN);
  if(leitura == 1)
  {
    return 1;
  }
  else
    return 0;
}

void abrir_fechadura()
{

  //Futuramente, adcionar -> Quando o sensor eletromagnético saber que a porta fechou, desliga ele
  
  int tempo_atual = millis();
  int pessoas = 0;
  
  tone(BUZZER_PIN, NOTA_BUZZER, 500); 

  //Acionamento do Rele e Buzzer (por 5segundos)

  while(tempo_atual + 5000 > millis()){
    digitalWrite(RELE_PIN, HIGH); 
    pessoas += contagem_pessoas();
  }
  

  //Rele desativado
  digitalWrite(RELE_PIN, LOW);


  //Envio da quantidade de pessooas ao dashboard web

}


void setup() {
  
  Serial.begin(9600);

  connect_wifi();
  
  ip_esp();

  start_display();
  
  pinMode(RELE_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RECPTOR_PIN, INPUT);


}

void loop() 
{
  display_home();

  char key = keypad.getKey();

  //Primeira tecla pressionada, se for diferente dessas, ele vai registrar o login
  if (key == 'A') {
    Serial.println("--- Registro de Login ---");

    display.clearDisplay();

    login = getLogin();
    Serial.println("\n");
  }

  if (key == 'B') {
    Serial.println("--- Registro de Senha ---");
    
    //Para poder aparecer a senha na tela oled
    display.clearDisplay();
    
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
    delay(2000);

  }


  if(key == 'D')
  {
    Serial.println("--- Limpando Credenciais ---");

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Limpando Credenciais...");
    display.display();
    delay(2000);

    login = "";
    senha = "";
  }

  //Se precionar essa tecla ele realiza a consulta no banco de dados atraves de uma requisao html
  if (key == '*') {
    Serial.println("--- Consulta de Acesso ---");

    String serverPath = serverName;

    if(login != "" && senha != ""){

      serverPath += login;

      Serial.println("url: " + serverPath);

      HTTPClient client;

      if (client.begin(serverPath)) 
      {
        //Verificar se a requisição foi bem-sucedida
        int httpCode = client.GET();
        Serial.println(httpCode);
        if (httpCode == 200) 
        {
          String resposta = client.getString();
          
          if (resposta == senha) {
            Serial.println("Acesso liberado!");

            display_acesso_liberado();
            abrir_fechadura();
          } 
          
          else {
            Serial.println("Senha incorreta!");
            display_acesso_negado();
          }

        } 

        else 
        {
          Serial.println("Erro na conexão com o servidor!");
        }

        client.end();
      }

      else {
        Serial.println("Não foi possível conectar com o servidor!");
      }
    }
    else
    {
      Serial.println("Não há informações para o acesso");
      //abrir_fechadura();

      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Nao ha informaçoes para o acesso!"); // Texto a ser exibido
      display.display(); 
      delay(3000);

    }
  }
  
}






