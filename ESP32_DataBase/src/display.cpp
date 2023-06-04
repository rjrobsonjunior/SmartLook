#include "display.h"

void setCursorMeio(Adafruit_SSD1306 display)
{
    display.setCursor(0, (display.height() - 8) / 2); // Define a posição do cursor no meio do display

}

void start_display(Adafruit_SSD1306 display)
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
    setCursorMeio(display);
    display.println("Fechadura Eletrônica");
    display.display();

    delay(500);
    display.setCursor(0, 24);
    display.println("Bem vindo...");
    display.display();

    delay(1500);


}

void display_acesso_liberado(Adafruit_SSD1306 display)
{
    display.clearDisplay();
    setCursorMeio(display);

    display.println("Acesso liberado!"); // Texto a ser exibido
    display.display();  
    
    delay(3000);

}

void display_acesso_negado(Adafruit_SSD1306 display)
{
    display.clearDisplay();
  
    setCursorMeio(display);
    display.println("Acesso negado!"); // Texto a ser exibido
    display.display();  
    
    delay(3000);
}

void display_home(Adafruit_SSD1306 display)
{
    display.clearDisplay();

    display.setCursor(0, 0);
    display.println("Menu de Selecao");

    display.setCursor(0, 16);
    display.println("A - Login e Senha");
    display.println("B - R. Facial");
    display.println("C - QR Code");
    display.display();  

    // Exibe o emoji de fechadura
    //display.drawBitmap(0, (display.height() - 8) / 2, lockIcon, 32, 16, 1);
    //display.display();
}
