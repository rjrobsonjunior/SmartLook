#include <Keypad.h>

#define ROW_NUM     4 // four rows
#define COLUMN_NUM 4 // three columns
#define PinoRele 4


char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte pin_rows[ROW_NUM] = {32, 33, 25, 26}; // GIOP18, GIOP5, GIOP17, GIOP16 connect to the row pins
byte pin_column[COLUMN_NUM] = {27, 14, 12, 13};  // GIOP4, GIOP0, GIOP2 connect to the column pins

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

void setup() {
  Serial.begin(9600);

  pinMode(PinoRele, OUTPUT);
  digitalWrite(PinoRele, LOW);



}

void loop() {
  char key = keypad.getKey();
  
  if (key) {
    Serial.println(key);  
  }
  if(key == '5')
  {
    digitalWrite(PinoRele, HIGH);
    
  }
  else if(key == '6')
  {
    digitalWrite(PinoRele, LOW);
  }
  /*
  if(key == '1' || key == '2')
  {
    Serial.println("acesso liberado");
  }*/


}
