#include <WiFi.h>
#include <AsyncTCP.h>
#include <AsyncMySQL.h>

// Define as informações de conexão com o servidor MySQL remoto
char ssid[] = "Nome_da_rede_Wi-Fi";
char password[] = "Senha_da_rede_Wi-Fi";
char server_addr[] = "10.181.8.81";
char user[] = "usuario";
char password_mysql[] = "senha";
char db_name[] = "nome_do_banco";

// Cria um objeto WiFiClient e um objeto AsyncMySQLClient
WiFiClient client;
AsyncMySQLClient mysql;

void setup() {
  Serial.begin(9600);
  
  // Conecta-se à rede Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando à rede Wi-Fi...");
  }
  Serial.println("Conectado à rede Wi-Fi");

  // Tenta conectar ao servidor MySQL remoto
  mysql.connect(server_addr, 3306, user, password_mysql, db_name);
  if (mysql.connected()) {
    Serial.println("Conectado ao servidor MySQL");
  } else {
    Serial.println("Falha na conexão com o servidor MySQL");
    while (true);
  }
}

void loop() {
  // Executa uma consulta SQL para obter as informações de login e senha do usuário
  mysql.query("SELECT login, senha FROM tabela WHERE id = 1", [](AsyncMySQLResult result){
    int row_count = result.rowsCount();
    if (row_count > 0) {
      AsyncMySQLRow row = result.getRow(0);
      // Imprime as informações de login e senha no monitor serial
      Serial.print("Login: ");
      Serial.println(row.value(0));
      Serial.print("Senha: ");
      Serial.println(row.value(1));
    } else {
      Serial.println("Nenhum resultado encontrado");
    }
  });
  
  delay(10000);
}