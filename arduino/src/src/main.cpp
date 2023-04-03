#include <Arduino.h>
#include <Wire.h> 
#include <Adafruit_GFX.h> 
#include <Adafruit_SSD1306.h> 
#include <Keypad.h>
#include <WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <string.h>


#define ROW_NUM     4 // four rows
#define COLUMN_NUM 4 // three columns
#define PinoRele 4

//
char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte pin_rows[ROW_NUM] = {32, 33, 25, 26}; // GIOP18, GIOP5, GIOP17, GIOP16 connect to the row pins
byte pin_column[COLUMN_NUM] = {27, 14, 12, 13};  // GIOP4, GIOP0, GIOP2 connect to the column pins

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

Adafruit_SSD1306 display = Adafruit_SSD1306(); 

// Configurações do WiFi
/*
const char* ssid = "Adryan e Enzo";
const char* password_wifi = "sanvscor";
*/
const char* ssid = "VIVOFOFIBRA-B1F1";
const char* password_wifi = "722323B1F1";

// Configurações do banco de dados MySQL

/*
IPAddress server_addr(127, 0, 0, 1); // Endereço IP do servidor MySQL
int port = 3306; // Porta do servidor MySQL
char* user_db = "user_test";
char* pass_db = "12345";
char* db = "test2";
*/
IPAddress server_addr(127, 0, 0, 1); // Endereço IP do servidor MySQL
int port = 3306; // Porta do servidor MySQL
char* user_db = "root";    
char* pass_db = "OficinadeIntegeracao1";
char* db = "testeOF1";


// Criar o objeto WiFiClient
WiFiClient client;

// Criar o objeto MySQL_Connection
MySQL_Connection conn((Client *)&client);

// Variáveis para armazenar o nome de usuário e a senha
String login = "";
String senha = "";
char option = '0';

//bool checkLogin(String login, String senha);

void connect_wifi();

void connect_db();

void identificationLogin();

void identificationFacial();

void identificationQRcode();

void setup(){

  Serial.begin(9600);

  //rele
  pinMode(PinoRele, OUTPUT);
  digitalWrite(PinoRele, LOW);

  //display
  Wire.begin(); 
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE); 
  display.setTextSize(2); 
  display.clearDisplay(); 

  //wifi
  connect_wifi();

  //database
  connect_db();
  
}
void loop() {

  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  char sql_query[100];
  sprintf(sql_query, "SELECT login, senha FROM testeOF1.info");
  cur_mem->execute(sql_query);
  MySQL_Row row = cur_mem->get_next_row();
  
  // Imprime as informações de login e senha no monitor serial
  Serial.print("Login: ");
  Serial.println(row.get_string(0));
  Serial.print("Senha: ");
  Serial.println(row.get_string(1));
  
  display.clearDisplay(); 

  delay(10000);
  
  display.setTextSize(2);
  display.setCursor(0,0); 
  display.print("Selected identification mode");
  display.display();
  delay(500);
  display.print(" 1- login and password 2");
  delay(500);
  display.display();
  display.print(" 2- facial recognition");
  delay(500);
  display.display();
  display.print(" 3- QR Code");
  delay(500);
  display.display();
  //select identification mode
  char option = keypad.getKey();
  
  switch (option)
  {
  //login and password
  case '1':
    /* code */
    identificationLogin();
    break;
  //facial recognition
  case '2':
    /* code */
    identificationFacial();
    break;
  //QR Code
  case '3':
    /* code */
    identificationQRcode();
    break;
  default:
    break;
  }
  
  display.clearDisplay(); 

  delay(10000);
}
//Functions

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

void identificationLogin()
{
  char login[11], senha[4];
  display.setTextSize(2);
  display.setCursor(0,0); 
  display.print("Enter the login: ");
  display.display();
  for(int i = 0; i < 11; i++)
  {
    login[i] = keypad.getKey();
    display.setTextSize(2);
    display.setCursor(0,0); 
    display.print(login);
    display.display();
  }
  for(int i = 0; i < 4; i++)
  {
    senha[i] = keypad.getKey();
    login[i] = keypad.getKey();
    display.setTextSize(2);
    display.setCursor(0,0); 
    display.print(senha);
    display.display();
  }
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
      display.setTextSize(2);
      display.setCursor(0,0); 
      display.print("Login ou senha inválidos!");
      display.display();
      digitalWrite(PinoRele, LOW);
      delay(1000);
    } else {
      display.setTextSize(2);
      display.setCursor(0,0); 
      display.print("Login ou senha válidos!");
      display.display();
      digitalWrite(PinoRele, HIGH);
      delay(5000);
    }
  } else {
      display.setTextSize(2);
      display.setCursor(0,0); 
      display.print("DataBase disconected");
      display.display();
  }
  delete cur;
}

void identificationFacial()
{

}

void identificationQRcode()
{

}

