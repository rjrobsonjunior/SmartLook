#include <Keypad.h>
#include <Arduino.h>
#include <WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

// Configurações do WiFi
const char* ssid = "Adryan e Enzo";
const char* password_wifi = "sanvscor";

// Configurações do banco de dados MySQL
IPAddress server_addr(127, 0, 0, 1); // Endereço IP do servidor MySQL
int port = 3306; // Porta do servidor MySQL
char* user_db = "user_test";
char* pass_db = "12345";
char* db = "test2";

// Configurações do teclado matricial

const byte ROWS = 4; // Número de linhas do teclado matricial
const byte COLS = 4; // Número de colunas do teclado matricial
char keys[ROWS][COLS] = { // Mapeamento das teclas do teclado matricial
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {5, 4, 3, 2}; // Pinos do ESP32 conectados às linhas do teclado matricial
byte colPins[COLS] = {9, 8, 7, 6}; // Pinos do ESP32 conectados às colunas do teclado matricial

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Criar o objeto WiFiClient
WiFiClient client;

// Criar o objeto MySQL_Connection
MySQL_Connection conn((Client *)&client);

// Variáveis para armazenar o nome de usuário e a senha
String login = "";
String senha = "";


//Definindo a função que faz a consulta ao banco de dados
bool checkLogin(String login, String senha) {

  if (conn.connect(server_addr, 3306, user_db, pass_db, db)) 
  {
    Serial.println("Conexão ao MySQL realizada com sucesso!");
    
    //Preparando a query SQL
    String query = "SELECT * FROM pessoas WHERE login = '"+login+"' AND senha = '"+senha+"'";
    Serial.println("Query: " + query);
    
    //Executando a query SQL
    MySQL_Cursor *cur = new MySQL_Cursor(&conn);
    cur->execute(query);
    
    //Lendo os resultados da consulta
    MySQL_Row row;
    row = cur->get_next_row();
    
    if (row.empty()) {
      Serial.println("Login ou senha inválidos!");
      return false;
    } else {
      Serial.println("Login e senha válidos!");
      return true;
    }
    
    delete cur;
  } else {
    Serial.println("Falha na conexão ao MySQL!");
    return false;
  }
}

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

void connect_db()
{
  // Conectar-se ao servidor MySQL
  if (conn.connect(server_addr, port, user_db, pass_db, db)) {
    Serial.println("Conectado ao servidor MySQL");
  
  }

  else {
    Serial.println("Falha na conexão com o servidor MySQL");

    while (true){
      delay(1000);
    }
  }
}

void setup() {
  
  Serial.begin(9600);

  connect_wifi();

  connect_db();
}

void loop() {

  // Ler as entradas do teclado matricial
  char key = keypad.getKey();

  if (key){

    //Verificação de autenticação
    if (key == '#') {
      if(checkLogin(login, senha))
      {
        Serial.println("Usuario Autenticado!");
      }
      else{
        Serial.println("Usuario não encontrado!");
      }

      login = "";
      senha = "";

    } 

    else if(key == '*')
    {
      //Limpa o campo de login e senha
      login = "";
      senha = "";
    } 

    else{

      //Preenche os campos de autenticação

      if (login.length() < 5 && isDigit(key)) {
        login += key;
      }

      if (senha.length() < 5 && !isDigit(key)) {
        senha += key;
      }

    }
  }

}






