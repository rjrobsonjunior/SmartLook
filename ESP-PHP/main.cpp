#include <Keypad.h>
#include <Arduino.h>
#include <WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <HTTPClient.h>

// Configurações do WiFi
const char* ssid = "Adryan e Enzo";
const char* password_wifi = "sanvscor";

/*
// Configurações do banco de dados MySQL
IPAddress server_addr(127, 0, 0, 1); // Endereço IP do servidor MySQL
int port = 3306; // Porta do servidor MySQL
char* user_db = "user_test";
char* pass_db = "12345";
char* db = "test2";
*/

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

// Definir as constantes de conexão com o servidor
const char* serverName = "http://seu_servidor.com/testmysql.php";

// Variáveis para armazenar o nome de usuário e a senha
String login = "";
String senha = "";

/*
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
    const char* query_char = query.c_str();
    cur->execute(query_char);
    
    /*
    //Lendo os resultados da consulta
    MySQL_Cursor row;
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
  } 
  else {
    Serial.println("Falha na conexão ao MySQL!");
    return false;
  }
  */


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
/*
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
*/

String getPassword() {
  String password = "";
  for(int i = 0; i < 5; i++) {
    char key = keypad.getKey();
    if(key != NO_KEY) {
      password += key;
      Serial.print("*"); // mostra asterisco no LCD ao digitar a senha
      delay(100); // aguarda um pouco para a próxima tecla ser pressionada
    }
  }
  return password;
}

String getLogin() {
  String login_f = "";
  for(int i = 0; i < 3; i++) {
    char key = keypad.getKey();
    if(key != NO_KEY) {
      login_f += key;
      Serial.print(login_f);
      delay(100); // aguarda um pouco para a próxima tecla ser pressionada
    }
  }
  return login_f;
}

void setup() {
  
  Serial.begin(9600);

  connect_wifi();

  // Definir o tempo limite de espera para a requisição HTTP
  HTTPClient http;
  http.setTimeout(10000);

}

void loop() {

  char key = keypad.getKey();
  if (key) {

    //Primeira tecla pressionada, se for diferente dessas, ele vai registrar o login
    if (key != '#' && key != '*') {
      Serial.print("Digite o login: ");
      login = getLogin();
    }

    //Se for pressionado esta tecla ele registra a senha
    if (key == '#') {
      Serial.print("Digite sua senha: ");
      senha = getPassword();
    }


    //Se precionar essa tecla ele realiza a consulta no banco de dados atraves de uma requisao html
    if (key == '*') {
      /* Funcionamento: O login é mandando via http para o servidor, e esse servidor consulta o login no banco de dados e retorna a senha. Se a senha for igaul ao que o usuario digitou, ele libera a entrada.*/
      Serial.println();
      Serial.print("Login: ");
      Serial.println(login);
      Serial.print("Senha: ");
      Serial.println(senha);

      String serverPath = serverName;

      serverPath += login;
      Serial.print("url: " + serverPath);

      HTTPClient client;
      if (client.begin(serverPath)) 
      {
        //Verificar se a requisição foi bem-sucedida
        int httpCode = client.GET();
        if (httpCode == 200) {
          String resposta = client.getString();
          if (resposta == senha) {
            Serial.println("Acesso liberado!");
          } else {
            Serial.println("Senha incorreta!");
          }
        } 

        else {
          Serial.println("Erro na conexão com o servidor!");
        }

        client.end();
      }

      else {
        Serial.println("Não foi possível conectar com o servidor!");
      }

      login = "";
      senha = "";
    }
  }
}






