#include <LiquidCrystal.h>
#include <string.h>

String login = "Login: 08578388577";
String senha = "Senha: ****";

LiquidCrystal lcd(12, 13, 7, 7, 5, 4);

void setup() {
  lcd.begin(16, 2);
  lcd.clear();
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print(login);
  lcd.setCursor(0, 1);
  lcd.print(senha);
}

