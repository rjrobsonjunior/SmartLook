#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <MySQL_Encrypt_Sha1.h>
#include <MySQL_Packet.h>

#include <Wire.h> //INCLUSÃO DE BIBLIOTECA
#include <Adafruit_GFX.h> //INCLUSÃO DE BIBLIOTECA
#include <Adafruit_SSD1306.h> //INCLUSÃO DE BIBLIOTECA
 
Adafruit_SSD1306 display = Adafruit_SSD1306(); //OBJETO DO TIPO Adafruit_SSD1306
 
void setup(){
  Wire.begin(); //INICIALIZA A BIBLIOTECA WIRE
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //INICIALIZA O DISPLAY COM ENDEREÇO I2C 0x3C
  display.setTextColor(WHITE); //DEFINE A COR DO TEXTO
  display.setTextSize(2); //DEFINE O TAMANHO DA FONTE DO TEXTO
  display.clearDisplay(); //LIMPA AS INFORMAÇÕES DO DISPLAY
}
void loop() {
  display.setTextSize(2);
  display.setCursor(0,0); //POSIÇÃO EM QUE O CURSOR IRÁ FAZER A ESCRITA
  display.print("Identificacao:"); //ESCREVE O TEXTO NO DISPLAY
  display.display(); //EFETIVA A ESCRITA NO DISPLAY
  delay(1000); //INTERVALO DE 1,5 SEGUNDOS
  display.setTextSize(1.6);
  display.clearDisplay(); //LIMPA AS INFORMAÇÕES DO DISPLAY
  display.setCursor(2,2); //POSIÇÃO EM QUE O CURSOR IRÁ FAZER A ESCRITA
  display.print("1-QR code  2-Ident. Facial   3-Login e senha"); //ESCREVE O TEXTO NO DISPLAY
  display.display(); //EFETIVA A ESCRITA NO DISPLAY
  delay(1500); //INTERVALO DE 1,5 SEGUNDOS
  display.setTextSize(2);
  display.clearDisplay(); //LIMPA AS INFORMAÇÕES DO DISPLAY
  display.setCursor(10,10); //POSIÇÃO EM QUE O CURSOR IRÁ FAZER A ESCRITA
  display.print("NEGADO"); //ESCREVE O TEXTO NO DISPLAY
  display.display(); //EFETIVA A ESCRITA NO DISPLAY
  delay(1500); //INTERVALO DE 1,5 SEGUNDOS
  display.clearDisplay(); //LIMPA AS INFORMAÇÕES DO DISPLAY
  display.setCursor(10,10); //POSIÇÃO EM QUE O CURSOR IRÁ FAZER A ESCRITA
  display.print("LIBERADO"); //ESCREVE O TEXTO NO DISPLAY
  display.display(); //EFETIVA A ESCRITA NO DISPLAY
  delay(1500); //INTERVALO DE 1,5 SEGUNDOS
  display.clearDisplay(); //LIMPA AS INFORMAÇÕES DO DISPLAY
}
